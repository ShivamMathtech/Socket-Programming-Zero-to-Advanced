# receive_timeout — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../09-advanced-sockets/timeouts/timeout.c)

## What does this code do?

Run the receive timeout implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–4 | Include declarations for the concrete OS/library APIs used below. |
| 5–19 | A local connected pair isolates receive timeout behavior from routing and DNS. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 8 | `socketpair` | Create two local connected endpoints for an isolated error/timeout experiment. |
| 10 | `set_timeout` | Set per-operation blocking I/O timeouts; this is not a whole-transaction deadline. |
| 13 | `recv` | Fill at most the supplied capacity. Check signed return before using it as a length; positive-length TCP zero means EOF. |
| 18 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 18 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <errno.h>
  3  #include <stdio.h>
  4  #include <sys/socket.h>
  5  /* Step: A local connected pair isolates receive timeout behavior from routing and DNS. */
  6  int main(void) {
  7      int pair[2];
  8      if (socketpair(AF_UNIX, SOCK_STREAM, 0, pair) < 0) die("socketpair");
  9      int status = 1;
 10      if (set_timeout(pair[0], 1) < 0) perror("timeout");
 11      else {
 12          char byte;
 13          ssize_t n = recv(pair[0], &byte, 1, 0);
 14          if (n < 0 && (errno == EAGAIN || errno == EWOULDBLOCK)) {
 15              puts("Receive timed out; peer is still open"); status = 0;
 16          } else fprintf(stderr, "unexpected recv result\n");
 17      }
 18      close_fd(pair[0]); close_fd(pair[1]); return status;
 19  }
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
