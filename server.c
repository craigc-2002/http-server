#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "libserver.h"

int main(int argc, char* argv[])
{
    /* parse arguments using getopt to check if the user wants the server to run continuously 
     *
     * -c : Run server continously. If not given, the server only replies to a single request and then exits
     * -f : Path to search for requested files.  Can be absolute or relative to loaction of binary 
     */
    bool run_continuous = false;
    
    char default_path[] = "./";
    char* file_path = default_path; // path that will be searched for requested files
    
    int opt;
    extern int optind;
    extern char* optarg;

    while ((opt = getopt(argc, argv, "cf:")) != -1)
    {
        switch (opt)
        {
            case 'c':
                run_continuous = true;
                break;

            case 'f':
                file_path = optarg;
                break;

            default:
                fprintf(stderr, "Usage:\n./server [options] [port_num]\n\n");
                fprintf(stderr, "Options:\n");
                fprintf(stderr, "-c run server continuously:\n");
                fprintf(stderr, "-f : path to search for requested files\n\n");
                exit(-1);
        }
    }

    // check command line arguments to see if a port number has been specified, if not default to 8080
    uint16_t port_num = 8080;

    if (optind < argc)
    {
        port_num = atoi(argv[optind]);
    }

    printf("Server Started on port %d\n", port_num);
    printf("Serving files from %s\n\n", file_path);

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
        htons(port_num), //0x901f, // port 8080
        0};

    bind(s, (const struct sockaddr*) &addr, sizeof(addr));
    listen(s, 10); // set socket to listen for incoming connections

    do
    {
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
        http_reply_t reply = parse_http_request(buf, file_path);

        // construct correct HTTP response depending on the reply status code
        char* response;

        switch (reply.status_code)
        {
            case 200:
            {
                // request was successful, return the requested resource

                // open requested file
                FILE* requested_file = fopen(reply.requested_file, "r");

                // check that the requested file is opened successfully
                if (requested_file == NULL)
                {
                    fprintf(stderr, "Requested file can't be opened");
                    exit(-1);
                }

                // save contents of requested file into buffer
                fseek(requested_file, 0, SEEK_END); // go to end of file to get the size
                int file_length = ftell(requested_file); // position of stream at end of file gives the length
                fseek(requested_file, 0, SEEK_SET);

                char* req_file_buf = malloc(file_length+1); // buffer to hold file contents + null terminator
                fread(req_file_buf, sizeof(char), file_length+1, requested_file); // read file contents into buffer 1 byte at a time
                req_file_buf[file_length] = 0;
                fclose(requested_file);
                printf("%s\n", req_file_buf);

                // form HTTP response with file contents
                response = http_response(200, req_file_buf);
                break;
            }

            case 400:
            {
                // request was not correctly formed
                response = http_response(400, "Malformed request");
                break;
            }

            case 404:
            {
                // request did not ask for a valid file
                response = http_response(404, "404 Not Found");
                break;
            }

            default:
            {
                // the reply doesn't have a status code handled above, return a server error
                response = http_response(500, "Server error");
            }
        }


        printf("\nHTTP Response:\n%s\n", response);
        // send response to client
        write(client_fd, response, strlen(response));

        free(response);
        close(client_fd);

    }while (run_continuous);

    close(s);
}
