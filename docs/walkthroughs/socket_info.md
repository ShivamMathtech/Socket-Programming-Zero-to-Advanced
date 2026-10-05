# socket_info — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../01-socket-fundamentals/examples/socket-info.c)

## What does this code do?

Allocate and inspect an unconnected socket.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–4 | Include declarations for the concrete OS/library APIs used below. |
| 5–8 | socket allocates a descriptor; it does not create a TCP connection. |
| 9–15 | getsockopt writes both the option value and its actual byte length. |
| 16–19 | Each successful socket needs an owner responsible for closing it. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 7 | `socket` | Allocate a descriptor; -1 fails, while zero and positive descriptor numbers are valid. |
| 12 | `getsockopt` | Read an option into caller storage; length is capacity on entry and result size afterward. |
| 13 | `close` | Release the owned descriptor once. This Linux code does not blindly retry close failures. |
| 17 | `close` | Release the owned descriptor once. This Linux code does not blindly retry close failures. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include <stdio.h>
  2  #include <stdlib.h>
  3  #include <sys/socket.h>
  4  #include <unistd.h>
  5  /* Step: socket allocates a descriptor; it does not create a TCP connection. */
  6  int main(void) {
  7      int fd = socket(AF_INET, SOCK_STREAM, 0);
  8      if (fd < 0) { perror("socket"); return EXIT_FAILURE; }
  9      /* Step: getsockopt writes both the option value and its actual byte length. */
 10      int type = 0;
 11      socklen_t length = sizeof type;
 12      if (getsockopt(fd, SOL_SOCKET, SO_TYPE, &type, &length) < 0) {
 13          perror("getsockopt"); if (close(fd) < 0) perror("close"); return EXIT_FAILURE;
 14      }
 15      printf("descriptor=%d type=%d (SOCK_STREAM=%d)\n", fd, type, SOCK_STREAM);
 16      /* Step: Each successful socket needs an owner responsible for closing it. */
 17      if (close(fd) < 0) { perror("close"); return EXIT_FAILURE; }
 18      return 0;
 19  }
```

## What if it fails?

Inspect each signed return before treating it as a byte count. Failed acquisition
must not be used as a descriptor. After ownership is acquired, every exit path
must close or explicitly transfer that resource. The source shows whether an
error ends the entire program, only a peer, or one retry attempt.

The descriptor value is not fixed. getsockopt needs output capacity before the call.

## Modify it deliberately

Inspect the type of a SOCK_DGRAM endpoint too.

Refer to the [API reference](../socket-api.md) and
[protocol limits](../protocols-and-limits.md), then run a relevant integration
check and record the observation. Do not claim that merely compiling proves the
protocol works.
