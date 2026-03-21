#pragma once
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include "Response.hpp"
#include "headers/ServerConfig.hpp"

class Request
{
	public:
		std::vector<std::string> vec;
		std::vector<std::string> folders;
		std::map<std::string, std::string> map;
		std::string body;
		std::string method;
		std::string path;
		std::string rpath;
		std::string protocol;
		std::string boundary;
		bool index;
		bool autoIndex;
		int fd;
	public:
		Response res;
		Request();
		~Request();
		void get(std::vector<LocationConfig>::iterator &it0);
		void post(std::stringstream &out, int fdo);
		void del();
		void parser(std::stringstream &ss, std::stringstream &out, int fdo, std::vector<ServerConfig> &servers);
		bool parseFirstLine(std::stringstream &ss);
		void parseHedear(std::stringstream &ss);
		std::string& getMethod();
		std::string& getStatus();
		void fullBody(std::stringstream &out, int fdo);
		void getBoundary();
		void postAction(std::stringstream &out);
};