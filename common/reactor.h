#ifndef COURSE_REACTOR_H
#define COURSE_REACTOR_H
#include "net.h"
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
/* Step: One bounded output queue per connection prevents a slow reader blocking other peers. */
#define REACTOR_CLIENTS 64
#define REACTOR_BUFFER 16384
struct connection { int fd, eof; size_t used; unsigned char output[REACTOR_BUFFER]; };
static inline void connection_close(struct connection *c) {
    close_fd(c->fd); c->fd = -1; c->used = 0; c->eof = 0;
}
static inline int wants_read(const struct connection *c) { return !c->eof && c->used < REACTOR_BUFFER; }
static inline int wants_write(const struct connection *c) { return c->used > 0; }
/* Step: The loop performs bounded work per ready descriptor; it never calls blocking send_all. */
static inline int connection_step(struct connection *c, int readable, int writable) {
    if (writable && c->used) {
        ssize_t n = send(c->fd, c->output, c->used, 0);
        if (n > 0) {
            c->used -= (size_t)n;
            memmove(c->output, c->output + n, c->used);
        } else if (n == 0 || (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)) return -1;
    }
    if (readable && wants_read(c)) {
        ssize_t n = recv(c->fd, c->output + c->used, REACTOR_BUFFER - c->used, 0);
        if (n > 0) c->used += (size_t)n;
        else if (n == 0) c->eof = 1;
        else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) return -1;
    }
    /* Step: An orderly half-close still requires draining queued replies before close. */
    return c->eof && c->used == 0 ? -1 : 0;
}
static inline int connection_add(struct connection clients[], int fd) {
    if (nonblocking(fd) < 0) { close_fd(fd); return -1; }
    for (int i = 0; i < REACTOR_CLIENTS; ++i) if (clients[i].fd < 0) {
        clients[i].fd = fd; clients[i].eof = 0; clients[i].used = 0; return i;
    }
    close_fd(fd); return -1;
}
#endif
