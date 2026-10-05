#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
/* Step: socket allocates a descriptor; it does not create a TCP connection. */
int main(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return EXIT_FAILURE; }
    /* Step: getsockopt writes both the option value and its actual byte length. */
    int type = 0;
    socklen_t length = sizeof type;
    if (getsockopt(fd, SOL_SOCKET, SO_TYPE, &type, &length) < 0) {
        perror("getsockopt"); if (close(fd) < 0) perror("close"); return EXIT_FAILURE;
    }
    printf("descriptor=%d type=%d (SOCK_STREAM=%d)\n", fd, type, SOCK_STREAM);
    /* Step: Each successful socket needs an owner responsible for closing it. */
    if (close(fd) < 0) { perror("close"); return EXIT_FAILURE; }
    return 0;
}
