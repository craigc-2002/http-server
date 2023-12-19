# http_server
## Basic HTTP Server
Very basic minimalist HTTP server. Based on [this video](https://www.youtube.com/watch?v=2HrYIl6GpYg) by Nir Lichtman, and extended by Craig Cochrane.

It has no security or error checking, so don't use it.

## Usage
```./server [options] [port number]```

### Options
- -c : Run server continously. If not given, the server only replies to a single request and then exits

### Port number
A port number can optionally be added after the command line options. If it is not given, the port number defaults to 8080.

## To Do
- remove debugging output and add proper logging - use syslog library
- Limit files that can be served (maybe add a set folder that is searched for files)
- build directory for build artifacts
- update readme
- implement support for other http requests
