#include "Response.hpp"
#include <string>

Response::Response()
{
	this->protocol = "HTTP/1.0";
	this->server = "webserv";
	this->cLength = "0";
}

Response::~Response()
{

}

void Response::autoIndexFun(std::string &data)
{
	data = "<html>\n";
	data += "<body>\n";
	data += "<h1>index</h1>\n";
	for (std::vector<std::string>::iterator it = folders.begin(); it != folders.end(); it++)
		data += "<p><a href='/" + *it + "'>" + *it + "</a></p>\n";
	data += "</body>\n";
	data += "</html>\n";
	this->cType = "text/html";
	std::stringstream ss;
	ss << data.length();
	this->cLength = ss.str();
}

void Response::readFile(std::string &data)
{
	std::ifstream inp(this->rpath.c_str(), std::ios::binary);
	std::string file((std::istreambuf_iterator<char>(inp)),std::istreambuf_iterator<char>());
	data = file;
	this->cType = this->getType(this->path);
	std::stringstream ss;
	ss << data.length();
	this->cLength = ss.str();
}

void Response::make_res(std::vector<LocationConfig>::iterator &it0)
{
	std::string data1 = "";
	this->res = this->protocol + " " + this->code + " " + this->msg + "\r\n";
	if (!this->location.empty())
		this->res += "Location: " + this->location + "\r\n";
	this->res += "Server: " + this->server + "\r\n";
	if (this->method == "GET" && this->code == "200")
	{
		if (this->autoIndex)
			this->autoIndexFun(data1);
		else
			this->readFile(data1);
	}
	else if (this->code != "200")
	{
		this->cType = "text/html";
		std::map<int, std::string>::iterator itm = it0->error_pages.find(std::atoi(this->code.c_str()));
		if (itm != it0->error_pages.end())
		{
			std::ifstream inp(itm->second.c_str(), std::ios::binary);
			std::string file((std::istreambuf_iterator<char>(inp)),std::istreambuf_iterator<char>());
			data1 = file;
		}
		else
			data1 = this->htmlPage();
		std::stringstream ss;
		ss << data1.length();
		this->cLength = ss.str();
	}
	if (this->method == "POST")
		this->cLength = "0";
	if (this->method == "GET")
		this->res += "Content-Type: " + this->cType + "\r\n";
	this->res += "Content-Length: " + this->cLength + "\r\n";
	this->Cookie();
	this->res += "Connection: close\r\n";
	this->res +=  "\r\n";
	if (!data1.empty())
		this->res +=  data1;
}

void Response::Cookie()
{
	if (this->map.find("Cookie") != this->map.end())
		if (this->map.find("Cookie")->second.find("login=1"))
			this->res += "Set-Cookie: sid=1\r\n";
}

std::string Response::htmlPage()
{
	std::string html;
	html = "<html>\n";
	html += "<head><title>" + this->code + " " + this->msg + "</title></head>\n";
	html += "<body>\n";
	html += "<center><h1>" + this->code + " " + this->msg + "</h1></center>\n";
	html += "<hr><center>" + this->server + "</center>\n";
	html += "</body>\n";
	html += "</html>\n";
	return html;
}

void Response::setCode(std::string str)
{
	this->code = str;
}

void Response::setMsg(std::string str)
{
	this->msg = str;
}

void Response::setMethod(std::string str)
{
	this->method = str;
}

void Response::setPath(std::string str)
{
	this->path = str;
}

void Response::setRpath(std::string str)
{
	this->rpath = str;
}

void Response::setFolders(std::vector<std::string>& vec)
{
	this->folders = vec;
}

void Response::setIndex(bool b)
{
	this->index = b;
}

void Response::setAutoIndex(bool b)
{
	this->autoIndex = b;
}

void Response::setBody(std::string str)
{
	this->body = str;
}

std::string Response::getType(std::string& str)
{
	if (str.find(".html") != std::string::npos)
		return "text/html";
	return "text/plain";
}

void Response::setMap(std::map<std::string, std::string>& map)
{
	this->map = map;
}

std::string Response::getRes()
{
	return this->res;
}

std::string& Response::getStatus()
{
	return this->code;
}

void Response::setRedirect(std::string &loc)
{
	this->location = loc;
}