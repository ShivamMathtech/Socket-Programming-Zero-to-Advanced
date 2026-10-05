#include "reactor.h"
#include <poll.h>
#include <stdio.h>
/* Step: poll uses an array of descriptor/event pairs; negative descriptors are ignored. */
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
    net_init(); int listener = net_listen("127.0.0.1", argv[1]);
    if (listener < 0) die("listen");
    if (nonblocking(listener) < 0) { close_fd(listener); die("nonblocking"); }
    static struct connection clients[REACTOR_CLIENTS];
    for (int i = 0; i < REACTOR_CLIENTS; ++i) clients[i].fd = -1;
    printf("poll echo on %s\n", argv[1]); fflush(stdout);
    for (;;) {
        struct pollfd events[REACTOR_CLIENTS + 1] = {0};
        events[0].fd = listener; events[0].events = POLLIN;
        for (int i = 0; i < REACTOR_CLIENTS; ++i) {
            events[i + 1].fd = clients[i].fd;
            if (wants_read(&clients[i])) events[i + 1].events |= POLLIN;
            if (wants_write(&clients[i])) events[i + 1].events |= POLLOUT;
        }
        int ready = poll(events, REACTOR_CLIENTS + 1, -1);
        if (ready < 0 && errno == EINTR) continue;
        if (ready < 0) { perror("poll"); break; }
        for (int i = 0; i < REACTOR_CLIENTS; ++i) if (clients[i].fd >= 0) {
            short flags = events[i + 1].revents;
            /* Step: HUP can coexist with buffered bytes; read them instead of dropping the connection immediately. */
            if ((flags & (POLLERR | POLLNVAL)) ||
                connection_step(&clients[i], flags & (POLLIN | POLLHUP), flags & POLLOUT) < 0)
                connection_close(&clients[i]);
        }
        if (events[0].revents & POLLIN) {
            int fd = accept(listener, NULL, NULL);
            if (fd >= 0) (void)connection_add(clients, fd);
            else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) perror("accept");
        }
    }
    for (int i = 0; i < REACTOR_CLIENTS; ++i) connection_close(&clients[i]);
    close_fd(listener); return 1;
}
