#include "../headers/ConfigParser.hpp"

LocationConfig::LocationConfig() : ServerConfig()
{
}

LocationConfig::LocationConfig(const std::string& p) : ServerConfig(), path(p)
{
}

LocationConfig::~LocationConfig()
{
}

const std::string& LocationConfig::getPath() const
{
    return path;
}

const std::string& LocationConfig::getCgiExtension() const
{
    return cgi_extension;
}

const std::string& LocationConfig::getCgiPath() const
{
    return cgi_path;
}

const std::string& LocationConfig::getRedirect() const
{
    return redirect;
}

const std::string& LocationConfig::getUploadPath() const
{
    return upload_path;
}

void LocationConfig::setPath(const std::string& p)
{
    path = p;
}

void LocationConfig::setCgiExtension(const std::string& ext)
{
    cgi_extension = ext;
}

void LocationConfig::setCgiPath(const std::string& p)
{
    cgi_path = p;
}

void LocationConfig::setRedirect(const std::string& r)
{
    redirect = r;
}

void LocationConfig::setUploadPath(const std::string& p)
{
    upload_path = p;
}