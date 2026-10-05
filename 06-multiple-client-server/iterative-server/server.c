#include "net.h"
#include <stdio.h>
/* Step: A loop accepts many clients over time, but serves only one at a time. */
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
    net_init();
    int listener = net_listen("127.0.0.1", argv[1]);
    if (listener < 0) die("listen");
    printf("Iterative echo on %s\n", argv[1]); fflush(stdout);
    for (;;) {
        int peer = accept_retry(listener);
        if (peer < 0) { perror("accept"); break; }
        if (set_timeout(peer, 15) < 0 || echo_connection(peer) < 0) perror("client");
        close_fd(peer);
    }
    close_fd(listener); return 1;
}
