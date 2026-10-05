# frame_server — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../04-tcp-client-server/server.c)

## What does this code do?

Run the frame server implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–3 | Include declarations for the concrete OS/library APIs used below. |
| 4–15 | The shared helpers below expose, rather than replace, the send/recv loops. |
| 16–26 | A per-operation timeout bounds idle blocking, not total request duration. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 7 | `net_init` | Install the course SIGPIPE policy so send failures return normally for handling. |
| 8 | `net_listen` | Resolve/bind/listen through the documented helper; the caller owns a successful descriptor. |
| 10 | `malloc` | Allocate storage whose lifetime survives this loop iteration; transfer or release ownership explicitly. |
| 11 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 14 | `accept_retry` | Retry an interrupted accept while retaining the listener. |
| 17 | `set_timeout` | Set per-operation blocking I/O timeouts; this is not a whole-transaction deadline. |
| 17 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 20 | `frame_recv` | Validate the length before filling the caller body buffer; report frame, EOF or error distinctly. |
| 21 | `frame_send` | Serialize a bounded four-byte length and body; an empty body still has a header. |
| 23 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 25 | `free` | Release heap storage once it is no longer needed. |
| 25 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <stdio.h>
  3  #include <stdlib.h>
  4  /* Step: The shared helpers below expose, rather than replace, the send/recv loops. */
  5  int main(int argc, char **argv) {
  6      if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
  7      net_init();
  8      int listener = net_listen("127.0.0.1", argv[1]);
  9      if (listener < 0) die("listen");
 10      unsigned char *buffer = malloc(COURSE_FRAME_MAX);
 11      if (!buffer) { close_fd(listener); die("malloc"); }
 12      printf("Framed echo on 127.0.0.1:%s\n", argv[1]); fflush(stdout);
 13      for (;;) {
 14          int peer = accept_retry(listener);
 15          if (peer < 0) { perror("accept"); break; }
 16          /* Step: A per-operation timeout bounds idle blocking, not total request duration. */
 17          if (set_timeout(peer, 15) < 0) { perror("timeout"); close_fd(peer); continue; }
 18          uint32_t length;
 19          int result;
 20          while ((result = frame_recv(peer, buffer, COURSE_FRAME_MAX, &length)) == 1)
 21              if (frame_send(peer, buffer, length) < 0) { result = -1; break; }
 22          if (result < 0) perror("frame");
 23          close_fd(peer);
 24      }
 25      free(buffer); close_fd(listener); return 1;
 26  }
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
