#include "reactor.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
/* Step: select takes disposable bit sets; rebuild them before every wait. */
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
    net_init();
    int listener = net_listen("127.0.0.1", argv[1]);
    if (listener < 0) die("listen");
    if (listener >= FD_SETSIZE || nonblocking(listener) < 0) { close_fd(listener); return 1; }
    static struct connection clients[REACTOR_CLIENTS];
    for (int i = 0; i < REACTOR_CLIENTS; ++i) clients[i].fd = -1;
    printf("select echo on %s\n", argv[1]); fflush(stdout);
    for (;;) {
        fd_set reads, writes; FD_ZERO(&reads); FD_ZERO(&writes);
        FD_SET(listener, &reads); int largest = listener;
        for (int i = 0; i < REACTOR_CLIENTS; ++i) if (clients[i].fd >= 0) {
            int fd = clients[i].fd;
            if (wants_read(&clients[i])) FD_SET(fd, &reads);
            if (wants_write(&clients[i])) FD_SET(fd, &writes);
            if (fd > largest) largest = fd;
        }
        int ready = select(largest + 1, &reads, &writes, NULL, NULL);
        if (ready < 0 && errno == EINTR) continue;
        if (ready < 0) { perror("select"); break; }
        /* Step: Process existing peers before accepting, so descriptor reuse cannot consume stale readiness. */
        for (int i = 0; i < REACTOR_CLIENTS; ++i) if (clients[i].fd >= 0) {
            int fd = clients[i].fd;
            if (connection_step(&clients[i], FD_ISSET(fd, &reads), FD_ISSET(fd, &writes)) < 0)
                connection_close(&clients[i]);
        }
        if (FD_ISSET(listener, &reads)) {
            int fd = accept(listener, NULL, NULL);
            if (fd >= FD_SETSIZE) close_fd(fd);
            else if (fd >= 0) (void)connection_add(clients, fd);
            else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) perror("accept");
        }
    }
    for (int i = 0; i < REACTOR_CLIENTS; ++i) connection_close(&clients[i]);
    close_fd(listener); return 1;
}
