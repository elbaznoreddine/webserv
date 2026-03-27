#pragma once
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "headers/LocationConfig.hpp"

class Request;
class Response
{
	public:
		std::string code;
		std::string msg;
		std::string res;

		std::string server;
		std::string cType;
		std::string cLength;

		std::string body;
		std::string location;
	public:	
		Response();
		~Response();
		void make_res(ServerConfig &server, Request& req);
		void make_res(std::vector<LocationConfig>::iterator &it0, Request& req);
		std::string htmlPage();
		void setCode(std::string str);
		void setMsg(std::string str);
		void setBody(std::string str);
		std::string getType(std::string& str);
		std::string getRes();
		void Cookie(Request& req);
		void autoIndexFun(std::string &data, Request& req);
		void readFile(std::string &data, Request& req);
		std::string& getStatus();
		void setRedirect(std::string &loc);
};