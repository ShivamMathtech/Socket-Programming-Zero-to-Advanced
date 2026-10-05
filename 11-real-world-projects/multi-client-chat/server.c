#include "net.h"
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
/* Step: The wire protocol is newline-delimited, with at most 512 bytes before each newline. */
#define PEERS 32
#define LINE 512
#define QUEUE 16384
struct peer { int fd, eof; unsigned id; size_t input_size, output_size; char input[LINE], output[QUEUE]; };
static struct peer peers[PEERS];
static void drop(struct peer *p) { close_fd(p->fd); p->fd = -1; p->input_size = p->output_size = 0; p->eof = 0; }
/* Step: Queue broadcasts separately for each receiver; disconnect a receiver whose queue overflows. */
static void broadcast(struct peer *sender) {
    char line[LINE + 64];
    int prefix = snprintf(line, sizeof line, "peer#%u: ", sender->id);
    if (prefix < 0 || (size_t)prefix >= sizeof line) return;
    size_t size = (size_t)prefix;
    memcpy(line + size, sender->input, sender->input_size); size += sender->input_size;
    line[size++] = '\n';
    for (int i = 0; i < PEERS; ++i) if (peers[i].fd >= 0) {
        struct peer *target = &peers[i];
        if (size > QUEUE - target->output_size) { drop(target); continue; }
        memcpy(target->output + target->output_size, line, size); target->output_size += size;
    }
}
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
    net_init(); int listener = net_listen("127.0.0.1", argv[1]);
    if (listener < 0) die("listen");
    if (nonblocking(listener) < 0) { close_fd(listener); die("nonblocking"); }
    for (int i = 0; i < PEERS; ++i) peers[i].fd = -1;
    unsigned next_id = 1;
    printf("Multi-chat on %s (32 peers, 512-byte lines)\n", argv[1]); fflush(stdout);
    for (;;) {
        struct pollfd events[PEERS + 1] = {0}; events[0].fd = listener; events[0].events = POLLIN;
        for (int i = 0; i < PEERS; ++i) {
            events[i + 1].fd = peers[i].fd;
            events[i + 1].events = (peers[i].eof ? 0 : POLLIN) | (peers[i].output_size ? POLLOUT : 0);
        }
        int ready = poll(events, PEERS + 1, -1);
        if (ready < 0 && errno == EINTR) continue;
        if (ready < 0) { perror("poll"); break; }
        for (int i = 0; i < PEERS; ++i) {
            struct peer *p = &peers[i]; short flags = events[i + 1].revents;
            if (p->fd < 0) continue;
            if (flags & (POLLERR | POLLNVAL)) { drop(p); continue; }
            if ((flags & POLLOUT) && p->output_size) {
                ssize_t n = send(p->fd, p->output, p->output_size, 0);
                if (n > 0) { p->output_size -= (size_t)n; memmove(p->output, p->output + n, p->output_size); }
                else if (n == 0 || (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)) { drop(p); continue; }
            }
            if (!p->eof && (flags & (POLLIN | POLLHUP))) {
                char bytes[1024]; ssize_t n = recv(p->fd, bytes, sizeof bytes, 0);
                if (n == 0) p->eof = 1;
                else if (n < 0 && errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) { drop(p); continue; }
                /* Step: Parse all complete lines, retaining a split line across recv calls. */
                for (ssize_t j = 0; j < n && p->fd >= 0; ++j) {
                    if (bytes[j] == '\n') { broadcast(p); p->input_size = 0; }
                    else if (p->input_size == LINE) { drop(p); break; }
                    else p->input[p->input_size++] = bytes[j];
                }
            }
            if (p->fd >= 0 && p->eof && !p->output_size) drop(p);
        }
        if (events[0].revents & POLLIN) {
            int fd = accept(listener, NULL, NULL);
            if (fd < 0) { if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) perror("accept"); continue; }
            if (nonblocking(fd) < 0) { perror("nonblocking"); close_fd(fd); continue; }
            int slot;
            for (slot = 0; slot < PEERS; ++slot) if (peers[slot].fd < 0) break;
            if (slot == PEERS) close_fd(fd);
            else { peers[slot].fd = fd; peers[slot].id = next_id++; }
        }
    }
    for (int i = 0; i < PEERS; ++i) drop(&peers[i]);
    close_fd(listener); return 1;
}
