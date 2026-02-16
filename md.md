📐 Class Diagram
┌─────────────────────────────────────────────────────┐
│              ServerConfig (Base Class)              │
├─────────────────────────────────────────────────────┤
│ Protected Attributes:                               │
│   # int port                                        │
│   # string host                                     │
│   # string root                                     │
│   # string index                                    │
│   # bool autoindex                                  │
│   # vector<string> methods                          │
│   # map<int, string> error_pages                    │
│   # size_t client_max_body_size                     │
├─────────────────────────────────────────────────────┤
│ Public Methods:                                     │
│   + ServerConfig()                                  │
│   + ~ServerConfig()                                 │
│   + getPort(), setPort()                            │
│   + getHost(), setHost()                            │
│   + getRoot(), setRoot()                            │
│   + getIndex(), setIndex()                          │
│   + getAutoindex(), setAutoindex()                  │
│   + getMethods(), addMethod()                       │
│   + getErrorPages(), setErrorPage()                 │
│   + getClientMaxBodySize(), setClientMaxBodySize()  │
└─────────────────────────────────────────────────────┘
                        △
                        │
                        │ inherits
                        │
┌─────────────────────────────────────────────────────┐
│         LocationConfig : public ServerConfig        │
├─────────────────────────────────────────────────────┤
│ Private Attributes:                                 │
│   - string path                                     │
│   - string cgi_extension                            │
│   - string cgi_path                                 │
│   - string redirect                                 │
│   - string upload_path                              │
├─────────────────────────────────────────────────────┤
│ Public Methods:                                     │
│   + LocationConfig()                                │
│   + LocationConfig(path)                            │
│   + ~LocationConfig()                               │
│   + getPath(), setPath()                            │
│   + getCgiExtension(), setCgiExtension()            │
│   + getCgiPath(), setCgiPath()                      │
│   + getRedirect(), setRedirect()                    │
│   + getUploadPath(), setUploadPath()                │
│   + (inherits all ServerConfig methods)             │
└─────────────────────────────────────────────────────┘


┌─────────────────────────────────────────────────────┐
│                   ConfigParser                      │
├─────────────────────────────────────────────────────┤
│ Private Attributes:                                 │
│   - vector<ServerConfig> servers                    │
│   - string config_file                              │
├─────────────────────────────────────────────────────┤
│ Private Methods:                                    │
│   - trim()                                          │
│   - extractLocationPath()                           │
│   - parseServerDirective()                          │
│   - parseLocationDirective()                        │
│   - validateConfig()                                │
├─────────────────────────────────────────────────────┤
│ Public Methods:                                     │
│   + ConfigParser(filename)                          │
│   + ~ConfigParser()                                 │
│   + parse()                                         │
│   + getServers()                                    │
│   + printConfig()                                   │
└─────────────────────────────────────────────────────┘
🎯 Relationships
ConfigParser
    │
    ├── HAS MANY ──→ ServerConfig
                         │
                         ├── HAS MANY ──→ LocationConfig
                         │                      │
                         │                      │
                         └──────────────────────┘
                              INHERITS FROM
Attribute Distribution
ServerConfig has ALL attributes:
cpp// Server-only attributes
- port
- host
- server_names(i must check this)

// Common attributes (inherited by Location)
- root
- index
- autoindex
- methods
- error_pages
- client_max_body_size
LocationConfig inherits everything + adds:
cpp// Inherits from ServerConfig:
- root, index, autoindex, methods, error_pages, client_max_body_size

// Location-only attributes:
- path
- cgi_extension
- cgi_path
- redirect
- upload_path
```

### 🔄 Data Flow
```
1. ConfigParser reads file
        ↓
2. Creates ServerConfig objects
        ↓
3. Each ServerConfig contains LocationConfig objects
        ↓
4. LocationConfig inherits common attributes from ServerConfig
        ↓
5. Can override inherited values


### Where can each directive appear?

