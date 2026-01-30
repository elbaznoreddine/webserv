#include "../headers/Configparser.hpp"


int main(int argc, char* argv[]) {
	
	if (argc != 2) {
		std::cerr << "./webserv [configuration file]";
		return 1;
    }
	
	std::string config_file = argv[1];

    ConfigParser parser(config_file);
    
    if (!parser.parse()) {
        std::cerr << " Failed to parse config file!" << std::endl;
        return 1;
    }
    std::cout << "Config file parsed successfully!" << std::endl;
    return 0;
}