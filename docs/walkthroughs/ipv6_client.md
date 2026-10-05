# ipv6_client — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../09-advanced-sockets/ipv6/client.c)

## What does this code do?

Run the ipv6 client implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–4 | Include declarations for the concrete OS/library APIs used below. |
| 5–15 | This same client resolves IPv4 or IPv6; IPv6 literals need no URL brackets here. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 7 | `strlen` | Measure trusted/local NUL-terminated text only; never infer binary payload length this way. |
| 8 | `net_init` | Install the course SIGPIPE policy so send failures return normally for handling. |
| 8 | `net_connect` | Resolve and try blocking connection candidates; success returns an owned descriptor. |
| 10 | `strlen` | Measure trusted/local NUL-terminated text only; never infer binary payload length this way. |
| 11 | `set_timeout` | Set per-operation blocking I/O timeouts; this is not a whole-transaction deadline. |
| 11 | `send_all` | Complete a blocking byte send by retaining offsets, or fail the exchange. |
| 12 | `recv_exact` | Read a defined byte count while distinguishing boundary EOF from truncation. |
| 13 | `fwrite` | Write an explicit byte count instead of assuming NUL-terminated network text. |
| 14 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <stdio.h>
  3  #include <string.h>
  4  #include <sys/socket.h>
  5  /* Step: This same client resolves IPv4 or IPv6; IPv6 literals need no URL brackets here. */
  6  int main(int argc, char **argv) {
  7      if (argc != 4 || strlen(argv[3]) > 4096) { fprintf(stderr, "usage: %s HOST PORT MESSAGE\n", argv[0]); return 1; }
  8      net_init(); int fd = net_connect(argv[1], argv[2], SOCK_STREAM);
  9      if (fd < 0) die("connect");
 10      size_t length = strlen(argv[3]); char reply[4096]; int status = 1;
 11      if (set_timeout(fd, 3) < 0 || send_all(fd, argv[3], length) < 0) perror("send");
 12      else if (recv_exact(fd, reply, length) != 1) fprintf(stderr, "incomplete echo\n");
 13      else if (fwrite(reply, 1, length, stdout) == length && putchar('\n') != EOF) status = 0;
 14      close_fd(fd); return status;
 15  }
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
