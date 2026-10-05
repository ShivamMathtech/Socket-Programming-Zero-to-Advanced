#include "chat.h"
#include "net.h"
#include <errno.h>
#include <poll.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
/* Step: Watch keyboard and socket together so either participant can speak first. */
int chat_session(int fd) {
    int input_open = 1;
    if (set_timeout(fd, 15) < 0) return -1;
    for (;;) {
        struct pollfd items[2] = {{.fd = input_open ? STDIN_FILENO : -1, .events = POLLIN},
                                 {.fd = fd, .events = POLLIN}};
        int ready = poll(items, 2, -1);
        if (ready < 0 && errno == EINTR) continue;
        if (ready < 0) return -1;
        if ((items[0].revents | items[1].revents) & (POLLERR | POLLNVAL)) { errno = EIO; return -1; }
        unsigned char buffer[4096];
        if (items[0].revents & (POLLIN | POLLHUP)) {
            ssize_t n = read(STDIN_FILENO, buffer, sizeof buffer);
            if (n < 0 && errno != EINTR) return -1;
            if (n == 0) {
                /* Step: Ctrl-D stops sending but leaves the receive path alive. */
                input_open = 0;
                if (shutdown(fd, SHUT_WR) < 0) return -1;
            } else if (n > 0 && send_all(fd, buffer, (size_t)n) < 0) return -1;
        }
        if (items[1].revents & (POLLIN | POLLHUP)) {
            ssize_t n = recv(fd, buffer, sizeof buffer, 0);
            if (n < 0 && errno == EINTR) continue;
            if (n < 0) return -1;
            if (n == 0) return 0;
            if (fwrite(buffer, 1, (size_t)n, stdout) != (size_t)n || fflush(stdout) == EOF) return -1;
        }
    }
}
