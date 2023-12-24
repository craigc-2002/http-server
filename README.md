# http_server
## Basic HTTP Server
Very basic HTTP server for serving static files. Based on [this video](https://www.youtube.com/watch?v=2HrYIl6GpYg) by Nir Lichtman, and extended by Craig Cochrane.

Files are served from a specified directory, which defaults to the directory the server is run from, but can be specified with command line options. The given path is prepended to the file requested in0 the GET request, so a / must be added at the end of a directory name.

Only GET requests are handled, any other request type will be replied to with 405, method not allowed.

It has no security or error checking, so don't use it on the internet.

## Usage
```./server [options]```

### Options
- -c : Run server continuously. If not given, the server only replies to a single request and then exits
- -f : Path to search for requested files.  Can be absolute or relative to loaction of binary
- -p : Port the server will run on. Defualts to 8080

## Logging
Logging is implemented using syslog. This works but the log entries just appear in /var/log/syslog along with the logs from other servises. To solve this, the following can be added to the /etc/rsyslog.conf file:
```
if $syslogtag contains "http_server" then /var/log/http_server_debug.log 
if $syslogtag contains "http_server" and $syslogseverity-text != "DEBUG" then /var/log/http_server.log
```

This makes all log entries appear in /var/log/http_server_debug.log and informational messages and above appear in /var/log/http_server_debug.log.

Note: This changes depending on syslog implementation used. A different logging method should be used for portable applications.

## To Do
- build directory for build artifacts
- implement support for different file types (change content-type header)
