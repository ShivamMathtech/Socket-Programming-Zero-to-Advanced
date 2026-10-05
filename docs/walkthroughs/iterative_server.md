# iterative_server — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../06-multiple-client-server/iterative-server/server.c)

## What does this code do?

Run the iterative server implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–2 | Include declarations for the concrete OS/library APIs used below. |
| 3–17 | A loop accepts many clients over time, but serves only one at a time. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 6 | `net_init` | Install the course SIGPIPE policy so send failures return normally for handling. |
| 7 | `net_listen` | Resolve/bind/listen through the documented helper; the caller owns a successful descriptor. |
| 11 | `accept_retry` | Retry an interrupted accept while retaining the listener. |
| 13 | `set_timeout` | Set per-operation blocking I/O timeouts; this is not a whole-transaction deadline. |
| 13 | `echo_connection` | Loop over binary receive prefixes and fully echo each one until EOF/failure. |
| 14 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 16 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <stdio.h>
  3  /* Step: A loop accepts many clients over time, but serves only one at a time. */
  4  int main(int argc, char **argv) {
  5      if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
  6      net_init();
  7      int listener = net_listen("127.0.0.1", argv[1]);
  8      if (listener < 0) die("listen");
  9      printf("Iterative echo on %s\n", argv[1]); fflush(stdout);
 10      for (;;) {
 11          int peer = accept_retry(listener);
 12          if (peer < 0) { perror("accept"); break; }
 13          if (set_timeout(peer, 15) < 0 || echo_connection(peer) < 0) perror("client");
 14          close_fd(peer);
 15      }
 16      close_fd(listener); return 1;
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
