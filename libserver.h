/* libserver.h
 *
 * Declarations of functions used in http_server
 */

// create an HTTP response string with the given status code and content
char* http_response(int status_code, char* content);