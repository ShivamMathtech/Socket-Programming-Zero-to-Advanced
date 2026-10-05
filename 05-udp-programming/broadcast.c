#include "net.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
/* Step: A single explicit broadcast demonstrates SO_BROADCAST; no discovery loop is hidden. */
int main(int argc, char **argv) {
    if (argc != 4 || !valid_port(argv[2]) || strlen(argv[3]) > 1200) {
        fprintf(stderr, "usage: %s BROADCAST_IP PORT MESSAGE\n", argv[0]); return 1;
    }
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET; address.sin_port = htons((uint16_t)strtoul(argv[2], NULL, 10));
    if (inet_pton(AF_INET, argv[1], &address.sin_addr) != 1) return 1;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) die("socket");
    int one = 1, status = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &one, sizeof one) < 0) perror("SO_BROADCAST");
    else {
        ssize_t n;
        do { n = sendto(fd, argv[3], strlen(argv[3]), 0, (struct sockaddr *)&address, sizeof address); }
        while (n < 0 && errno == EINTR);
        if (n < 0) perror("sendto");
        else { printf("Sent %zd bytes once\n", n); status = 0; }
    }
    close_fd(fd); return status;
}
