#pragma once

#include "ServerConfig.hpp"
class LocationConfig : public ServerConfig
{
private:
    std::string path;
    std::string redirect;
    std::string upload_path;
    bool upload_enable;
    int redirect_code;
    std::map<std::string, std::string> cgi_handlers;

public:
    LocationConfig();
    LocationConfig(const std::string& p);
    ~LocationConfig();

    const std::string& getPath() const;
    const std::string& getRedirect() const;
    const std::string& getUploadPath() const;
    bool getUploadEnable() const;
    int getRedirectCode() const;
    const std::map<std::string, std::string>& getCgiHandlers() const;
    void setPath(const std::string& p);
    void setRedirect(const std::string& r);
    void setUploadPath(const std::string& p);
    void setUploadEnable(bool enable);
    void setRedirectCode(int code);
    void addCgiHandler(const std::string& extension, const std::string& interpreter);
};