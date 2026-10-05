#include "net.h"
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>
/* Step: Ignoring SIGPIPE lets normal control flow inspect EPIPE instead of losing the process. */
int main(void) {
    net_init(); int pair[2];
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, pair) < 0) die("socketpair");
    close_fd(pair[1]);
    int result = send_all(pair[0], "x", 1);
    int saved = errno;
    close_fd(pair[0]);
    if (result < 0 && saved == EPIPE) { puts("EPIPE handled without termination"); return 0; }
    fprintf(stderr, "expected EPIPE in this Linux AF_UNIX demonstration\n"); return 1;
}
