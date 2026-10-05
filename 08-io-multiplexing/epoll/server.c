#include "reactor.h"
#include <stdio.h>
#include <sys/epoll.h>
/* Step: epoll stores interest in the kernel; this lesson uses level triggering. */
static uint32_t interests(struct connection *c) {
    return (wants_read(c) ? EPOLLIN : 0U) | (wants_write(c) ? EPOLLOUT : 0U);
}
static void remove_client(int queue, struct connection *c) {
    if (epoll_ctl(queue, EPOLL_CTL_DEL, c->fd, NULL) < 0) perror("epoll del");
    connection_close(c);
}
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
    net_init(); int listener = net_listen("127.0.0.1", argv[1]);
    if (listener < 0) die("listen");
    if (nonblocking(listener) < 0) { close_fd(listener); die("nonblocking"); }
    int queue = epoll_create1(EPOLL_CLOEXEC);
    if (queue < 0) { close_fd(listener); die("epoll_create1"); }
    struct epoll_event event = {.events = EPOLLIN, .data.u32 = REACTOR_CLIENTS};
    if (epoll_ctl(queue, EPOLL_CTL_ADD, listener, &event) < 0) {
        close_fd(queue); close_fd(listener); die("epoll add listener");
    }
    static struct connection clients[REACTOR_CLIENTS];
    for (int i = 0; i < REACTOR_CLIENTS; ++i) clients[i].fd = -1;
    printf("epoll echo on %s\n", argv[1]); fflush(stdout);
    for (;;) {
        struct epoll_event ready[REACTOR_CLIENTS + 1];
        int count = epoll_wait(queue, ready, REACTOR_CLIENTS + 1, -1);
        if (count < 0 && errno == EINTR) continue;
        if (count < 0) { perror("epoll_wait"); break; }
        int accepting = 0;
        /* Step: Finish this batch of old client events before assigning a free slot to a new peer. */
        for (int j = 0; j < count; ++j) {
            unsigned index = ready[j].data.u32;
            if (index == REACTOR_CLIENTS) { accepting = 1; continue; }
            struct connection *c = &clients[index];
            uint32_t flags = ready[j].events;
            if ((flags & EPOLLERR) || connection_step(c, flags & (EPOLLIN | EPOLLHUP), flags & EPOLLOUT) < 0) {
                remove_client(queue, c); continue;
            }
            event.events = interests(c); event.data.u32 = index;
            if (epoll_ctl(queue, EPOLL_CTL_MOD, c->fd, &event) < 0) {
                perror("epoll mod"); remove_client(queue, c);
            }
        }
        if (accepting) {
            int fd = accept(listener, NULL, NULL);
            if (fd < 0) {
                if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) perror("accept");
                continue;
            }
            int index = connection_add(clients, fd);
            if (index < 0) continue;
            event.events = EPOLLIN; event.data.u32 = (unsigned)index;
            if (epoll_ctl(queue, EPOLL_CTL_ADD, fd, &event) < 0) {
                perror("epoll add"); connection_close(&clients[index]);
            }
        }
    }
    for (int i = 0; i < REACTOR_CLIENTS; ++i) connection_close(&clients[i]);
    close_fd(queue); close_fd(listener); return 1;
}
