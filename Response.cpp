#include "Response.hpp"
#include <string>

Response::Response()
{
	this->protocol = "HTTP/1.0";
	this->server = "webserv";
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

void Response::make_res()
{
	std::string data1;
	this->res = this->protocol + " " + this->code + " " + this->msg + "\r\n";
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
		data1 = this->htmlPage();
		std::stringstream ss;
		ss << data1.length();
		this->cLength = ss.str();
	}
	if (this->method != "DELETE")
	{
		this->res += "Content-Type: " + this->cType + "\r\n";
		this->res += "Content-Length: " + this->cLength + "\r\n";
	}
	this->Cookie();
	this->res += "Connection: close\r\n";
	this->res +=  "\r\n";
	if (this->method == "DELETE")
		this->res += '\0';
	this->res +=  data1;
}

void Response::Cookie()
{
	if (this->map.find("Cookie") != this->map.end())
		this->res += "Set-Cookie: " + this->map.find("Cookie")->second + "\r\n";
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