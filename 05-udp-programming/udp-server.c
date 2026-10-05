#include "net.h"
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
/* Step: UDP binds an endpoint but never calls listen or accept. */
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
    int fd = net_bound("127.0.0.1", argv[1], SOCK_DGRAM);
    if (fd < 0) die("bind UDP");
    printf("UDP echo on 127.0.0.1:%s\n", argv[1]); fflush(stdout);
    for (;;) {
        unsigned char data[65536];
        struct sockaddr_storage peer;
        socklen_t length = sizeof peer;
        /* Step: Reinitialize length on every receive; even zero bytes is a valid datagram. */
        ssize_t n = recvfrom(fd, data, sizeof data, 0, (struct sockaddr *)&peer, &length);
        if (n < 0 && errno == EINTR) continue;
        if (n < 0) { perror("recvfrom"); break; }
        ssize_t sent;
        do { sent = sendto(fd, data, (size_t)n, 0, (struct sockaddr *)&peer, length); }
        while (sent < 0 && errno == EINTR);
        if (sent != n) perror("sendto");
    }
    close_fd(fd); return 1;
}