```
┌─────────────────────────────────────────────────────────────┐
│ CONTEXT HIERARCHY                                           │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  Server Block { }                                           │
│  ├── Server-only directives                                 │
│  ├── Directives that can be in both server and location     │
│  └── Location Block { }                                     │
│      └── Location-specific directives                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## SERVER BLOCK DIRECTIVES

### Can ONLY appear in `server { }`:

```nginx
server {
    # 1. LISTEN (REQUIRED) - Which port/address to listen on
    listen 8080;
    listen 127.0.0.1:8080;
    listen localhost:9090;
    
    # 2. SERVER_NAME (Optional) - Domain names for this server
    server_name example.com www.example.com localhost;
    server_name _;  # Default server (catch-all)
}
```

### Can appear in BOTH `server { }` and `location { }`:

```nginx
server {
    # 3. CLIENT_MAX_BODY_SIZE - Max size of request body
    client_max_body_size 10M;
    client_max_body_size 1024;      # bytes
    client_max_body_size 5K;        # kilobytes
    client_max_body_size 2G;        # gigabytes
    
    # 4. ERROR_PAGE - Custom error pages
    error_page 404 /404.html;
    error_page 500 502 503 504 /50x.html;
    error_page 403 /errors/forbidden.html;
    
    location /uploads {
        # Can override server's client_max_body_size
        client_max_body_size 50M;
    }
}
```

---

## LOCATION BLOCK DIRECTIVES

### Can ONLY appear in `location { }`:

```nginx
location /path {
    # 1. ROOT - Root directory for file serving
    root /var/www/html;
    root /usr/share/nginx/html;
    
    # 2. INDEX - Default files when requesting a directory
    index index.html index.htm;
    index home.html default.html;
    index index.php index.html;
    
    # 3. ALLOW_METHODS / ALLOWED_METHODS - HTTP methods allowed
    allow_methods GET POST DELETE;
    allow_methods GET;
    allowed_methods GET POST PUT;  # Alternative name
    
    # 4. AUTOINDEX - Enable/disable directory listing
    autoindex on;
    autoindex off;
    
    # 5. RETURN - HTTP redirection
    return 301 /new-path;
    return 302 https://example.com;
    return 301 https://www.example.com$request_uri;
    
    # 6. CGI_PASS / CGI_EXTENSION - CGI configuration
    cgi_pass .php /usr/bin/php-cgi;
    cgi_pass .py /usr/bin/python3;
    cgi_extension .php;
    cgi_path /usr/bin/php-cgi;
    
    # 7. UPLOAD_ENABLE - Enable file uploads
    upload_enable on;
    upload_enable off;
    
    # 8. UPLOAD_PATH / UPLOAD_STORE - Where to store uploaded files
    upload_path /var/www/uploads;
    upload_store /tmp/uploads;
}
```

---

## COMPLETE DIRECTIVE LIST BY CONTEXT

### SERVER CONTEXT ONLY:

| Directive | Required? | Format | Example |
|-----------|-----------|--------|---------|
| `listen` | **YES** | `listen port;` or `listen host:port;` | `listen 8080;` |
| `server_name` | No | `server_name name1 name2 ...;` | `server_name localhost;` |

### BOTH SERVER AND LOCATION:

| Directive | Format | Example |
|-----------|--------|---------|
| `client_max_body_size` | `client_max_body_size size;` | `client_max_body_size 10M;` |
| `error_page` | `error_page code ... path;` | `error_page 404 /404.html;` |

### LOCATION CONTEXT ONLY:

| Directive | Format | Example |
|-----------|--------|---------|
| `root` | `root path;` | `root /var/www;` |
| `index` | `index file1 file2 ...;` | `index index.html;` |
| `allow_methods` | `allow_methods METHOD ...;` | `allow_methods GET POST;` |
| `autoindex` | `autoindex on\|off;` | `autoindex on;` |
| `return` | `return code url;` | `return 301 /new;` |
| `cgi_pass` | `cgi_pass ext handler;` | `cgi_pass .php /usr/bin/php;` |
| `upload_enable` | `upload_enable on\|off;` | `upload_enable on;` |
| `upload_path` | `upload_path dir;` | `upload_path /uploads;` |

---

## DIRECTIVE DETAILS

### 1. listen (SERVER ONLY - REQUIRED)

**Purpose:** Define which port and/or host the server listens on

**Formats:**
```nginx
listen port;                    # Listen on all interfaces, specific port
listen host:port;               # Listen on specific host and port
listen *:port;                  # Explicit: all interfaces
```

**Examples:**
```nginx
listen 8080;                    # All interfaces, port 8080
listen 80;                      # HTTP default port
listen 443;                     # HTTPS default port
listen 127.0.0.1:8080;         # Localhost only
listen localhost:9090;          # Same as above
listen 0.0.0.0:3000;           # Explicit all interfaces
```

**Multiple listen directives:**
```nginx
server {
    listen 80;                  # HTTP
    listen 443;                 # HTTPS
    listen 8080;                # Alternative port
}
```

**What your code needs to do:**
- Parse the port number (required)
- Parse the optional host/IP (default: 0.0.0.0 = all interfaces)
- Create socket and bind to this address:port
- Can have multiple listen directives = multiple sockets

---

### 2. server_name (SERVER ONLY - OPTIONAL)

**Purpose:** Define domain names for virtual hosting

**Format:**
```nginx
server_name name1 name2 name3 ...;
```

**Examples:**
```nginx
server_name example.com;
server_name example.com www.example.com;
server_name *.example.com;                    # Wildcard (if you implement)
server_name .example.com;                     # Matches example.com and *.example.com
server_name "";                               # Empty name
server_name _;                                # Default/catch-all server
```

**What your code needs to do:**
- If empty: This server is the default for this port
- If not empty: Match incoming Host header against these names
- Multiple servers on same port → select by Host header
- No match → use first server or one with server_name _

---

### 3. client_max_body_size (SERVER & LOCATION - OPTIONAL)

**Purpose:** Limit the size of client request body (file uploads, POST data)

**Format:**
```nginx
client_max_body_size size;
```

**Examples:**
```nginx
client_max_body_size 1M;        # 1 megabyte
client_max_body_size 100K;      # 100 kilobytes
client_max_body_size 2G;        # 2 gigabytes
client_max_body_size 1024;      # 1024 bytes (no unit)
client_max_body_size 0;         # Unlimited (not recommended)
```

**Units:**
- No unit = bytes
- K or k = kilobytes (1024 bytes)
- M or m = megabytes (1024 * 1024 bytes)
- G or g = gigabytes (1024 * 1024 * 1024 bytes)

**Inheritance:**
```nginx
server {
    client_max_body_size 10M;    # Default for all locations
    
    location / {
        # Uses 10M from server
    }
    
    location /uploads {
        client_max_body_size 50M; # Override: 50M for this location
    }
}
```

**What your code needs to do:**
- If not specified: Use default (e.g., 1M or unlimited)
- When receiving request: Check Content-Length header
- If body too large: Return 413 Payload Too Large
- Location value overrides server value

---

### 4. error_page (SERVER & LOCATION - OPTIONAL)

**Purpose:** Serve custom HTML pages for HTTP errors

**Format:**
```nginx
error_page code [code ...] uri;
```

**Examples:**
```nginx
error_page 404 /404.html;
error_page 500 502 503 504 /50x.html;           # Multiple codes
error_page 403 /errors/forbidden.html;
error_page 404 =200 /empty.html;                # Change response code (advanced)
```

**What your code needs to do:**
- When error occurs (404, 500, etc.)
- Check if custom error page exists for this code
- If yes: Serve that file
- If no: Generate default error page
- Make sure the error page file actually exists

---

### 5. root (LOCATION ONLY - OPTIONAL)

**Purpose:** Define root directory for file serving

**Format:**
```nginx
root path;
```

**Examples:**
```nginx
root /var/www/html;
root /usr/share/nginx/html;
root /home/user/website;
```

**How it works:**
```nginx
location /images {
    root /var/www;
}

