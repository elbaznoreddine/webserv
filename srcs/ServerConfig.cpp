#include "../headers/ConfigParser.hpp"

ServerConfig::ServerConfig() 
    : port(8080), 
      host("0.0.0.0"), 
      autoindex(false), 
      client_max_body_size(1048576)
{
}

ServerConfig::~ServerConfig()
{
}

int ServerConfig::getPort() const
{
    return port;
}

const std::string& ServerConfig::getHost() const
{
    return host;
}

void ServerConfig::setPort(int p)
{
    port = p;
}

void ServerConfig::setHost(const std::string& h)
{
    host = h;
}

const std::string& ServerConfig::getRoot() const
{
    return root;
}

const std::string& ServerConfig::getIndex() const
{
    return index;
}

bool ServerConfig::getAutoindex() const
{
    return autoindex;
}

const std::vector<std::string>& ServerConfig::getMethods() const
{
    return methods;
}

const std::map<int, std::string>& ServerConfig::getErrorPages() const
{
    return error_pages;
}

size_t ServerConfig::getClientMaxBodySize() const
{
    return client_max_body_size;
}


void ServerConfig::setRoot(const std::string& r)
{
    root = r;
}

void ServerConfig::setIndex(const std::string& i)
{
    index = i;
}

void ServerConfig::setAutoindex(bool a) {
    autoindex = a;
}

void ServerConfig::addMethod(const std::string& m)
{
    methods.push_back(m);
}

void ServerConfig::setErrorPage(int code, const std::string& path)
{
    error_pages[code] = path;
}

void ServerConfig::setClientMaxBodySize(size_t size)
{
    client_max_body_size = size;
}


void ServerConfig::addLocation(const LocationConfig& loc)
{
    locations.push_back(loc);
}

std::vector<LocationConfig>& ServerConfig::getLocations()
{
    return locations;
}