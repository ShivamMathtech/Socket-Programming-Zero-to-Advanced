# tcp_chat_server — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../11-real-world-projects/tcp-chat/server.c)

## What does this code do?

Run the tcp chat server implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–3 | Include declarations for the concrete OS/library APIs used below. |
| 4–15 | Accept exactly one peer, then run a duplex terminal session. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 7 | `net_init` | Install the course SIGPIPE policy so send failures return normally for handling. |
| 7 | `net_listen` | Resolve/bind/listen through the documented helper; the caller owns a successful descriptor. |
| 10 | `accept_retry` | Retry an interrupted accept while retaining the listener. |
| 10 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 12 | `chat_session` | Run a two-party terminal/network polling loop with documented EOF and write-timeout behavior. |
| 14 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include "chat.h"
  3  #include <stdio.h>
  4  /* Step: Accept exactly one peer, then run a duplex terminal session. */
  5  int main(int argc, char **argv) {
  6      if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
  7      net_init(); int listener = net_listen("127.0.0.1", argv[1]);
  8      if (listener < 0) die("listen");
  9      fprintf(stderr, "Chat listening on %s\n", argv[1]);
 10      int peer = accept_retry(listener); close_fd(listener);
 11      if (peer < 0) die("accept");
 12      int result = chat_session(peer);
 13      if (result < 0) perror("chat");
 14      close_fd(peer); return result < 0 ? 1 : 0;
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
