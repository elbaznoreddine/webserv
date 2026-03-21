#include "Request.hpp"
#include "headers/Server.hpp"

Request::Request()
{
	this->autoIndex = false;
	this->index = false;
	this->fd = 0;
}

Request::~Request()
{

}

bool Request::parseFirstLine(std::stringstream &ss)
{
	std::string line;
	std::getline(ss, line);

	std::stringstream f1(line);
	std::string word;

	while (f1 >> word)
		this->vec.push_back(word);
	if (this->vec.size() != 3
	|| (this->vec[0] != "GET" && this->vec[0] != "POST" && this->vec[0] != "DELETE") 
	|| (this->vec[2] != "HTTP/1.0" && this->vec[2] != "HTTP/1.1"))
	{
		this->res.setCode("400");
		this->res.setMsg("Bad Request");
		this->res.make_res();
		return false;
	}
	this->method = this->vec[0];
	this->path = this->vec[1];
	this->protocol = this->vec[2];
	return true;
}

void Request::parseHedear(std::stringstream &ss)
{
	std::string line;
	while (std::getline(ss, line))
	{
		if (line[0] == '\r')
			break ;
		std::stringstream ss1(line);
		std::string key;
		std::string val;
		std::getline(ss1, key, ':');
		std::getline(ss1, val);
		this->map.insert(std::make_pair(key, val));
	}
}

void Request::parser(std::stringstream &ss, std::stringstream &out, int fdo, std::vector<ServerConfig> &servers)
{

	for (std::vector<ServerConfig>::iterator it = servers.begin(); it != servers.end(); it++)
	{
		for (std::vector<LocationConfig>::iterator it1 = it->locations.begin(); it1 != it->locations.end(); it1++)
		{
			// std::cout << "!!!!!!!!!!!!!!!!!\n";
			// std::cout << "auto index : " << it->autoindex << "\n";
			// std::cout << "max body size : " << it->client_max_body_size << "\n";
			// std::cout << "error_pages : " << "\n";
			// for (std::map<int, std::string>::iterator it2 = it1->error_pages.begin(); it2 != it1->error_pages.end(); it1++)
			// 	std::cout << it2->first << " :: " << it2->second << "\n";
			// std::cout << "index_files : " << "\n";
			// for (std::vector<std::string>::iterator it2 = it1->index_files.begin(); it2 != it1->index_files.end(); it2++)
			// 	std::cout << *it2 << "\n";
			// std::cout << "methods : " << "\n";
			// for (std::vector<std::string>::iterator it2 = it1->methods.begin(); it2 != it1->methods.end(); it2++)
			// 	std::cout << *it2 << "\n";
			// std::cout << "root : " << it1->root << "\n";
			// std::cout << "path : " << it1->path << "\n";
		}
	}

	if (!this->parseFirstLine(ss))
		return ;
	std::vector<LocationConfig>::iterator it0;
	std::vector<ServerConfig>::iterator it = servers.begin();
	for (it; it != servers.end(); it++)
	{
		std::vector<LocationConfig>::iterator it1 = it->locations.begin();
		it0 = it->locations.end();
		for (it1; it1 != it->locations.end(); it1++)
		{
			if (this->path.find(it1->path) != std::string::npos)
			{
				it0 = it1;
				break;
			}
		}	
	}
	if (it0 == it->locations.end())
	{
		this->res.setCode("404");
		this->res.setMsg("Not Found");
	}
	else
	{
		this->parseHedear(ss);
		if (this->method == "GET")
			get(it0);
		if (this->method == "DELETE")
			del();
		if (this->method == "POST")
			post(out, fdo);
	}
	this->res.setMethod(this->method);
	this->res.setPath(this->path);
	this->res.setRpath(this->rpath);
	this->res.setFolders(this->folders);
	this->res.setIndex(this->index);
	this->res.setAutoIndex(this->autoIndex);
	this->res.setBody(this->body);
	this->res.setMap(this->map);
	this->res.make_res();
	return ;
}

