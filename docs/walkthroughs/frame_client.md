# frame_client — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../04-tcp-client-server/client.c)

## What does this code do?

Run the frame client implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–5 | Include declarations for the concrete OS/library APIs used below. |
| 6–20 | Read one input line, send a length-prefixed frame, then receive one frame. |
| 21–27 | fwrite uses the received length, so a remote NUL is not a terminator. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 9 | `net_init` | Install the course SIGPIPE policy so send failures return normally for handling. |
| 10 | `net_connect` | Resolve and try blocking connection candidates; success returns an owned descriptor. |
| 12 | `set_timeout` | Set per-operation blocking I/O timeouts; this is not a whole-transaction deadline. |
| 12 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 16 | `strlen` | Measure trusted/local NUL-terminated text only; never infer binary payload length this way. |
| 17 | `frame_send` | Serialize a bounded four-byte length and body; an empty body still has a header. |
| 18 | `frame_recv` | Validate the length before filling the caller body buffer; report frame, EOF or error distinctly. |
| 22 | `fwrite` | Write an explicit byte count instead of assuming NUL-terminated network text. |
| 26 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <stdio.h>
  3  #include <stdlib.h>
  4  #include <string.h>
  5  #include <sys/socket.h>
  6  /* Step: Read one input line, send a length-prefixed frame, then receive one frame. */
  7  int main(int argc, char **argv) {
  8      if (argc != 3) { fprintf(stderr, "usage: %s HOST PORT < lines.txt\n", argv[0]); return 1; }
  9      net_init();
 10      int fd = net_connect(argv[1], argv[2], SOCK_STREAM);
 11      if (fd < 0) die("connect");
 12      if (set_timeout(fd, 15) < 0) { close_fd(fd); die("timeout"); }
 13      char input[4096], output[4096];
 14      int status = 0;
 15      while (fgets(input, sizeof input, stdin)) {
 16          uint32_t length = (uint32_t)strlen(input), received;
 17          if (frame_send(fd, input, length) < 0 ||
 18              frame_recv(fd, output, sizeof output, &received) != 1) {
 19              fprintf(stderr, "frame exchange failed or peer closed\n"); status = 1; break;
 20          }
 21          /* Step: fwrite uses the received length, so a remote NUL is not a terminator. */
 22          if (fwrite(output, 1, received, stdout) != received) { status = 1; break; }
 23          fflush(stdout);
 24      }
 25      if (ferror(stdin)) status = 1;
 26      close_fd(fd); return status;
 27  }
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
