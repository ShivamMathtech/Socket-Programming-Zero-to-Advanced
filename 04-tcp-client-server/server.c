#include "net.h"
#include <stdio.h>
#include <stdlib.h>
/* Step: The shared helpers below expose, rather than replace, the send/recv loops. */
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
    net_init();
    int listener = net_listen("127.0.0.1", argv[1]);
    if (listener < 0) die("listen");
    unsigned char *buffer = malloc(COURSE_FRAME_MAX);
    if (!buffer) { close_fd(listener); die("malloc"); }
    printf("Framed echo on 127.0.0.1:%s\n", argv[1]); fflush(stdout);
    for (;;) {
        int peer = accept_retry(listener);
        if (peer < 0) { perror("accept"); break; }
        /* Step: A per-operation timeout bounds idle blocking, not total request duration. */
        if (set_timeout(peer, 15) < 0) { perror("timeout"); close_fd(peer); continue; }
        uint32_t length;
        int result;
        while ((result = frame_recv(peer, buffer, COURSE_FRAME_MAX, &length)) == 1)
            if (frame_send(peer, buffer, length) < 0) { result = -1; break; }
        if (result < 0) perror("frame");
        close_fd(peer);
    }
    free(buffer); close_fd(listener); return 1;
}
