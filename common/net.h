#ifndef COURSE_NET_H
#define COURSE_NET_H
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
/* Read net.c: these helpers are short POSIX loops, not a framework. */
#define COURSE_FRAME_MAX (1024U * 1024U)
#define COURSE_CHUNK 4096
void net_init(void);
void close_fd(int fd);
void die(const char *what);
int valid_port(const char *port);
int net_bound(const char *host, const char *port, int type);
int net_listen(const char *host, const char *port);
int net_connect(const char *host, const char *port, int type);
int nonblocking(int fd);
int set_timeout(int fd, int seconds);
int send_all(int fd, const void *data, size_t length);
/* recv_exact: 1 complete; 0 EOF before any bytes; -1 error/truncation. */
int recv_exact(int fd, void *data, size_t length);
int frame_send(int fd, const void *data, uint32_t length);
int frame_recv(int fd, void *data, uint32_t capacity, uint32_t *length);
int echo_connection(int fd);
int accept_retry(int listener);
#endif
