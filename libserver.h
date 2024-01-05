/* libserver.h
 *
 * Declarations of functions used in http_server
 */

// struct to contain the file requested in a HTTP get request and the status code for the reply
typedef struct http_reply_t{
    int status_code;
    char requested_file[1024];
    char file_type[128];
} http_reply_t;

// create an HTTP response string with the given status code and content
char* http_response(int status_code, char* content); // http response with content-type of text/html
char* http_response_content_type(int status_code, char* content, const char* content_type);

// create an HTTP response for a redirect
char* http_redirect(const char* dest);

// parse an HTTP request
http_reply_t parse_http_request(char* request, const char* file_path);
