#include "net.h"
#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <time.h>
/* Step: A monotonic deadline survives wall-clock changes and repeated signal interruptions. */
static long long milliseconds(void) {
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now) < 0) die("clock_gettime");
    return (long long)now.tv_sec * 1000 + now.tv_nsec / 1000000;
}
int main(int argc, char **argv) {
    if (argc != 3 || !valid_port(argv[2])) { fprintf(stderr, "usage: %s IPv4 PORT\n", argv[0]); return 1; }
    struct sockaddr_in address = {0}; address.sin_family = AF_INET;
    address.sin_port = htons((uint16_t)strtoul(argv[2], NULL, 10));
    if (inet_pton(AF_INET, argv[1], &address.sin_addr) != 1) return 1;
    int fd = socket(AF_INET, SOCK_STREAM, 0), status = 1;
    if (fd < 0) die("socket");
    if (nonblocking(fd) < 0) { perror("fcntl"); goto done; }
    /* Step: EINPROGRESS means establishment is pending, not failed and not yet successful. */
    if (connect(fd, (struct sockaddr *)&address, sizeof address) < 0) {
        if (errno != EINPROGRESS) { perror("connect"); goto done; }
        long long deadline = milliseconds() + 3000;
        struct pollfd item = {.fd = fd, .events = POLLOUT};
        for (;;) {
            long long remaining = deadline - milliseconds();
            if (remaining <= 0) { fprintf(stderr, "connect deadline exceeded\n"); goto done; }
            int ready = poll(&item, 1, (int)remaining);
            if (ready < 0 && errno == EINTR) continue;
            if (ready < 0) { perror("poll"); goto done; }
            if (ready == 0) continue;
            /* Step: Writable can mean failure. SO_ERROR is the authoritative completion result. */
            int error = 0; socklen_t size = sizeof error;
            if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &size) < 0) { perror("SO_ERROR"); goto done; }
            if (error) { errno = error; perror("connect completion"); goto done; }
            break;
        }
    }
    puts("Connected before the 3 second deadline"); status = 0;
done:
    close_fd(fd); return status;
}
