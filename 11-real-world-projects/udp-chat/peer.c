#include "net.h"
#include <arpa/inet.h>
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>
/* Step: Both peers bind a local port, then configure one default remote UDP endpoint. */
int main(int argc, char **argv) {
    if (argc != 4 || !valid_port(argv[3])) {
        fprintf(stderr, "usage: %s LOCAL_PORT PEER_IPv4 PEER_PORT\n", argv[0]); return 1;
    }
    int fd = net_bound("127.0.0.1", argv[1], SOCK_DGRAM);
    if (fd < 0) die("bind");
    struct sockaddr_in peer = {0}; peer.sin_family = AF_INET;
    peer.sin_port = htons((uint16_t)strtoul(argv[3], NULL, 10));
    if (inet_pton(AF_INET, argv[2], &peer.sin_addr) != 1) { close_fd(fd); return 1; }
    if (connect(fd, (struct sockaddr *)&peer, sizeof peer) < 0) { close_fd(fd); die("UDP connect"); }
    fprintf(stderr, "UDP peer ready on %s; Ctrl-D exits locally\n", argv[1]);
    int status = 0;
    for (;;) {
        struct pollfd items[2] = {{.fd=STDIN_FILENO,.events=POLLIN},{.fd=fd,.events=POLLIN}};
        int ready = poll(items, 2, -1);
        if (ready < 0 && errno == EINTR) continue;
        if (ready < 0) { perror("poll"); status = 1; break; }
        if (items[1].revents & (POLLERR | POLLNVAL)) { fprintf(stderr,"UDP endpoint error; start both peers\n"); status=1; break; }
        char data[65536];
        if (items[0].revents & (POLLIN | POLLHUP)) {
            ssize_t n = read(STDIN_FILENO, data, 1200);
            if (n < 0 && errno == EINTR) continue;
            if (n < 0) { perror("stdin"); status = 1; break; }
            if (n == 0) break;
            /* Step: A connected UDP socket still sends datagrams and does not become reliable. */
            ssize_t sent;
            do { sent = send(fd, data, (size_t)n, 0); } while (sent < 0 && errno == EINTR);
            if (sent != n) { perror("send datagram"); status = 1; break; }
        }
        if (items[1].revents & POLLIN) {
            ssize_t n = recv(fd, data, sizeof data, 0);
            if (n < 0 && errno == EINTR) continue;
            if (n < 0) { perror("recv"); status = 1; break; }
            if (fwrite(data, 1, (size_t)n, stdout) != (size_t)n || fflush(stdout) == EOF) { status = 1; break; }
        }
    }
    close_fd(fd); return status;
}