Request: GET /images/photo.jpg
File path: /var/www/images/photo.jpg
           └─ root ─┘└─ uri path ─┘
```

**Important:** The location path is APPENDED to root!

**What your code needs to do:**
- Combine root + request URI to get file path
- Check if file exists
- Serve the file or return 404

---

### 6. index (LOCATION ONLY - OPTIONAL)

**Purpose:** Default file(s) to serve when URI is a directory

**Format:**
```nginx
index file1 file2 file3 ...;
```

**Examples:**
```nginx
index index.html;
index index.html index.htm;
index index.php index.html default.html;
```

**How it works:**
```nginx
location / {
    root /var/www;
    index index.html index.htm;
}

Request: GET /
Try: /var/www/index.html  → If exists, serve it
     /var/www/index.htm   → If exists, serve it
     Otherwise → 403 Forbidden or show directory listing (if autoindex on)
```

**What your code needs to do:**
- If URI ends with / (directory)
- Try each index file in order
- Serve first one that exists
- If none exist: Show directory listing (if autoindex on) or 403

---

### 7. allow_methods / allowed_methods (LOCATION ONLY - OPTIONAL)

**Purpose:** Specify which HTTP methods are allowed for this location

**Format:**
```nginx
allow_methods METHOD1 METHOD2 ...;
allowed_methods METHOD1 METHOD2 ...;  # Alternative name
```

**Examples:**
```nginx
allow_methods GET;                    # Read-only
allow_methods GET POST;               # Read and create
allow_methods GET POST DELETE;        # Read, create, delete
allow_methods GET POST PUT DELETE;    # Full CRUD
```

**Common HTTP methods:**
- GET - Retrieve resource
- POST - Create/upload resource
- DELETE - Remove resource
- PUT - Update resource (optional for webserv)
- HEAD - Like GET but no body (optional)

**What your code needs to do:**
- Check incoming request method
- If not in allowed list: Return 405 Method Not Allowed
- Include "Allow" header in 405 response with allowed methods

**Default if not specified:** GET only (safest) or GET POST

---

### 8. autoindex (LOCATION ONLY - OPTIONAL)

**Purpose:** Enable/disable automatic directory listing

**Format:**
```nginx
autoindex on;
autoindex off;
```

**How it works:**
```nginx
location /files {
    root /var/www;
    autoindex on;
}

