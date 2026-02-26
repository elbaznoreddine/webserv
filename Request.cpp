#include "Request.hpp"

Request::Request()
{
	this->autoIndex = false;
	this->index = false;
}

Request::~Request()
{

}

std::string Request::parser(std::stringstream &ss)
{
	this->fd = 0;
	std::string line;
	std::getline(ss, line);

	std::stringstream f1(line);
	std::string word;

	while (f1 >> word)
		vec.push_back(word);
	if (vec.size() != 3 || (vec[0] != "GET" && vec[0] != "POST" && vec[0] != "DELETE") || (vec[2] != "HTTP/1.0" && vec[2] != "HTTP/1.1"))
	{
		this->res.setCode("400");
		this->res.setMsg("Bad Request");
		return this->res.make_res();
	}
	this->method = vec[0];
	this->path = vec[1];
	this->protocol = vec[2];
	while (std::getline(ss, line))
	{
		if (line[0] == '\r')
			break ;
		std::stringstream ss1(line);
		std::string key;
		std::string val;
		ss1 >> key;
		ss1 >> val;
		key = key.substr(0, key.size() - 1);
		map.insert(std::make_pair(key, val));
	}
	std::string data((std::istreambuf_iterator<char>(ss)),std::istreambuf_iterator<char>());
	this->body = data;
	if (this->method == "GET")
		get();
	if (this->method == "DELETE")
		del();
	if (this->method == "POST")
		post();
	this->res.setMethod(this->method);
	this->res.setPath(this->path);
	this->res.setRpath(this->rpath);
	this->res.setFolders(this->folders);
	this->res.setIndex(this->index);
	this->res.setAutoIndex(this->autoIndex);
	this->res.setBody(this->body);
	this->res.setMap(this->map);
	return this->res.make_res();
}

void Request::get()
{
	struct stat sb;
	this->rpath = "./html/" + this->path;
	stat(this->rpath.c_str(), &sb);
	switch (sb.st_mode & S_IFMT) {
		case S_IFDIR:
			if (this->rpath[this->rpath.size() - 1] != '/')
			{
				this->res.setCode("301");
				this->res.setMsg("Moved Permanently");
			}
			else if (this->index)
			{
				this->path = "index.html";
				this->get();
			}
			else if (this->autoIndex)
			{
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
				this->res.setCode("404");
				this->res.setMsg("Not Found");
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


void Request::post()
{
	this->res.setCode("200");
	this->res.setMsg("OK");
}

void Request::del()
{
	struct stat sb;
	this->rpath = "./html/" + this->path;
	stat(this->rpath.c_str(), &sb);
	switch (sb.st_mode & S_IFMT) {
		case S_IFDIR:
			if (this->rpath[this->rpath.size() - 1] != '/')
			{
				this->res.setCode("301");
				this->res.setMsg("Moved Permanently");
			}
			else if (!writeAccess)
			{
				this->res.setCode("301");
				this->res.setMsg("Moved Permanently");
			}
			else
			{
				DIR *dir;
				dir = opendir(this->rpath.c_str());
				struct dirent *ent;
				if (dir)
				{
					ent = readdir(dir);
					while (ent)
					{
						std::string str = this->rpath + ent->d_name;
						std::remove(str.c_str());
						ent = readdir(dir);
					}
					closedir(dir);
				}
				std::remove(this->rpath.c_str());
				this->res.setCode("200");
				this->res.setMsg("OK");
			}
			break;
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