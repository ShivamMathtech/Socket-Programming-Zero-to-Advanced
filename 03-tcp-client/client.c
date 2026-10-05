#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
/* Step: The first client accepts a numeric IPv4 address and one short text message. */
int main(int argc, char **argv) {
    if (argc != 4) { fprintf(stderr, "usage: %s IPv4 PORT MESSAGE\n", argv[0]); return 1; }
    char *end; errno = 0;
    long port = strtol(argv[2], &end, 10);
    if (errno || end == argv[2] || *end || port < 1 || port > 65535 || strlen(argv[3]) > 4096) {
        fprintf(stderr, "bad port or message longer than 4096 bytes\n"); return 1;
    }
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) { perror("signal"); return 1; }
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET; address.sin_port = htons((uint16_t)port);
    if (inet_pton(AF_INET, argv[1], &address.sin_addr) != 1) {
        fprintf(stderr, "numeric IPv4 address required\n"); return 1;
    }
    int fd = socket(AF_INET, SOCK_STREAM, 0), status = 1;
    if (fd < 0) { perror("socket"); return 1; }
    /* Step: connect triggers active TCP establishment; the OS normally selects a local port. */
    if (connect(fd, (struct sockaddr *)&address, sizeof address) < 0) { perror("connect"); goto done; }
    size_t length = strlen(argv[3]), sent = 0;
    while (sent < length) {
        ssize_t n = send(fd, argv[3] + sent, length - sent, 0);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) { perror("send"); goto done; }
        sent += (size_t)n;
    }
    /* Step: Half-close signals end of request while keeping the receive direction open. */
    if (shutdown(fd, SHUT_WR) < 0) { perror("shutdown"); goto done; }
    unsigned char buffer[4096];
    for (;;) {
        ssize_t n = recv(fd, buffer, sizeof buffer, 0);
        if (n < 0 && errno == EINTR) continue;
        if (n < 0) { perror("recv"); goto done; }
        if (n == 0) break;
        if (fwrite(buffer, 1, (size_t)n, stdout) != (size_t)n) { perror("stdout"); goto done; }
    }
    if (putchar('\n') == EOF) goto done;
    status = 0;
done:
    if (close(fd) < 0) { perror("close"); status = 1; }
    return status;
}
