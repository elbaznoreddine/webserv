#pragma once
#include <vector>
#include <string>
#include <map>

struct Listen
{
    std::string host;
    int         port;

    Listen(const std::string& h, int p) : host(h), port(p) {}
};
class LocationConfig;
class ServerConfig
{
public:
    // int port;
    // std::string host;
	std::vector<Listen> all_listen;
	std::vector<std::string> hosts;
    std::string root;
    std::vector<std::string> index_files;
    bool autoindex;
    std::vector<std::string> methods;
    std::map<int, std::string> error_pages;
    std::string client_max_body_size;

    std::vector<LocationConfig> locations;

public:
    ServerConfig();
    virtual ~ServerConfig();

    // int getPort() const;
    // const std::string& getHost() const;
	const std::vector<Listen> &getListen() const;
	void addListen(const std::string& host, int port);

    // void setPort(int p);
    // void setHost(const std::string& h);

    const std::string& getRoot() const;
    const std::vector<std::string>& getIndexFiles() const;
    bool getAutoindex() const;
    const std::vector<std::string>& getMethods() const;
    const std::map<int, std::string>& getErrorPages() const;
    std::string getClientMaxBodySize() const;

    void setRoot(const std::string& r);
    void addIndexFile(const std::string& i);
    void setAutoindex(bool a);
    void addMethod(const std::string& m);
    void setErrorPage(int code, const std::string& path);
    void setClientMaxBodySize(std::string size);
    
    void addLocation(const LocationConfig& loc);
    std::vector<LocationConfig>& getLocations();
};