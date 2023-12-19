/* libserver.h
 *
 * Declarations of functions used in http_server
 */

// struct to contain the file requested in a HTTP get request and the status code for the reply
typedef struct http_reply_t{
    int status_code;
    char requested_file[1024];
} http_reply_t;

// create an HTTP response string with the given status code and content
char* http_response(int status_code, char* content);

// parse an HTTP request
http_reply_t parse_http_request(char* request, const char* file_path);
