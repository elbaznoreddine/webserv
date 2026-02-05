#include "../headers/Configparser.hpp"

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./webserv [configuration file]" << std::endl;
		return 1;
	}
	
	std::string config_file = argv[1];
	
	try
	{
		ConfigParser parser(config_file);
		
		if (!parser.parse())
		{
			std::cerr << "Failed to parse configuration file" << std::endl;
			return 1;
		}
		
		const std::vector<ServerConfig>& servers = parser.getServers();
		std::cout << "Number of servers: " << servers.size() << std::endl;
		
		for (size_t i = 0; i < servers.size(); i++)
		{
			std::cout << "\nServer " << (i + 1) << ":" << std::endl;
			std::cout << "  Host: " << servers[i].getHost() << std::endl;
			std::cout << "  Port: " << servers[i].getPort() << std::endl;
			
			std::cout << "  Root: " << servers[i].getRoot() << std::endl;
			
			const std::vector<std::string>& index_files = servers[i].getIndexFiles();
			if (!index_files.empty())
			{
				std::cout << "  Index: ";
				for (size_t j = 0; j < index_files.size(); j++)
				{
					std::cout << index_files[j];
					if (j < index_files.size() - 1)
						std::cout << ", ";
				}
				std::cout << std::endl;
			}
			
			std::cout << "  Autoindex: " << (servers[i].getAutoindex() ? "on" : "off") << std::endl;
			std::cout << "  Client Max Body Size: " << servers[i].getClientMaxBodySize() << " bytes" << std::endl;
			
			const std::vector<std::string>& methods = servers[i].getMethods();
			if (!methods.empty())
			{
				std::cout << "  Allowed Methods: ";
				for (size_t j = 0; j < methods.size(); j++)
				{
					std::cout << methods[j];
					if (j < methods.size() - 1)
						std::cout << ", ";
				}
				std::cout << std::endl;
			}
			
			const std::map<int, std::string>& error_pages = servers[i].getErrorPages();
			if (!error_pages.empty())
			{
				std::cout << "  Error Pages:" << std::endl;
				for (std::map<int, std::string>::const_iterator it = error_pages.begin();
				     it != error_pages.end(); ++it)
				{
					std::cout << "    " << it->first << " -> " << it->second << std::endl;
				}
			}
			
			std::vector<LocationConfig>& locations = const_cast<ServerConfig&>(servers[i]).getLocations();
			if (!locations.empty())
			{
				std::cout << "  Locations:" << std::endl;
				for (size_t j = 0; j < locations.size(); j++)
				{
					std::cout << "    Path: " << locations[j].getPath() << std::endl;
					
					if (!locations[j].getRoot().empty())
						std::cout << "      Root: " << locations[j].getRoot() << std::endl;
					
					const std::vector<std::string>& loc_index = locations[j].getIndexFiles();
					if (!loc_index.empty())
					{
						std::cout << "      Index: ";
						for (size_t k = 0; k < loc_index.size(); k++)
						{
							std::cout << loc_index[k];
							if (k < loc_index.size() - 1)
								std::cout << ", ";
						}
						std::cout << std::endl;
					}
					
					std::cout << "      Autoindex: " << (locations[j].getAutoindex() ? "on" : "off") << std::endl;
					
					const std::vector<std::string>& loc_methods = locations[j].getMethods();
					if (!loc_methods.empty())
					{
						std::cout << "      Methods: ";
						for (size_t k = 0; k < loc_methods.size(); k++)
						{
							std::cout << loc_methods[k];
							if (k < loc_methods.size() - 1)
								std::cout << ", ";
						}
						std::cout << std::endl;
					}
					
					if (locations[j].getClientMaxBodySize() != 1048576)
						std::cout << "      Client Max Body Size: " << locations[j].getClientMaxBodySize() << " bytes" << std::endl;
					
					if (!locations[j].getCgiPath().empty())
						std::cout << "      CGI Path: " << locations[j].getCgiPath() << std::endl;
					
					if (!locations[j].getCgiExtension().empty())
						std::cout << "      CGI Extension: " << locations[j].getCgiExtension() << std::endl;
					
					const std::map<std::string, std::string>& cgi_handlers = locations[j].getCgiHandlers();
					if (!cgi_handlers.empty())
					{
						std::cout << "      CGI Handlers:" << std::endl;
						for (std::map<std::string, std::string>::const_iterator it = cgi_handlers.begin();
						     it != cgi_handlers.end(); ++it)
						{
							std::cout << "        " << it->first << " -> " << it->second << std::endl;
						}
					}
					
					if (!locations[j].getRedirect().empty())
					{
						std::cout << "      Redirect: ";
						if (locations[j].getRedirectCode() > 0)
							std::cout << locations[j].getRedirectCode() << " ";
						std::cout << locations[j].getRedirect() << std::endl;
					}
					
					if (!locations[j].getUploadPath().empty())
						std::cout << "      Upload Path: " << locations[j].getUploadPath() << std::endl;
					
					if (locations[j].getUploadEnable())
						std::cout << "      Upload: enabled" << std::endl;
				}
			}
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << "Error: " << e.what() << std::endl;
		return 1;
	}
	
	return 0;
}
// int main(int argc, char* argv[]) {
	
// 	if (argc != 2) {
// 		std::cerr << "./webserv [configuration file]";
// 		return 1;
//     }
	
// 	std::string config_file = argv[1];
// 	try
// 	{
// 		ConfigParser parser(config_file);
// 		parser.parse();
// 	}
// 	catch(const std::exception& e)
// 	{
// 		std::cerr << e.what() << '\n';
// 	}
//     return 0;
// }