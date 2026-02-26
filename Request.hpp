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

class Request
{
	private:
		std::vector<std::string> vec;
		std::vector<std::string> folders;
		std::map<std::string, std::string> map;
		std::string body;
		std::string method;
		std::string path;
		std::string rpath;
		std::string protocol;
		bool index;
		bool autoIndex;
		bool writeAccess;
		int fd;
		Response res;
	public:
		Request();
		~Request();
		void get();
		void post();
		void del();
		std::string parser(std::stringstream &ss);
};