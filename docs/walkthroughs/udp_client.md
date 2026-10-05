# udp_client — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../05-udp-programming/udp-client.c)

## What does this code do?

Run the udp client implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–7 | Include declarations for the concrete OS/library APIs used below. |
| 8–24 | Small UDP requests avoid fragmentation in this introductory exercise. |
| 25–38 | Verify the reply endpoint; a source IP/port check is not authentication. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 10 | `strlen` | Measure trusted/local NUL-terminated text only; never infer binary payload length this way. |
| 14 | `htons` | Encode a 16-bit port in network byte order. |
| 14 | `strtoul` | Parse a validated unsigned numeric port; subsequent conversion must fit its defined range. |
| 15 | `inet_pton` | Convert numeric address text; return 1 means valid conversion, not merely nonnegative. |
| 16 | `socket` | Allocate a descriptor; -1 fails, while zero and positive descriptor numbers are valid. |
| 18 | `set_timeout` | Set per-operation blocking I/O timeouts; this is not a whole-transaction deadline. |
| 18 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 19 | `strlen` | Measure trusted/local NUL-terminated text only; never infer binary payload length this way. |
| 21 | `sendto` | Send one UDP datagram to the explicit endpoint. Local success does not prove delivery. |
| 28 | `recvfrom` | Receive one datagram plus optional source metadata; reset source-address capacity first. |
| 30 | `recvfrom` | Receive one datagram plus optional source metadata; reset source-address capacity first. |
| 34 | `fwrite` | Write an explicit byte count instead of assuming NUL-terminated network text. |
| 37 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <arpa/inet.h>
  3  #include <errno.h>
  4  #include <stdio.h>
  5  #include <stdlib.h>
  6  #include <string.h>
  7  #include <sys/socket.h>
  8  /* Step: Small UDP requests avoid fragmentation in this introductory exercise. */
  9  int main(int argc, char **argv) {
 10      if (argc != 4 || !valid_port(argv[2]) || strlen(argv[3]) > 1200) {
 11          fprintf(stderr, "usage: %s IPv4 PORT MESSAGE (<=1200 bytes)\n", argv[0]); return 1;
 12      }
 13      struct sockaddr_in peer = {0};
 14      peer.sin_family = AF_INET; peer.sin_port = htons((uint16_t)strtoul(argv[2], NULL, 10));
 15      if (inet_pton(AF_INET, argv[1], &peer.sin_addr) != 1) return 1;
 16      int fd = socket(AF_INET, SOCK_DGRAM, 0);
 17      if (fd < 0) die("socket");
 18      if (set_timeout(fd, 3) < 0) { close_fd(fd); die("timeout"); }
 19      size_t count = strlen(argv[3]);
 20      ssize_t n;
 21      do { n = sendto(fd, argv[3], count, 0, (struct sockaddr *)&peer, sizeof peer); }
 22      while (n < 0 && errno == EINTR);
 23      int status = 1;
 24      if (n < 0 || (size_t)n != count) { perror("sendto"); goto done; }
 25      /* Step: Verify the reply endpoint; a source IP/port check is not authentication. */
 26      struct sockaddr_in sender; socklen_t length = sizeof sender;
 27      char data[65536];
 28      do { n = recvfrom(fd, data, sizeof data, 0, (struct sockaddr *)&sender, &length); }
 29      while (n < 0 && errno == EINTR);
 30      if (n < 0) { perror("recvfrom (3 second timeout)"); goto done; }
 31      if (sender.sin_addr.s_addr != peer.sin_addr.s_addr || sender.sin_port != peer.sin_port) {
 32          fprintf(stderr, "unexpected source\n"); goto done;
 33      }
 34      if (fwrite(data, 1, (size_t)n, stdout) != (size_t)n || putchar('\n') == EOF) goto done;
 35      status = 0;
 36  done:
 37      close_fd(fd); return status;
 38  }
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
