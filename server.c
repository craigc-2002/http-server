#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "libserver.h"

int main()
{
    printf("Server Started\n\n");

    // open an IPv4 TCP socket
    int s = socket(AF_INET, SOCK_STREAM, 0);

    // set socket to have no timeout on the address and port when it closes
    const int enable = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int));
    #ifdef SO_REUSEPORT
    setsockopt(s, SOL_SOCKET, SO_REUSEPORT, &enable, sizeof(int));
    #endif

    // bind socket to port 8080
    struct sockaddr_in addr = {
        AF_INET, // IPv4
        0x901f, // port 8080
        0};

    bind(s, (const struct sockaddr*) &addr, sizeof(addr));
    listen(s, 10); // set socket to listen for incoming connections

    // accept the first connection
    int client_fd = accept(s, 0, 0);
    if (client_fd < 0)
    {
        exit(-1);
    }

    // receive HTTP request from client into a buffer (if request is more than 255 bytes it will be truncated, only the requested file is actually read)
    char buf[256] = {0};
    recv(client_fd, buf, 255, 0);
    printf("%s\n", buf);

    // parse HTTP request
    // GET /file.html .....
    
    // get file name from the request
    char* f = buf + 5;
    *strchr(f, ' ') = 0;
    printf("%s\n", f);

    // save contents of requested file into buffer
    FILE* requested_file = fopen(f, "r");
    fseek(requested_file, 0, SEEK_END); // go to end of file to get the size
    int file_length = ftell(requested_file); // position of stream at end of file gives the length
    fseek(requested_file, 0, SEEK_SET);

    char* req_file_buf = malloc(file_length+1); // buffer to hold file contents + null terminator
    fgets(req_file_buf, file_length+1, requested_file);
    req_file_buf[file_length] = 0;
    fclose(requested_file);
    printf("%s\n", req_file_buf);

    // form HTTP response with file contents
    char* response = http_response(200, req_file_buf);
    printf("\nHTTP Response:\n%s\n", response);

    // send response to client
    write(client_fd, response, strlen(response));

    free(response);
    close(client_fd);
    close(s);
}
