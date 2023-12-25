/* libserver.c
 * Craig Cochrane, 2023
 *
 * Definitions of functions used in http_server
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>

#include "libserver.h"

char* http_response_content_type(int status_code, char* content, const char* content_type)
{
    /* Build a string containing the HTTP response.
       Includes headers with status code and content length and the response content

       Returns a string containing the full HTTP response. String must be freed after use!
    */

    // template string for an HTTP response
    const char template[] = 
    "HTTP/1.1 %d \r\n"
    "Content-Type: %s\r\n"
    "Content-Length: %d\r\n"
    "Connection: close\r\n"
    "\n"
    "%s\n";

    // build the string from the response template
    char* response;

    int length = snprintf(response, 0, template, status_code, content_type, (int)strlen(content), content);  // return value of snprintf is the number of chars that would be written, gives the length of the formatted string
    if (length < 0)
    {
        syslog(LOG_ERR, "Can't form request string");
        exit(-1);
    }

    response = malloc(length+1);  // allocate enough space for the response + null terminator
    if (response == NULL) // check return value from malloc
    {
        syslog(LOG_ERR, "Memory allocation for request string failed");
        exit(-1);
    }
    
    snprintf(response, length+1, template, status_code, content_type, (int)strlen(content), content); // save fotmatted response + null terminator to buffer

   return response;
}

char* http_response(int status_code, char* content)
{
    http_response_content_type(status_code, content, "text/html");
}

http_reply_t parse_http_request(char* request_str, const char* file_path)
{
    /* Parse an HTTP request
     * 
     * The request method is checked, any requests that are not GET requests will be replied to with 405, method not allowed
     *
     * Return a http_reply_t struct containing the requested file if applicable and the status code of the reply
     * file_path is the path that should be prepended to the requested path from the GET request
     */

    http_reply_t request;
    memset(&request, 0, sizeof(struct http_reply_t));

    // check that the request is a GET request, if not return a 405 status code
    {
        char* method = request_str;

        char* space = strchr(method, ' ');
        if (space == NULL)
        {
            syslog(LOG_INFO, "Invalid HTTP request");
            request.status_code = 400;
            return request;
        }
        *space = 0;

        if (strcmp(method, "GET") != 0)
        {
            syslog(LOG_INFO, "HTTP request method not allowed: %s", method);
            request.status_code = 405;
            return request;
        }
    }

    // get file name from the request, filename is given from the 5th character of the GET request to the first space
    // GET /file.html .....
    {
        char* f = request_str + 5;
        // check that a space exits in the request, if not then request is malformed and 400 status code should be returned
        char* space = strchr(f, ' ');
        if (space == NULL)
        {
            syslog(LOG_INFO, "Invalid HTTP request");
            request.status_code = 400;
            return request;
        }
        *space = 0;

        int total_file_path_len = strlen(file_path) + strlen(f);
        char* full_path = malloc(total_file_path_len + 1);
        
        strcpy(full_path, file_path);
        strcat(full_path, f); // concatenate the requested file to the path
        
        strcpy(request.requested_file, full_path);
        free(full_path);

        // check that the requested file exists and can be accessed, if not a 404 status code should be returned
        if ((access(request.requested_file, R_OK)) != 0)
        {
            request.status_code = 404;
            syslog(LOG_INFO, "Requested file not found: %s", request.requested_file);
            return request;
        }

        // parse the requested filename for file extension, use this to set the content-type reply header
        // default to application/octet-stream if a file extension not given
        char* ext = strchr(f, '.'); 
        strcpy(request.file_type, "application/octet-stream");

        if (ext != NULL) // file has an extension which isn't .txt
        {
            memset(request.file_type, 0, strlen(request.file_type)); // clear current file type

            if ((strcmp(ext, ".html") == 0) || (strcmp(ext, ".htm") == 0))
            {
                strcpy(request.file_type, "text/html");
            }
            else if (strcmp(ext, ".css") == 0)
            {
                strcpy(request.file_type, "text/css");
            }
            else if (strcmp(ext, ".js") == 0)
            {
                strcpy(request.file_type, "text/javascript");
            }
            else if (strcmp(ext, ".csv") == 0)
            {
                strcpy(request.file_type, "text/csv");
            }
            else if (strcmp(ext, ".json") == 0)
            {
                strcpy(request.file_type, "application/json");
            }
            else if (strcmp(ext, ".png") == 0)
            {
                strcpy(request.file_type, "image/png");
            }
            else if (strcmp(ext, ".gif") == 0)
            {
                strcpy(request.file_type, "image/gif");
            }
            else if ((strcmp(ext, ".jpg") == 0) || (strcmp(ext, ".jpeg") == 0))
            {
                strcpy(request.file_type, "image/jpeg");
            }
            else if (strcmp(ext, ".ico") == 0)
            {
                strcpy(request.file_type, "image/vnd.microsoft.icon");
            }
            else if (strcmp(ext, ".bin") == 0)
            {
                strcpy(request.file_type, "application/octet-stream");
            }
            else
            {
                // default to text/plain for other file extensions
                strcpy(request.file_type, "text/plain");
            }
        }
    }

    request.status_code = 200;
    syslog(LOG_INFO, "Requested file: %s\n", request.requested_file);

    return request;
}
