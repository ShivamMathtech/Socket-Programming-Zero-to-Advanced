# socket_options — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../09-advanced-sockets/socket-options/options.c)

## What does this code do?

Run the socket options implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–3 | Include declarations for the concrete OS/library APIs used below. |
| 4–17 | Query the kernel after setting options; effective buffer sizes are implementation dependent. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 6 | `socket` | Allocate a descriptor; -1 fails, while zero and positive descriptor numbers are valid. |
| 9 | `setsockopt` | Set an option with the correct level, value type and byte size; inspect failure immediately. |
| 9 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 11 | `getsockopt` | Read an option into caller storage; length is capacity on entry and result size afterward. |
| 11 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 14 | `getsockopt` | Read an option into caller storage; length is capacity on entry and result size afterward. |
| 14 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 16 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <stdio.h>
  3  #include <sys/socket.h>
  4  /* Step: Query the kernel after setting options; effective buffer sizes are implementation dependent. */
  5  int main(void) {
  6      int fd = socket(AF_INET, SOCK_STREAM, 0);
  7      if (fd < 0) die("socket");
  8      int one = 1, value = 0;
  9      if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &one, sizeof one) < 0) { close_fd(fd); die("setsockopt"); }
 10      socklen_t size = sizeof value;
 11      if (getsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &value, &size) < 0) { close_fd(fd); die("getsockopt"); }
 12      printf("SO_KEEPALIVE=%d\n", value);
 13      size = sizeof value;
 14      if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &size) < 0) { close_fd(fd); die("SO_RCVBUF"); }
 15      printf("SO_RCVBUF=%d (host-dependent)\n", value);
 16      close_fd(fd); return 0;
 17  }
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
