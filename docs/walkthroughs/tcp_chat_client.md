# tcp_chat_client — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../11-real-world-projects/tcp-chat/client.c)

## What does this code do?

Run the tcp chat client implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–4 | Include declarations for the concrete OS/library APIs used below. |
| 5–13 | The client and server share the same duplex behavior after establishment. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 8 | `net_init` | Install the course SIGPIPE policy so send failures return normally for handling. |
| 8 | `net_connect` | Resolve and try blocking connection candidates; success returns an owned descriptor. |
| 10 | `chat_session` | Run a two-party terminal/network polling loop with documented EOF and write-timeout behavior. |
| 12 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include "chat.h"
  3  #include <stdio.h>
  4  #include <sys/socket.h>
  5  /* Step: The client and server share the same duplex behavior after establishment. */
  6  int main(int argc, char **argv) {
  7      if (argc != 3) { fprintf(stderr, "usage: %s HOST PORT\n", argv[0]); return 1; }
  8      net_init(); int fd = net_connect(argv[1], argv[2], SOCK_STREAM);
  9      if (fd < 0) die("connect");
 10      int result = chat_session(fd);
 11      if (result < 0) perror("chat");
 12      close_fd(fd); return result < 0 ? 1 : 0;
 13  }
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
