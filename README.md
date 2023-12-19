# http_server
## Basic HTTP Server
Very basic minimalist HTTP server. Based on [this video](https://www.youtube.com/watch?v=2HrYIl6GpYg) by Nir Lichtman, and extended by Craig Cochrane.

It has no security or error checking, so don't use it.

Files are served from a specified directory, which defaults to the directory the server is run from, but can be specified with command line options. The given path is prepended to the file requested in the GET request, so a / must be added at the end of a directory name.

## Usage
```./server [options]```

### Options
- -c : Run server continuously. If not given, the server only replies to a single request and then exits
- -f : Path to search for requested files.  Can be absolute or relative to loaction of binary
- -p : Port the server will run on. Defualts to 8080

## To Do
- remove debugging output and add proper logging - use syslog library
- build directory for build artifacts
- update readme
- implement support for other http requests
