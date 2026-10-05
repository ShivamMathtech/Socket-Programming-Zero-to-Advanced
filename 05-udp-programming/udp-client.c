#include "net.h"
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
/* Step: Small UDP requests avoid fragmentation in this introductory exercise. */
int main(int argc, char **argv) {
    if (argc != 4 || !valid_port(argv[2]) || strlen(argv[3]) > 1200) {
        fprintf(stderr, "usage: %s IPv4 PORT MESSAGE (<=1200 bytes)\n", argv[0]); return 1;
    }
    struct sockaddr_in peer = {0};
    peer.sin_family = AF_INET; peer.sin_port = htons((uint16_t)strtoul(argv[2], NULL, 10));
    if (inet_pton(AF_INET, argv[1], &peer.sin_addr) != 1) return 1;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) die("socket");
    if (set_timeout(fd, 3) < 0) { close_fd(fd); die("timeout"); }
    size_t count = strlen(argv[3]);
    ssize_t n;
    do { n = sendto(fd, argv[3], count, 0, (struct sockaddr *)&peer, sizeof peer); }
    while (n < 0 && errno == EINTR);
    int status = 1;
    if (n < 0 || (size_t)n != count) { perror("sendto"); goto done; }
    /* Step: Verify the reply endpoint; a source IP/port check is not authentication. */
    struct sockaddr_in sender; socklen_t length = sizeof sender;
    char data[65536];
    do { n = recvfrom(fd, data, sizeof data, 0, (struct sockaddr *)&sender, &length); }
    while (n < 0 && errno == EINTR);
    if (n < 0) { perror("recvfrom (3 second timeout)"); goto done; }
    if (sender.sin_addr.s_addr != peer.sin_addr.s_addr || sender.sin_port != peer.sin_port) {
        fprintf(stderr, "unexpected source\n"); goto done;
    }
    if (fwrite(data, 1, (size_t)n, stdout) != (size_t)n || putchar('\n') == EOF) goto done;
    status = 0;
done:
    close_fd(fd); return status;
}
