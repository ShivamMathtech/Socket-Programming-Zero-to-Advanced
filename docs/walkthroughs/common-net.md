# common-net — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../common/net.c)

## What does this code do?

Implement the blocking POSIX helper contracts used in later lessons.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–12 | Include declarations for the concrete OS/library APIs used below. |
| 13–32 | SIGPIPE would otherwise terminate a process before it handles EPIPE. |
| 33–76 | Resolve both address families, try candidates, and free the whole list. |
| 77–86 | Preserve existing descriptor flags when enabling nonblocking mode. |
| 87–98 | Blocking send may write only a prefix. Advance by the returned byte count. |
| 99–114 | Distinguish clean boundary EOF from a truncated record. Never use strlen here. |
| 115–131 | Four network-order length bytes form a portable, bounded application frame. |
| 132–147 | Raw echo treats all bytes, including NUL, as payload; EOF ends the session. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 23 | `close` | Release the owned descriptor once. This Linux code does not blindly retry close failures. |
| 30 | `strtoul` | Parse a validated unsigned numeric port; subsequent conversion must fit its defined range. |
| 40 | `getaddrinfo` | Obtain candidate addresses; report its returned EAI code through gai_strerror, not assumed errno. |
| 47 | `socket` | Allocate a descriptor; -1 fails, while zero and positive descriptor numbers are valid. |
| 50 | `setsockopt` | Set an option with the correct level, value type and byte size; inspect failure immediately. |
| 54 | `setsockopt` | Set an option with the correct level, value type and byte size; inspect failure immediately. |
| 55 | `bind` | Assign the local endpoint using the provided family-specific address size; failure needs cleanup. |
| 56 | `connect` | Attempt peer association/establishment. Nonblocking TCP may report EINPROGRESS instead of completing. |
| 58 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 60 | `freeaddrinfo` | Release the entire candidate list after success or exhausting alternatives. |
| 71 | `net_bound` | Resolve candidates and bind a socket; this does not create a TCP listener by itself. |
| 72 | `listen` | Enable a passive stream listener; the backlog is a pending-queue request, not a worker limit. |
| 73 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 79 | `fcntl` | Inspect/preserve existing flags while changing nonblocking behavior. |
| 80 | `fcntl` | Inspect/preserve existing flags while changing nonblocking behavior. |
| 84 | `setsockopt` | Set an option with the correct level, value type and byte size; inspect failure immediately. |
| 85 | `setsockopt` | Set an option with the correct level, value type and byte size; inspect failure immediately. |
| 92 | `send` | Offer this byte range to the kernel. Only a positive returned prefix has progressed; retain any suffix. |
| 104 | `recv` | Fill at most the supplied capacity. Check signed return before using it as a length; positive-length TCP zero means EOF. |
| 118 | `htonl` | Encode an unsigned 32-bit value in network byte order. |
| 119 | `send_all` | Complete a blocking byte send by retaining offsets, or fail the exchange. |
| 120 | `send_all` | Complete a blocking byte send by retaining offsets, or fail the exchange. |
| 124 | `recv_exact` | Read a defined byte count while distinguishing boundary EOF from truncation. |
| 128 | `recv_exact` | Read a defined byte count while distinguishing boundary EOF from truncation. |
| 136 | `recv` | Fill at most the supplied capacity. Check signed return before using it as a length; positive-length TCP zero means EOF. |
| 140 | `send_all` | Complete a blocking byte send by retaining offsets, or fail the exchange. |
| 145 | `accept` | Obtain a connected descriptor distinct from the listener. Set its ownership and mode explicitly. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <arpa/inet.h>
  3  #include <errno.h>
  4  #include <fcntl.h>
  5  #include <netdb.h>
  6  #include <signal.h>
  7  #include <stdio.h>
  8  #include <stdlib.h>
  9  #include <string.h>
 10  #include <sys/socket.h>
 11  #include <sys/time.h>
 12  #include <unistd.h>
 13  /* Step: SIGPIPE would otherwise terminate a process before it handles EPIPE. */
 14  void net_init(void) {
 15      struct sigaction action = {0};
 16      action.sa_handler = SIG_IGN;
 17      if (sigemptyset(&action.sa_mask) < 0 || sigaction(SIGPIPE, &action, NULL) < 0)
 18          die("sigaction");
 19  }
 20  void die(const char *what) { perror(what); exit(EXIT_FAILURE); }
 21  void close_fd(int fd) {
 22      /* Linux: do not retry close after EINTR; the descriptor may be reused. */
 23      if (fd >= 0 && close(fd) < 0) perror("close");
 24  }
 25  int valid_port(const char *port) {
 26      if (!port || !*port) return 0;
 27      for (const char *p = port; *p; ++p) if (*p < '0' || *p > '9') return 0;
 28      errno = 0;
 29      char *end;
 30      unsigned long n = strtoul(port, &end, 10);
 31      return !errno && *end == '\0' && n > 0 && n <= 65535;
 32  }
 33  /* Step: Resolve both address families, try candidates, and free the whole list. */
 34  static int endpoint(const char *host, const char *port, int type, int binding) {
 35      if (!valid_port(port)) { errno = EINVAL; return -1; }
 36      struct addrinfo hints = {0}, *addresses = NULL;
 37      hints.ai_family = AF_UNSPEC;
 38      hints.ai_socktype = type;
 39      hints.ai_flags = AI_NUMERICSERV | (binding ? AI_PASSIVE : 0);
 40      int result = getaddrinfo(host, port, &hints, &addresses);
 41      if (result != 0) {
 42          fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(result));
 43          errno = EINVAL; return -1;
 44      }
 45      int fd = -1, saved = EADDRNOTAVAIL;
 46      for (struct addrinfo *a = addresses; a; a = a->ai_next) {
 47          fd = socket(a->ai_family, a->ai_socktype, a->ai_protocol);
 48          if (fd < 0) { saved = errno; continue; }
 49          int one = 1;
 50          if (binding && setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one) < 0)
 51              goto next;
 52          /* Explicit IPv6-only binding makes the behavior independent of OS default. */
 53          if (binding && a->ai_family == AF_INET6 &&
 54              setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, &one, sizeof one) < 0) goto next;
 55          if ((binding ? bind(fd, a->ai_addr, a->ai_addrlen)
 56                       : connect(fd, a->ai_addr, a->ai_addrlen)) == 0) break;
 57  next:
 58          saved = errno; close_fd(fd); fd = -1;
 59      }
 60      freeaddrinfo(addresses);
 61      if (fd < 0) errno = saved;
 62      return fd;
 63  }
 64  int net_bound(const char *host, const char *port, int type) {
 65      return endpoint(host, port, type, 1);
 66  }
 67  int net_connect(const char *host, const char *port, int type) {
 68      return endpoint(host, port, type, 0);
 69  }
 70  int net_listen(const char *host, const char *port) {
 71      int fd = net_bound(host, port, SOCK_STREAM);
 72      if (fd >= 0 && listen(fd, 64) < 0) {
 73          int saved = errno; close_fd(fd); errno = saved; return -1;
 74      }
 75      return fd;
 76  }
 77  /* Step: Preserve existing descriptor flags when enabling nonblocking mode. */
 78  int nonblocking(int fd) {
 79      int flags = fcntl(fd, F_GETFL, 0);
 80      return flags < 0 ? -1 : fcntl(fd, F_SETFL, flags | O_NONBLOCK);
 81  }
 82  int set_timeout(int fd, int seconds) {
 83      struct timeval timeout = {.tv_sec = seconds, .tv_usec = 0};
 84      if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout) < 0) return -1;
 85      return setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof timeout);
 86  }
 87  /* Step: Blocking send may write only a prefix. Advance by the returned byte count. */
 88  int send_all(int fd, const void *data, size_t length) {
 89      const unsigned char *bytes = data;
 90      size_t offset = 0;
 91      while (offset < length) {
 92          ssize_t n = send(fd, bytes + offset, length - offset, 0);
 93          if (n < 0 && errno == EINTR) continue;
 94          if (n <= 0) { if (n == 0) errno = EPIPE; return -1; }
 95          offset += (size_t)n;
 96      }
 97      return 0;
 98  }
 99  /* Step: Distinguish clean boundary EOF from a truncated record. Never use strlen here. */
