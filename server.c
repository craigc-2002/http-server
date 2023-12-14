#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    printf("Server Started\n");
    int s = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in addr = {
        AF_INET,
        0x901f,
        0};

    bind(s, &addr, sizeof(addr));
    listen(s, 10);
    int client_fd = accept(s, 0, 0);

    char buf[256] = {0};
    recv(client_fd, buf, 256, 0);
    printf("%s\n", buf);

    // GET /file.html .....
    char* f = buf + 5;
    *strchr(f, ' ') = 0;
    printf("%s\n", f);

    int open_fd = open(f, O_RDONLY);
    sendfile(client_fd, open_fd,  0, 256);

    close(open_fd);
    close(client_fd);
    close(s);
}
