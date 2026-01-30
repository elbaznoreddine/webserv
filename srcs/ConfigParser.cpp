#include "../headers/ConfigParser.hpp"


ConfigParser::ConfigParser(std::string& filename) : config_file(filename)
{
}

ConfigParser::~ConfigParser()
{
}

std::string ConfigParser::trim(std::string& str)
{
	size_t first = str.find_first_not_of(" \t\r\n");
	size_t last = str.find_last_not_of(" \t\r\n");
	if (first == std::string::npos || last == std::string::npos)
		return "";
	return str.substr(first, last - first + 1);
}