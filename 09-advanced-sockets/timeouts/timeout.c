#include "net.h"
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
/* Step: A local connected pair isolates receive timeout behavior from routing and DNS. */
int main(void) {
    int pair[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, pair) < 0) die("socketpair");
    int status = 1;
    if (set_timeout(pair[0], 1) < 0) perror("timeout");
    else {
        char byte;
        ssize_t n = recv(pair[0], &byte, 1, 0);
        if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
            puts("Receive timed out; peer is still open"); status = 0;
        } else fprintf(stderr, "unexpected recv result\n");
    }
    close_fd(pair[0]); close_fd(pair[1]); return status;
}