Request: GET /files/
If /var/www/files/ is a directory and no index file:
  → Generate HTML page listing directory contents
  
If autoindex off:
  → Return 403 Forbidden
```

**What your code needs to do:**
- When directory requested and no index file found
- If autoindex on: Generate HTML with links to files/subdirectories
- If autoindex off: Return 403 Forbidden

**Directory listing HTML example:**
```html
<html>
<head><title>Index of /files/</title></head>
<body>
<h1>Index of /files/</h1>
<ul>
<li><a href="file1.txt">file1.txt</a></li>
<li><a href="file2.pdf">file2.pdf</a></li>
<li><a href="subdir/">subdir/</a></li>
</ul>
</body>
</html>
```

**Default if not specified:** off (more secure)

---

### 9. return (LOCATION ONLY - OPTIONAL)

**Purpose:** HTTP redirection - send client to different URL

**Format:**
```nginx
return code URL;
```

**Examples:**
```nginx
return 301 /new-location;                    # Permanent redirect (same site)
return 302 /temporary;                       # Temporary redirect
return 301 https://www.example.com;          # Redirect to external site
return 301 https://example.com$request_uri;  # Preserve path (advanced)
```

**Common redirect codes:**
- 301 - Moved Permanently (SEO-friendly, cached by browsers)
- 302 - Found (Temporary redirect)
- 303 - See Other
- 307 - Temporary Redirect (method preserved)
- 308 - Permanent Redirect (method preserved)

### 10. cgi_pass / cgi_extension (LOCATION ONLY - OPTIONAL)

**Purpose:** Execute CGI scripts for dynamic content

**Format:**
```nginx
cgi_pass extension interpreter_path;
cgi_extension extension;
cgi_path interpreter_path;
```

**Examples:**
```nginx
cgi_pass .php /usr/bin/php-cgi;
cgi_pass .py /usr/bin/python3;
cgi_pass .pl /usr/bin/perl;

# Alternative format:
cgi_extension .php;
cgi_path /usr/bin/php-cgi;
```

**How it works:**
```nginx
location /cgi-bin {
    root /var/www;
    cgi_pass .php /usr/bin/php-cgi;
}

Request: GET /cgi-bin/script.php
File: /var/www/cgi-bin/script.php
→ Execute with: /usr/bin/php-cgi /var/www/cgi-bin/script.php
→ Return output to client
```

**What your code needs to do:**
1. Check file extension (.php, .py, etc.)
2. If matches CGI extension:
   - Fork process
   - Set CGI environment variables (see below)
   - Execute interpreter with script path
   - Pipe request body to CGI stdin
   - Read CGI output from stdout
   - Parse CGI output (headers + body)
   - Send response to client
3. If no match: Serve as static file

**CGI Environment Variables (Must set):**
```
REQUEST_METHOD=GET
QUERY_STRING=name=value&foo=bar
CONTENT_TYPE=application/x-www-form-urlencoded
CONTENT_LENGTH=42
SCRIPT_NAME=/cgi-bin/script.php
SCRIPT_FILENAME=/var/www/cgi-bin/script.php
PATH_INFO=/extra/path
REQUEST_URI=/cgi-bin/script.php?query=1
SERVER_PROTOCOL=HTTP/1.1
SERVER_NAME=localhost
SERVER_PORT=8080
REMOTE_ADDR=127.0.0.1
```

**CGI Output Format:**
```
Content-Type: text/html

