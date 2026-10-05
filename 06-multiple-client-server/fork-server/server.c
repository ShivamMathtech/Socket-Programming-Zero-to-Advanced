#include "net.h"
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <sys/wait.h>
#include <unistd.h>
/* Step: The parent owns the listener; each child owns one connected descriptor. */
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
    net_init();
    int listener = net_listen("127.0.0.1", argv[1]);
    if (listener < 0) die("listen");
    int active = 0;
    printf("Fork echo on %s; maximum 64 children\n", argv[1]); fflush(stdout);
    for (;;) {
        /* Step: Reap exited children regularly, including when no new clients arrive. */
        pid_t reaped;
        do {
            reaped = waitpid(-1, NULL, WNOHANG);
            if (reaped > 0) --active;
        } while (reaped > 0 || (reaped < 0 && errno == EINTR));
        if (reaped < 0 && errno != ECHILD) { perror("waitpid"); break; }
        struct pollfd item = {.fd = listener, .events = POLLIN};
        int ready = poll(&item, 1, 200);
        if (ready < 0 && errno == EINTR) continue;
        if (ready < 0) { perror("poll"); break; }
        if (!ready) continue;
        int peer = accept_retry(listener);
        if (peer < 0) { perror("accept"); break; }
        if (active >= 64) { close_fd(peer); continue; }
        pid_t child = fork();
        if (child < 0) { perror("fork"); close_fd(peer); continue; }
        /* Step: Close unused inherited descriptors in BOTH processes to avoid leaked references. */
        if (child == 0) {
            close_fd(listener);
            int result = set_timeout(peer, 15);
            if (result == 0) result = echo_connection(peer);
            if (result < 0) perror("child echo");
            close_fd(peer); _exit(result < 0 ? 1 : 0);
        }
        ++active; close_fd(peer);
    }
    close_fd(listener); return 1;
}
