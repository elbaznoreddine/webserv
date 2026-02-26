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

std::string Response::make_res()
{
	std::string data0;
	std::string data1;
	data0 = this->protocol + " " + this->code + " " + this->msg + "\r\n";
	data0 += "Server: " + this->server + "\r\n";
	if (this->method == "GET" && this->code == "200")
	{
		if (this->autoIndex)
		{
			data1 = "<html>\n";
			data1 += "<body>\n";
			data1 += "<h1>index</h1>\n";
			for (std::vector<std::string>::iterator it = folders.begin(); it != folders.end(); it++)
				data1 += "<p><a href='/" + *it + "'>" + *it + "</a></p>\n";
			data1 += "</body>\n";
			data1 += "</html>\n";
			this->cType = "text/html";
			std::stringstream ss;
			ss << data1.length();
			this->cLength = ss.str();
		}
		else
		{
			std::ifstream inp(this->rpath.c_str(), std::ios::binary);
			std::string data((std::istreambuf_iterator<char>(inp)),std::istreambuf_iterator<char>());
			data1 = data;
			this->cType = this->getType(this->path);
			std::stringstream ss;
			ss << data1.length();
			this->cLength = ss.str();
		}
	}
	else if (this->method == "POST")
	{
		std::ofstream file("out", std::ios::binary);
		file << this->body;
	}
	else if (this->code != "200")
	{
		this->cType = "text/html";
		data1 = this->htmlPage();
		std::stringstream ss;
		ss << data1.length();
		this->cLength = ss.str();
	}
	this->connection = "close";
	if (this->method != "DELETE")
	{
		data0 += "Content-Type: " + this->cType + "\r\n";
		data0 += "Content-Length: " + this->cLength + "\r\n";
	}
	if (this->map.find("Cookie") != this->map.end())
		data0 += "Set-Cookie: " + this->map.find("Cookie")->second + ";\r\n";
	data0 += "Connection: " + this->connection + "\r\n";
	data0 +=  "\r\n";
	std::string str = data0 + data1;
	return str;
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