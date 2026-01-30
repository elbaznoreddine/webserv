#pragma once

#include "ConfigParser.hpp"
class LocationConfig : public ServerConfig
{
private:
    std::string path;
    std::string cgi_extension;
    std::string cgi_path;
    std::string redirect;
    std::string upload_path;

public:
    LocationConfig();
    LocationConfig(const std::string& p);
    ~LocationConfig();

    const std::string& getPath() const;
    const std::string& getCgiExtension() const;
    const std::string& getCgiPath() const;
    const std::string& getRedirect() const;
    const std::string& getUploadPath() const;

    void setPath(const std::string& p);
    void setCgiExtension(const std::string& ext);
    void setCgiPath(const std::string& p);
    void setRedirect(const std::string& r);
    void setUploadPath(const std::string& p);
};