100  int recv_exact(int fd, void *data, size_t length) {
101      unsigned char *bytes = data;
102      size_t offset = 0;
103      while (offset < length) {
104          ssize_t n = recv(fd, bytes + offset, length - offset, 0);
105          if (n < 0 && errno == EINTR) continue;
106          if (n < 0) return -1;
107          if (n == 0) {
108              if (offset == 0) return 0;
109              errno = EPROTO; return -1;
110          }
111          offset += (size_t)n;
112      }
113      return 1;
114  }
115  /* Step: Four network-order length bytes form a portable, bounded application frame. */
116  int frame_send(int fd, const void *data, uint32_t length) {
117      if (length > COURSE_FRAME_MAX) { errno = EMSGSIZE; return -1; }
118      uint32_t header = htonl(length);
119      if (send_all(fd, &header, sizeof header) < 0) return -1;
120      return send_all(fd, data, length);
121  }
122  int frame_recv(int fd, void *data, uint32_t capacity, uint32_t *length) {
123      uint32_t header;
124      int result = recv_exact(fd, &header, sizeof header);
125      if (result != 1) return result;
126      *length = ntohl(header);
127      if (*length > capacity || *length > COURSE_FRAME_MAX) { errno = EMSGSIZE; return -1; }
128      result = recv_exact(fd, data, *length);
129      if (result != 1) { if (result == 0) errno = EPROTO; return -1; }
130      return 1;
131  }
132  /* Step: Raw echo treats all bytes, including NUL, as payload; EOF ends the session. */
133  int echo_connection(int fd) {
134      unsigned char bytes[COURSE_CHUNK];
135      for (;;) {
136          ssize_t n = recv(fd, bytes, sizeof bytes, 0);
137          if (n < 0 && errno == EINTR) continue;
138          if (n < 0) return -1;
139          if (n == 0) return 0;
140          if (send_all(fd, bytes, (size_t)n) < 0) return -1;
141      }
142  }
143  int accept_retry(int listener) {
144      int fd;
145      do { fd = accept(listener, NULL, NULL); } while (fd < 0 && errno == EINTR);
146      return fd;
147  }
```

## What if it fails?

Inspect each signed return before treating it as a byte count. Failed acquisition
must not be used as a descriptor. After ownership is acquired, every exit path
must close or explicitly transfer that resource. The source shows whether an
error ends the entire program, only a peer, or one retry attempt.

send_all is a blocking helper; EAGAIN is an error here rather than resumable reactor state. Endpoint resolution/connect has no overall deadline.

## Modify it deliberately

Add a deadline-aware variant with an explicit contract; do not silently change existing callers.

Refer to the [API reference](../socket-api.md) and
[protocol limits](../protocols-and-limits.md), then run a relevant integration
check and record the observation. Do not claim that merely compiling proves the
protocol works.
