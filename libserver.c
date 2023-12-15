/* libserver.c
 * Craig Cochrane, 2023
 *
 * Definitions of functions used in http_server
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "libserver.h"

char* http_response(int status_code, char* content)
{
    /* Build a string containing the HTTP response.
       Includes headers with status code and content length and the response content

       Returns a string containing the full HTTP response. String must be freed after use!
    */

    // template string for an HTTP response
    const char template[] = 
    "HTTP/1.1 %d OK\
    Content-Type: text/html\
    Content-Length: %d\
    \n\n\
    %s";

    // build the string from the response template
    char* response;

    int length = snprintf(response, 0, template, status_code, (int)strlen(content), content);  // return value of snprintf is the number of chars that would be written, gives the length of the formatted string
    if (length < 0)
    {
        exit(-1);
    }

    response = malloc(length+1);  // allocate enough space for the response + null terminator
    if (response == NULL) // check return value from malloc
    {
        exit(-1);
    }
    
    snprintf(response, length+1, template, status_code, (int)strlen(content), content); // save fotmatted response + null terminator to buffer

   return response;
}