<html>
<body>Hello from CGI!</body>
</html>
```

---

### 11. upload_enable (LOCATION ONLY - OPTIONAL)

**Purpose:** Enable/disable file uploads for this location

**Format:**
```nginx
upload_enable on;
upload_enable off;
```

**Example:**
```nginx
location /uploads {
    upload_enable on;
    upload_path /var/www/uploads;
    allow_methods POST DELETE;
}
```

**What your code needs to do:**
- If upload_enable off: Don't allow file uploads (return 403)
- If upload_enable on: Allow POST requests with file data
- Save uploaded files to upload_path directory

**Default if not specified:** off

---

### 12. upload_path / upload_store (LOCATION ONLY - OPTIONAL)

**Purpose:** Directory where uploaded files are stored

**Format:**
```nginx
upload_path /path/to/directory;
upload_store /path/to/directory;  # Alternative name
```

**Examples:**
```nginx
upload_path /var/www/uploads;
upload_path /tmp/user_files;
```

**What your code needs to do:**
- When receiving POST with file upload
- Save file to this directory
- Generate unique filename or use provided filename
- Create directory if it doesn't exist (optional)
- Check write permissions

---

## COMPLETE CONFIG EXAMPLE WITH ALL DIRECTIVES

```nginx
server {
    # SERVER ONLY DIRECTIVES (REQUIRED)
    listen 8080;
    listen 127.0.0.1:9090;
    
    # SERVER ONLY DIRECTIVES (OPTIONAL)
    server_name example.com www.example.com;
    
    # CAN BE IN SERVER OR LOCATION
    client_max_body_size 10M;
    error_page 404 /404.html;
    error_page 500 502 503 /50x.html;
    
    # LOCATION 1: Root
    location / {
        root /var/www/html;
        index index.html index.htm;
        allow_methods GET POST;
        autoindex off;
    }
    
    # LOCATION 2: File uploads
    location /uploads {
        root /var/www/uploads;
        allow_methods GET POST DELETE;
        upload_enable on;
        upload_path /var/www/uploads;
        client_max_body_size 50M;  # Override server default
        autoindex on;
    }
    
    # LOCATION 3: CGI scripts
    location /cgi-bin {
        root /var/www/cgi;
        cgi_pass .php /usr/bin/php-cgi;
        cgi_pass .py /usr/bin/python3;
        allow_methods GET POST;
    }
    
    # LOCATION 4: Redirection
    location /old-page {
        return 301 /new-page;
    }
    
    # LOCATION 5: Static files
    location /images {
        root /var/www/static;
        allow_methods GET;
        autoindex on;
    }
}

server {
    listen 8081;
    server_name admin.local;
    
    client_max_body_size 5M;
    error_page 403 /forbidden.html;
    
    location / {
        root /var/www/admin;
        index admin.html;
        allow_methods GET POST DELETE;
    }
}
```

---

## DIRECTIVE VALIDATION RULES

### What your parser should check:

**listen:**
- ✓ At least one listen directive exists in server
- ✓ Port is 0-65535
- ✓ Host is valid IP or hostname

**server_name:**
- ✓ Can be empty (optional)
- ✓ Any string is valid

**client_max_body_size:**
- ✓ Number is positive
- ✓ Unit is K, M, G, or none

**error_page:**
- ✓ Error code is 100-599
- ✓ Path starts with /

**root:**
- ✓ Path is absolute (starts with /)

**index:**
- ✓ At least one filename

**allow_methods:**
- ✓ At least one method
- ✓ Methods are valid (GET, POST, DELETE, etc.)

**autoindex:**
- ✓ Value is "on" or "off"

**return:**
- ✓ Code is valid redirect code (301, 302, etc.)
- ✓ URL exists

**cgi_pass:**
- ✓ Extension starts with .
- ✓ Handler path exists (runtime check)

**upload_enable:**
- ✓ Value is "on" or "off"

**upload_path:**
- ✓ Path is absolute
- ✓ Directory exists and writable (runtime check)

---

## QUICK REFERENCE CHECKLIST

**SERVER BLOCK:**
- [x] listen (REQUIRED)
- [x] server_name
- [x] client_max_body_size
- [x] error_page

**LOCATION BLOCK:**
- [x] root
- [x] index
- [x] allow_methods
- [x] autoindex
- [x] return
- [x] cgi_pass
- [x] upload_enable
- [x] upload_path
- [x] client_max_body_size (override)
- [x] error_page (override)