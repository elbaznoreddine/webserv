#pragma once
#include <vector>
#include <string>
#include <map>
class LocationConfig;
class ServerConfig
{
protected:
    int port;
    std::string host;
    
    std::string root;
    std::vector<std::string> index_files;
    bool autoindex;
    std::vector<std::string> methods;
    std::map<int, std::string> error_pages;
    size_t client_max_body_size;

    std::vector<LocationConfig> locations;

public:
    ServerConfig();
    virtual ~ServerConfig();

    int getPort() const;
    const std::string& getHost() const;

    void setPort(int p);
    void setHost(const std::string& h);

    const std::string& getRoot() const;
    const std::vector<std::string>& getIndexFiles() const;
    bool getAutoindex() const;
    const std::vector<std::string>& getMethods() const;
    const std::map<int, std::string>& getErrorPages() const;
    size_t getClientMaxBodySize() const;

    void setRoot(const std::string& r);
    void addIndexFile(const std::string& i);
    void setAutoindex(bool a);
    void addMethod(const std::string& m);
    void setErrorPage(int code, const std::string& path);
    void setClientMaxBodySize(size_t size);
    
    void addLocation(const LocationConfig& loc);
    std::vector<LocationConfig>& getLocations();
};