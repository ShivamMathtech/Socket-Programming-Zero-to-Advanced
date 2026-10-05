# common-reactor — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../common/reactor.h)

## What does this code do?

Retain per-peer output while making bounded nonblocking progress.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–6 | Include declarations for the concrete OS/library APIs used below. |
| 7–15 | One bounded output queue per connection prevents a slow reader blocking other peers. |
| 16–30 | The loop performs bounded work per ready descriptor; it never calls blocking send_all. |
| 31–41 | An orderly half-close still requires draining queued replies before close. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 12 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 19 | `send` | Offer this byte range to the kernel. Only a positive returned prefix has progressed; retain any suffix. |
| 22 | `memmove` | Shift the remaining queued suffix safely even though source and destination overlap. |
| 26 | `recv` | Fill at most the supplied capacity. Check signed return before using it as a length; positive-length TCP zero means EOF. |
| 35 | `nonblocking` | Set O_NONBLOCK while retaining prior status flags. |
| 35 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 39 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #ifndef COURSE_REACTOR_H
  2  #define COURSE_REACTOR_H
  3  #include "net.h"
  4  #include <errno.h>
  5  #include <string.h>
  6  #include <sys/socket.h>
  7  /* Step: One bounded output queue per connection prevents a slow reader blocking other peers. */
  8  #define REACTOR_CLIENTS 64
  9  #define REACTOR_BUFFER 16384
 10  struct connection { int fd, eof; size_t used; unsigned char output[REACTOR_BUFFER]; };
 11  static inline void connection_close(struct connection *c) {
 12      close_fd(c->fd); c->fd = -1; c->used = 0; c->eof = 0;
 13  }
 14  static inline int wants_read(const struct connection *c) { return !c->eof && c->used < REACTOR_BUFFER; }
 15  static inline int wants_write(const struct connection *c) { return c->used > 0; }
 16  /* Step: The loop performs bounded work per ready descriptor; it never calls blocking send_all. */
 17  static inline int connection_step(struct connection *c, int readable, int writable) {
 18      if (writable && c->used) {
 19          ssize_t n = send(c->fd, c->output, c->used, 0);
 20          if (n > 0) {
 21              c->used -= (size_t)n;
 22              memmove(c->output, c->output + n, c->used);
 23          } else if (n == 0 || (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR)) return -1;
 24      }
 25      if (readable && wants_read(c)) {
 26          ssize_t n = recv(c->fd, c->output + c->used, REACTOR_BUFFER - c->used, 0);
 27          if (n > 0) c->used += (size_t)n;
 28          else if (n == 0) c->eof = 1;
 29          else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) return -1;
 30      }
 31      /* Step: An orderly half-close still requires draining queued replies before close. */
 32      return c->eof && c->used == 0 ? -1 : 0;
 33  }
 34  static inline int connection_add(struct connection clients[], int fd) {
 35      if (nonblocking(fd) < 0) { close_fd(fd); return -1; }
 36      for (int i = 0; i < REACTOR_CLIENTS; ++i) if (clients[i].fd < 0) {
 37          clients[i].fd = fd; clients[i].eof = 0; clients[i].used = 0; return i;
 38      }
 39      close_fd(fd); return -1;
 40  }
 41  #endif
```

## What if it fails?

Inspect each signed return before treating it as a byte count. Failed acquisition
must not be used as a descriptor. After ownership is acquired, every exit path
must close or explicitly transfer that resource. The source shows whether an
error ends the entire program, only a peer, or one retry attempt.

Each output queue is 16 KiB. A peer EOF closes only after queued output drains. Idle peers have no timeout in this version.

## Modify it deliberately

Add last-progress timestamps and a finite reactor wait budget.

Refer to the [API reference](../socket-api.md) and
[protocol limits](../protocols-and-limits.md), then run a relevant integration
check and record the observation. Do not claim that merely compiling proves the
protocol works.
