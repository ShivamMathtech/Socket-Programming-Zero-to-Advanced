# common-chat — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../common/chat.c)

## What does this code do?

Multiplex one terminal and one connected socket.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–7 | Include declarations for the concrete OS/library APIs used below. |
| 8–23 | Watch keyboard and socket together so either participant can speak first. |
| 24–37 | Ctrl-D stops sending but leaves the receive path alive. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 11 | `set_timeout` | Set per-operation blocking I/O timeouts; this is not a whole-transaction deadline. |
| 15 | `poll` | Wait on requested events and inspect returned revents, including hangup/error conditions. |
| 21 | `read` | Read available local file/terminal bytes; handle EOF/error before interpreting a count. |
| 26 | `shutdown` | Change communication direction without releasing the descriptor; SHUT_WR preserves receiving. |
| 27 | `send_all` | Complete a blocking byte send by retaining offsets, or fail the exchange. |
| 30 | `recv` | Fill at most the supplied capacity. Check signed return before using it as a length; positive-length TCP zero means EOF. |
| 34 | `fwrite` | Write an explicit byte count instead of assuming NUL-terminated network text. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "chat.h"
  2  #include "net.h"
  3  #include <errno.h>
  4  #include <poll.h>
  5  #include <stdio.h>
  6  #include <sys/socket.h>
  7  #include <unistd.h>
  8  /* Step: Watch keyboard and socket together so either participant can speak first. */
  9  int chat_session(int fd) {
 10      int input_open = 1;
 11      if (set_timeout(fd, 15) < 0) return -1;
 12      for (;;) {
 13          struct pollfd items[2] = {{.fd = input_open ? STDIN_FILENO : -1, .events = POLLIN},
 14                                   {.fd = fd, .events = POLLIN}};
 15          int ready = poll(items, 2, -1);
 16          if (ready < 0 && errno == EINTR) continue;
 17          if (ready < 0) return -1;
 18          if ((items[0].revents | items[1].revents) & (POLLERR | POLLNVAL)) { errno = EIO; return -1; }
 19          unsigned char buffer[4096];
 20          if (items[0].revents & (POLLIN | POLLHUP)) {
 21              ssize_t n = read(STDIN_FILENO, buffer, sizeof buffer);
 22              if (n < 0 && errno != EINTR) return -1;
 23              if (n == 0) {
 24                  /* Step: Ctrl-D stops sending but leaves the receive path alive. */
 25                  input_open = 0;
 26                  if (shutdown(fd, SHUT_WR) < 0) return -1;
 27              } else if (n > 0 && send_all(fd, buffer, (size_t)n) < 0) return -1;
 28          }
 29          if (items[1].revents & (POLLIN | POLLHUP)) {
 30              ssize_t n = recv(fd, buffer, sizeof buffer, 0);
 31              if (n < 0 && errno == EINTR) continue;
 32              if (n < 0) return -1;
 33              if (n == 0) return 0;
 34              if (fwrite(buffer, 1, (size_t)n, stdout) != (size_t)n || fflush(stdout) == EOF) return -1;
 35          }
 36      }
 37  }
```

## What if it fails?

Inspect each signed return before treating it as a byte count. Failed acquisition
must not be used as a descriptor. After ownership is acquired, every exit path
must close or explicitly transfer that resource. The source shows whether an
error ends the entire program, only a peer, or one retry attempt.

Writes remain blocking with an operation timeout, so this is a two-party teaching chat. EOF semantics are explicitly symmetric session termination.

## Modify it deliberately

Convert the output path to a queue before using this pattern for many clients.

Refer to the [API reference](../socket-api.md) and
[protocol limits](../protocols-and-limits.md), then run a relevant integration
check and record the observation. Do not claim that merely compiling proves the
protocol works.
