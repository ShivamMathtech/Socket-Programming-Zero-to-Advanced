# udp_broadcast — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../05-udp-programming/broadcast.c)

## What does this code do?

Run the udp broadcast implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–7 | Include declarations for the concrete OS/library APIs used below. |
| 8–28 | A single explicit broadcast demonstrates SO_BROADCAST; no discovery loop is hidden. |


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
| 19 | `setsockopt` | Set an option with the correct level, value type and byte size; inspect failure immediately. |
| 22 | `sendto` | Send one UDP datagram to the explicit endpoint. Local success does not prove delivery. |
| 22 | `strlen` | Measure trusted/local NUL-terminated text only; never infer binary payload length this way. |
| 27 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

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
  8  /* Step: A single explicit broadcast demonstrates SO_BROADCAST; no discovery loop is hidden. */
  9  int main(int argc, char **argv) {
 10      if (argc != 4 || !valid_port(argv[2]) || strlen(argv[3]) > 1200) {
 11          fprintf(stderr, "usage: %s BROADCAST_IP PORT MESSAGE\n", argv[0]); return 1;
 12      }
 13      struct sockaddr_in address = {0};
 14      address.sin_family = AF_INET; address.sin_port = htons((uint16_t)strtoul(argv[2], NULL, 10));
 15      if (inet_pton(AF_INET, argv[1], &address.sin_addr) != 1) return 1;
 16      int fd = socket(AF_INET, SOCK_DGRAM, 0);
 17      if (fd < 0) die("socket");
 18      int one = 1, status = 1;
 19      if (setsockopt(fd, SOL_SOCKET, SO_BROADCAST, &one, sizeof one) < 0) perror("SO_BROADCAST");
 20      else {
 21          ssize_t n;
 22          do { n = sendto(fd, argv[3], strlen(argv[3]), 0, (struct sockaddr *)&address, sizeof address); }
 23          while (n < 0 && errno == EINTR);
 24          if (n < 0) perror("sendto");
 25          else { printf("Sent %zd bytes once\n", n); status = 0; }
 26      }
 27      close_fd(fd); return status;
 28  }
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
