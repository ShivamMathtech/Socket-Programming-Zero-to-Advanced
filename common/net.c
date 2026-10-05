#include "net.h"
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
/* Step: SIGPIPE would otherwise terminate a process before it handles EPIPE. */
void net_init(void) {
    struct sigaction action = {0};
    action.sa_handler = SIG_IGN;
    if (sigemptyset(&action.sa_mask) < 0 || sigaction(SIGPIPE, &action, NULL) < 0)
        die("sigaction");
}
void die(const char *what) { perror(what); exit(EXIT_FAILURE); }
void close_fd(int fd) {
    /* Linux: do not retry close after EINTR; the descriptor may be reused. */
    if (fd >= 0 && close(fd) < 0) perror("close");
}
int valid_port(const char *port) {
    if (!port || !*port) return 0;
    for (const char *p = port; *p; ++p) if (*p < '0' || *p > '9') return 0;
    errno = 0;
    char *end;
    unsigned long n = strtoul(port, &end, 10);
    return !errno && *end == '\0' && n > 0 && n <= 65535;
}
/* Step: Resolve both address families, try candidates, and free the whole list. */
static int endpoint(const char *host, const char *port, int type, int binding) {
    if (!valid_port(port)) { errno = EINVAL; return -1; }
    struct addrinfo hints = {0}, *addresses = NULL;
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = type;
    hints.ai_flags = AI_NUMERICSERV | (binding ? AI_PASSIVE : 0);
    int result = getaddrinfo(host, port, &hints, &addresses);
    if (result != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
        errno = EINVAL; return -1;
    }
    int fd = -1, saved = EADDRNOTAVAIL;
    for (struct addrinfo *a = addresses; a; a = a->ai_next) {
        fd = socket(a->ai_family, a->ai_socktype, a->ai_protocol);
        if (fd < 0) { saved = errno; continue; }
        int one = 1;
        if (binding && setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one) < 0)
            goto next;
        /* Explicit IPv6-only binding makes the behavior independent of OS default. */
        if (binding && a->ai_family == AF_INET6 &&
            setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &one, sizeof one) < 0) goto next;
        if ((binding ? bind(fd, a->ai_addr, a->ai_addrlen)
                     : connect(fd, a->ai_addr, a->ai_addrlen)) == 0) break;
next:
        saved = errno; close_fd(fd); fd = -1;
    }
    freeaddrinfo(addresses);
    if (fd < 0) errno = saved;
    return fd;
}
int net_bound(const char *host, const char *port, int type) {
    return endpoint(host, port, type, 1);
}
int net_connect(const char *host, const char *port, int type) {
    return endpoint(host, port, type, 0);
}
int net_listen(const char *host, const char *port) {
    int fd = net_bound(host, port, SOCK_STREAM);
    if (fd >= 0 && listen(fd, 64) < 0) {
        int saved = errno; close_fd(fd); errno = saved; return -1;
    }
    return fd;
}
/* Step: Preserve existing descriptor flags when enabling nonblocking mode. */
int nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    return flags < 0 ? -1 : fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
int set_timeout(int fd, int seconds) {
    struct timeval timeout = {.tv_sec = seconds, .tv_usec = 0};
    if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) < 0) return -1;
    return setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof timeout);
}
/* Step: Blocking send may write only a prefix. Advance by the returned byte count. */
int send_all(int fd, const void *data, size_t length) {
    const unsigned char *bytes = data;
    size_t offset = 0;
    while (offset < length) {
        ssize_t n = send(fd, bytes + offset, length - offset, 0);
        if (n < 0 && errno == EINTR) continue;
        if (n <= 0) { if (n == 0) errno = EPIPE; return -1; }
        offset += (size_t)n;
    }
    return 0;
}
/* Step: Distinguish clean boundary EOF from a truncated record. Never use strlen here. */
int recv_exact(int fd, void *data, size_t length) {
    unsigned char *bytes = data;
    size_t offset = 0;
    while (offset < length) {
        ssize_t n = recv(fd, bytes + offset, length - offset, 0);
        if (n < 0 && errno == EINTR) continue;
        if (n < 0) return -1;
        if (n == 0) {
            if (offset == 0) return 0;
            errno = EPROTO; return -1;
        }
        offset += (size_t)n;
    }
    return 1;
}
/* Step: Four network-order length bytes form a portable, bounded application frame. */
int frame_send(int fd, const void *data, uint32_t length) {
    if (length > COURSE_FRAME_MAX) { errno = EMSGSIZE; return -1; }
    uint32_t header = htonl(length);
    if (send_all(fd, &header, sizeof header) < 0) return -1;
    return send_all(fd, data, length);
}
int frame_recv(int fd, void *data, uint32_t capacity, uint32_t *length) {
    uint32_t header;
    int result = recv_exact(fd, &header, sizeof header);
    if (result != 1) return result;
    *length = ntohl(header);
    if (*length > capacity || *length > COURSE_FRAME_MAX) { errno = EMSGSIZE; return -1; }
    result = recv_exact(fd, data, *length);
    if (result != 1) { if (result == 0) errno = EPROTO; return -1; }
    return 1;
}
/* Step: Raw echo treats all bytes, including NUL, as payload; EOF ends the session. */
int echo_connection(int fd) {
    unsigned char bytes[COURSE_CHUNK];
    for (;;) {
        ssize_t n = recv(fd, bytes, sizeof bytes, 0);
        if (n < 0 && errno == EINTR) continue;
        if (n < 0) return -1;
        if (n == 0) return 0;
        if (send_all(fd, bytes, (size_t)n) < 0) return -1;
    }
}
int accept_retry(int listener) {
    int fd;
    do { fd = accept(listener, NULL, NULL); } while (fd < 0 && errno == EINTR);
    return fd;
}
