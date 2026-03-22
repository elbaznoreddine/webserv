#pragma once
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include "headers/LocationConfig.hpp"

class Response
{
	public:
		std::string protocol;
		std::string code;
		std::string msg;
		std::string res;

		std::string server;
		std::string cType;
		std::string cLength;

		std::string method;
		std::string path;
		std::string rpath;
		std::string body;
		std::vector<std::string> folders;
		std::map<std::string, std::string> map;
		bool index;
		bool autoIndex;
		std::string location;
	public:	
		Response();
		~Response();
		void make_res(std::vector<LocationConfig>::iterator &it0);
		std::string htmlPage();
		void setCode(std::string str);
		void setMsg(std::string str);
		void setMethod(std::string str);
		void setPath(std::string str);
		void setRpath(std::string str);
		void setBody(std::string str);
		void setFolders(std::vector<std::string>& vec);
		void setIndex(bool b);
		void setMap(std::map<std::string, std::string>& map);
		void setAutoIndex(bool b);
		std::string getType(std::string& str);
		std::string getRes();
		void Cookie();
		void autoIndexFun(std::string &data);
		void readFile(std::string &data);
		std::string& getStatus();
		void setRedirect(std::string &loc);
};