void Request::get(std::vector<LocationConfig>::iterator &it0)
{
	if (std::find(it0->methods.begin(), it0->methods.end(), "GET") == it0->methods.end())
	{
		this->res.setCode("403");
		this->res.setMsg("Forbidden");
		return ;
	}
	struct stat sb;
	this->rpath = it0->root + this->path;
	stat(this->rpath.c_str(), &sb);
	switch (sb.st_mode & S_IFMT) {
		case S_IFDIR:
			if (this->rpath[this->rpath.size() - 1] != '/')
			{
				this->res.setCode("301");
				this->res.setMsg("Moved Permanently");
			}
			else if (it0->index_files.size() > 0)
			{
				for (std::vector<std::string>::iterator itf = it0->index_files.begin(); itf != it0->index_files.end(); itf++)
				{
					std::string file = it0->root + "/" + *itf;
					int fdf = open(file.c_str(), O_RDONLY);
					if (fdf > 0)
					{
						close(fd);
						this->path = "/" + *itf;
						this->get(it0);
						return ;
					}
				}
			}
			else if (it0->autoindex)
			{
				this->autoIndex = true;
				DIR *dir;
				dir = opendir(this->rpath.c_str());
				struct dirent *ent;
				if (dir)
				{
					ent = readdir(dir);
					while (ent)
					{
						this->folders.push_back(ent->d_name);
						ent = readdir(dir);
					}
					closedir(dir);
				}
				this->res.setCode("200");
				this->res.setMsg("OK");
			}
			else
			{
				this->res.setCode("403");
				this->res.setMsg("Forbidden");
			}
			break;
		case S_IFREG:
			this->fd = open(this->rpath.c_str(), O_RDONLY);
			if (this->fd == -1)
			{
				this->res.setCode("403");
				this->res.setMsg("Forbidden");
				break;
			}
			close(this->fd);
			this->res.setCode("200");
			this->res.setMsg("OK");
			break;
		default:
			if (errno == 2)
			{
				this->res.setCode("404");
				this->res.setMsg("Not Found");
			}
			else
			{
				this->res.setCode("403");
				this->res.setMsg("Forbidden");
			}
	}
}

void Request::fullBody(std::stringstream &out, int fdo)
{
	char buffer[1024];
	std::memset(buffer, 0, sizeof(buffer));
	int bytes = recv(fdo, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
	while (bytes > 0)
	{
		for (int i = 0; i < bytes; i++)
			out << buffer[i];
		std::memset(buffer, 0, sizeof(buffer));
		bytes = recv(fdo, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
	}
}

void Request::getBoundary()
{
	std::stringstream ct(this->map["Content-Type"]);
	ct >> this->boundary;
	ct >>	this->boundary;
	this->boundary.erase(0, boundary.find("=") + 1);
	this->boundary = "--" + this->boundary;
}

void  Request::postAction(std::stringstream &out)
{
	std::string line;
	std::string filename;
	while (std::getline(out, line))
	{
		if (line.find(this->boundary + "--\r") != std::string::npos || line.empty())
			break ;
		else if (line.find("filename") != std::string::npos)
		{
			filename = line.substr(line.find("filename") + 10);
			filename.erase(filename.size() - 2);
		}
		else if (line == "\r" && !filename.empty())
		{
			std::ofstream output(filename, std::ios::binary);
			std::stringstream out1;
			while (std::getline(out, line))
			{
				if (line.find(this->boundary + "\r") != std::string::npos || line.find(this->boundary + "--\r") != std::string::npos)
					break ;
				out1 << line << std::endl;
			}
			std::string s = out1.str();
			s.erase(s.size() - 2);
			output << s;
			filename = "";
		}
	}
}

void Request::post(std::stringstream &out, int fdo)
{
	this->fullBody(out, fdo);

	// for (size_t i = 0; i < out.str().size(); i++)
	// {
	// 	if (out.str()[i] == '\r')
	// 		std::cout << "\\r";
	// 	else if (out.str()[i] == '\n')
	// 		std::cout << "\\n\n";
	// 	else
	// 		std::cout << out.str()[i];
	// }

	this->getBoundary();
	this->postAction(out);
	this->res.setCode("200");
	this->res.setMsg("OK");
	std::stringstream ss;
	ss << out.str().size();
	this->res.cLength = ss.str();
	this->res.cType = "text/plain";
}

void Request::del()
{
	struct stat sb;
	this->rpath = "./html/" + this->path;
	stat(this->rpath.c_str(), &sb);
	switch (sb.st_mode & S_IFMT) {
		case S_IFREG:
			this->fd = open(this->rpath.c_str(), O_RDONLY);
			if (this->fd == -1)
			{
				this->res.setCode("404");
				this->res.setMsg("Not Found");
				break;
			}
			close(this->fd);
			std::remove(this->rpath.c_str());
			this->res.setCode("200");
			this->res.setMsg("OK");
			break;
		default:
			this->res.setCode("403");
			this->res.setMsg("Forbidden");
	}
}

std::string& Request::getMethod()
{
	return this->method;
}

std::string& Request::getStatus()
{
	return this->res.getStatus();
}