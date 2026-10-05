# udp_server — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../05-udp-programming/udp-server.c)

## What does this code do?

Run the udp server implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–4 | Include declarations for the concrete OS/library APIs used below. |
| 5–14 | UDP binds an endpoint but never calls listen or accept. |
| 15–25 | Reinitialize length on every receive; even zero bytes is a valid datagram. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 8 | `net_bound` | Resolve candidates and bind a socket; this does not create a TCP listener by itself. |
| 16 | `recvfrom` | Receive one datagram plus optional source metadata; reset source-address capacity first. |
| 20 | `sendto` | Send one UDP datagram to the explicit endpoint. Local success does not prove delivery. |
| 24 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <errno.h>
  3  #include <stdio.h>
  4  #include <sys/socket.h>
  5  /* Step: UDP binds an endpoint but never calls listen or accept. */
  6  int main(int argc, char **argv) {
  7      if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
  8      int fd = net_bound("127.0.0.1", argv[1], SOCK_DGRAM);
  9      if (fd < 0) die("bind UDP");
 10      printf("UDP echo on 127.0.0.1:%s\n", argv[1]); fflush(stdout);
 11      for (;;) {
 12          unsigned char data[65536];
 13          struct sockaddr_storage peer;
 14          socklen_t length = sizeof peer;
 15          /* Step: Reinitialize length on every receive; even zero bytes is a valid datagram. */
 16          ssize_t n = recvfrom(fd, data, sizeof data, 0, (struct sockaddr *)&peer, &length);
 17          if (n < 0 && errno == EINTR) continue;
 18          if (n < 0) { perror("recvfrom"); break; }
 19          ssize_t sent;
 20          do { sent = sendto(fd, data, (size_t)n, 0, (struct sockaddr *)&peer, length); }
 21          while (sent < 0 && errno == EINTR);
 22          if (sent != n) perror("sendto");
 23      }
 24      close_fd(fd); return 1;
 25  }
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
