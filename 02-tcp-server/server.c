#include <arpa/inet.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
/* Step: This first server is standalone and deliberately serves one connection. */
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
    char *end;
    errno = 0;
    long port = strtol(argv[1], &end, 10);
    if (errno || end == argv[1] || *end || port < 1 || port > 65535) {
        fprintf(stderr, "PORT must be 1..65535\n"); return 1;
    }
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) { perror("signal"); return 1; }
    int listener = -1, peer = -1, status = 1;
    listener = socket(AF_INET, SOCK_STREAM, 0);
    if (listener < 0) { perror("socket"); goto done; }
    /* Step: Bind only loopback. htons converts the integer port to network order. */
    int one = 1;
    if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one) < 0) {
        perror("setsockopt"); goto done;
    }
    struct sockaddr_in address = {0};
    address.sin_family = AF_INET;
    address.sin_port = htons((uint16_t)port);
    address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if (bind(listener, (struct sockaddr *)&address, sizeof address) < 0) {
        perror("bind"); goto done;
    }
    if (listen(listener, 8) < 0) { perror("listen"); goto done; }
    printf("Listening on 127.0.0.1:%ld; one connection\n", port); fflush(stdout);
    /* Step: accept returns a DIFFERENT descriptor. The listening socket stays open. */
    do { peer = accept(listener, NULL, NULL); } while (peer < 0 && errno == EINTR);
    if (peer < 0) { perror("accept"); goto done; }
    unsigned char buffer[4096];
    for (;;) {
        ssize_t received = recv(peer, buffer, sizeof buffer, 0);
        if (received < 0 && errno == EINTR) continue;
        if (received < 0) { perror("recv"); goto done; }
        if (received == 0) break;
        /* Step: send may be short, so retain an offset until all bytes are echoed. */
        size_t sent = 0;
        while (sent < (size_t)received) {
            ssize_t n = send(peer, buffer + sent, (size_t)received - sent, 0);
            if (n < 0 && errno == EINTR) continue;
            if (n <= 0) { perror("send"); goto done; }
            sent += (size_t)n;
        }
    }
    status = 0;
done:
    /* Step: One cleanup path releases descriptors on success and on every failure. */
    if (peer >= 0 && close(peer) < 0) { perror("close peer"); status = 1; }
    if (listener >= 0 && close(listener) < 0) { perror("close listener"); status = 1; }
    return status;
}
