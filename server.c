#include <sys/socket.h>
#include <sys/sendfile.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <syslog.h>

#include "libserver.h"

int main(int argc, char* argv[])
{
    /* parse arguments using getopt to check if the user wants the server to run continuously 
     *
     * -c : Run server continously. If not given, the server only replies to a single request and then exits
     * -f : Path to search for requested files.  Can be absolute or relative to loaction of binary 
     * -p : Port the server will run on. Defualts to 8080
     */
    bool run_continuous = false;
    char* file_path;
    uint16_t port_num = 8080;
    
    char default_path[] = "./";
    file_path = default_path; // path that will be searched for requested files
    
    // parse arguments with getopt
    {
        int opt;
        extern int optind;
        extern char* optarg;

        while ((opt = getopt(argc, argv, "cd:p:")) != -1)
        {
            switch (opt)
            {
                case 'c':
                    run_continuous = true;
                    break;

                case 'd':
                    file_path = optarg;
                    break;

                case 'p':
                    port_num = atoi(optarg);
                    break;

                default:
                    fprintf(stderr, "Usage:\n./server [options]\n\n");
                    fprintf(stderr, "Options:\n");
                    fprintf(stderr, "-c run server continuously:\n");
                    fprintf(stderr, "-d : path to search for requested files\n");
                    fprintf(stderr, "-p : port the server will run on\n\n");
                    exit(-1);
            }
        }
    }

    // logging for new connections and errors
    openlog("http_server", LOG_CONS|LOG_PID, LOG_DAEMON);

    syslog(LOG_INFO, "Server Started on port %d", port_num);
    syslog(LOG_INFO, "Serving files from %s", file_path);

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
        struct sockaddr_in client_addr; // struct to contain the address of the client socket
        socklen_t addr_len = sizeof(struct sockaddr_in);  

        //  UPDATE THIS TO GET THE PROPER IP ADDRESS OF CLIENT
        int client_fd = accept(s, (struct sockaddr*)&client_addr, &addr_len);
        //int client_fd = accept(s, 0, 0);
        if (client_fd < 0)
        {
            syslog(LOG_ERR, "Failure connecting to client socket: %m");
            exit(-1);
        }
        syslog(LOG_INFO, "Connected to client at: %s\n", (inet_ntoa(client_addr.sin_addr)));

        // receive HTTP request from client into a buffer (if request is more than 255 bytes it will be truncated, only the requested file is actually read)
        char buf[256] = {0};
        recv(client_fd, buf, 255, 0);
        syslog(LOG_DEBUG, "Request: %s\n", buf);

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
                    syslog(LOG_ERR, "Requested file can't be opened");
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

                syslog(LOG_DEBUG, "%s\n", req_file_buf); // send the requested file buffer to debug log

                // form HTTP response with file contents
                response = http_response_content_type(200, req_file_buf, reply.file_type);
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

            case 405:
            {
                // request is for an invalid method
                response = http_response(405, "Method not allowed");
            }

            default:
            {
                // the reply doesn't have a status code handled above, return a server error
                response = http_response(500, "Server error");
            }
        }


        syslog(LOG_DEBUG, "\nHTTP Response: %s\n", response); // send the full HTTP response to debug log
        // send response to client
        write(client_fd, response, strlen(response));

        free(response);
        close(client_fd);

    }while (run_continuous);

    syslog(LOG_INFO, "Server stopped");
    close(s);
}
