# udp_chat — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../11-real-world-projects/udp-chat/peer.c)

## What does this code do?

Run the udp chat implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–8 | Include declarations for the concrete OS/library APIs used below. |
| 9–33 | Both peers bind a local port, then configure one default remote UDP endpoint. |
| 34–47 | A connected UDP socket still sends datagrams and does not become reliable. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 14 | `net_bound` | Resolve candidates and bind a socket; this does not create a TCP listener by itself. |
| 17 | `htons` | Encode a 16-bit port in network byte order. |
| 17 | `strtoul` | Parse a validated unsigned numeric port; subsequent conversion must fit its defined range. |
| 18 | `inet_pton` | Convert numeric address text; return 1 means valid conversion, not merely nonnegative. |
| 18 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 19 | `connect` | Attempt peer association/establishment. Nonblocking TCP may report EINPROGRESS instead of completing. |
| 19 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 24 | `poll` | Wait on requested events and inspect returned revents, including hangup/error conditions. |
| 30 | `read` | Read available local file/terminal bytes; handle EOF/error before interpreting a count. |
| 36 | `send` | Offer this byte range to the kernel. Only a positive returned prefix has progressed; retain any suffix. |
| 40 | `recv` | Fill at most the supplied capacity. Check signed return before using it as a length; positive-length TCP zero means EOF. |
| 43 | `fwrite` | Write an explicit byte count instead of assuming NUL-terminated network text. |
| 46 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <arpa/inet.h>
  3  #include <errno.h>
  4  #include <poll.h>
  5  #include <stdio.h>
  6  #include <stdlib.h>
  7  #include <sys/socket.h>
  8  #include <unistd.h>
  9  /* Step: Both peers bind a local port, then configure one default remote UDP endpoint. */
 10  int main(int argc, char **argv) {
 11      if (argc != 4 || !valid_port(argv[3])) {
 12          fprintf(stderr, "usage: %s LOCAL_PORT PEER_IPv4 PEER_PORT\n", argv[0]); return 1;
 13      }
 14      int fd = net_bound("127.0.0.1", argv[1], SOCK_DGRAM);
 15      if (fd < 0) die("bind");
 16      struct sockaddr_in peer = {0}; peer.sin_family = AF_INET;
 17      peer.sin_port = htons((uint16_t)strtoul(argv[3], NULL, 10));
 18      if (inet_pton(AF_INET, argv[2], &peer.sin_addr) != 1) { close_fd(fd); return 1; }
 19      if (connect(fd, (struct sockaddr *)&peer, sizeof peer) < 0) { close_fd(fd); die("UDP connect"); }
 20      fprintf(stderr, "UDP peer ready on %s; Ctrl-D exits locally\n", argv[1]);
 21      int status = 0;
 22      for (;;) {
 23          struct pollfd items[2] = {{.fd=STDIN_FILENO,.events=POLLIN},{.fd=fd,.events=POLLIN}};
 24          int ready = poll(items, 2, -1);
 25          if (ready < 0 && errno == EINTR) continue;
 26          if (ready < 0) { perror("poll"); status = 1; break; }
 27          if (items[1].revents & (POLLERR | POLLNVAL)) { fprintf(stderr,"UDP endpoint error; start both peers\n"); status=1; break; }
 28          char data[65536];
 29          if (items[0].revents & (POLLIN | POLLHUP)) {
 30              ssize_t n = read(STDIN_FILENO, data, 1200);
 31              if (n < 0 && errno == EINTR) continue;
 32              if (n < 0) { perror("stdin"); status = 1; break; }
 33              if (n == 0) break;
 34              /* Step: A connected UDP socket still sends datagrams and does not become reliable. */
 35              ssize_t sent;
 36              do { sent = send(fd, data, (size_t)n, 0); } while (sent < 0 && errno == EINTR);
 37              if (sent != n) { perror("send datagram"); status = 1; break; }
 38          }
 39          if (items[1].revents & POLLIN) {
 40              ssize_t n = recv(fd, data, sizeof data, 0);
 41              if (n < 0 && errno == EINTR) continue;
 42              if (n < 0) { perror("recv"); status = 1; break; }
 43              if (fwrite(data, 1, (size_t)n, stdout) != (size_t)n || fflush(stdout) == EOF) { status = 1; break; }
 44          }
 45      }
 46      close_fd(fd); return status;
 47  }
```

## What if it fails?

Inspect each signed return before treating it as a byte count. Failed acquisition
must not be used as a descriptor. After ownership is acquired, every exit path
must close or explicitly transfer that resource. The source shows whether an
error ends the entire program, only a peer, or one retry attempt.

See the matching chapter and protocol-limits guide for capacity, concurrency and timeout assumptions. All network payload counts must be validated before copying.

## Modify it deliberately

Complete the chapter modification exercise, preserving cleanup, partial progress and its wire-format contract.

Refer to the [API reference](../socket-api.md) and
[protocol limits](../protocols-and-limits.md), then run a relevant integration
check and record the observation. Do not claim that merely compiling proves the
protocol works.
