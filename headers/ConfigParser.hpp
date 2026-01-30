#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"
class ConfigParser
{
	private:
		std::vector<ServerConfig> servers;
		std::string config_file;
		std::string trim(std::string& str);
		std::string extractLocationPath(const std::string& line);
		void parseServerDirective(const std::string& line, ServerConfig& server);
		void parseLocationDirective(const std::string& line, LocationConfig& location);
		bool validateConfig();

	public:
		ConfigParser(std::string& filename);
		~ConfigParser();
		bool parse();
		std::vector<ServerConfig>& getServers();
		void printConfig();
};