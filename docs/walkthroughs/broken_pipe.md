# broken_pipe — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../09-advanced-sockets/error-handling/broken-pipe.c)

## What does this code do?

Run the broken pipe implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–4 | Include declarations for the concrete OS/library APIs used below. |
| 5–15 | Ignoring SIGPIPE lets normal control flow inspect EPIPE instead of losing the process. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 7 | `net_init` | Install the course SIGPIPE policy so send failures return normally for handling. |
| 8 | `socketpair` | Create two local connected endpoints for an isolated error/timeout experiment. |
| 9 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 10 | `send_all` | Complete a blocking byte send by retaining offsets, or fail the exchange. |
| 12 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <errno.h>
  3  #include <stdio.h>
  4  #include <sys/socket.h>
  5  /* Step: Ignoring SIGPIPE lets normal control flow inspect EPIPE instead of losing the process. */
  6  int main(void) {
  7      net_init(); int pair[2];
  8      if (socketpair(AF_UNIX, SOCK_STREAM, 0, pair) < 0) die("socketpair");
  9      close_fd(pair[1]);
 10      int result = send_all(pair[0], "x", 1);
 11      int saved = errno;
 12      close_fd(pair[0]);
 13      if (result < 0 && saved == EPIPE) { puts("EPIPE handled without termination"); return 0; }
 14      fprintf(stderr, "expected EPIPE in this Linux AF_UNIX demonstration\n"); return 1;
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
