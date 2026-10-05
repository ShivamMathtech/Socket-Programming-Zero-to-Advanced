# epoll_server — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../08-io-multiplexing/epoll/server.c)

## What does this code do?

Run the epoll server implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–3 | Include declarations for the concrete OS/library APIs used below. |
| 4–31 | epoll stores interest in the kernel; this lesson uses level triggering. |
| 32–62 | Finish this batch of old client events before assigning a free slot to a new peer. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 9 | `epoll_ctl` | Change registered interest; event data must still identify the same live peer. |
| 10 | `connection_close` | Release a peer and reset its slot state for deliberate later reuse. |
| 14 | `net_init` | Install the course SIGPIPE policy so send failures return normally for handling. |
| 14 | `net_listen` | Resolve/bind/listen through the documented helper; the caller owns a successful descriptor. |
| 16 | `nonblocking` | Set O_NONBLOCK while retaining prior status flags. |
| 16 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 17 | `epoll_create1` | Create a Linux readiness instance; this descriptor also has a cleanup owner. |
| 18 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 20 | `epoll_ctl` | Change registered interest; event data must still identify the same live peer. |
| 21 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 21 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 28 | `epoll_wait` | Retrieve ready entries. Readiness permits progress or an error/EOF result, not a complete protocol message. |
| 38 | `connection_step` | Make bounded progress without blocking, keeping unsent output and half-close state. |
| 42 | `epoll_ctl` | Change registered interest; event data must still identify the same live peer. |
| 47 | `accept` | Obtain a connected descriptor distinct from the listener. Set its ownership and mode explicitly. |
| 52 | `connection_add` | Admit one peer into a free bounded slot and set its socket nonblocking. |
| 55 | `epoll_ctl` | Change registered interest; event data must still identify the same live peer. |
| 56 | `connection_close` | Release a peer and reset its slot state for deliberate later reuse. |
| 60 | `connection_close` | Release a peer and reset its slot state for deliberate later reuse. |
| 61 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 61 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "reactor.h"
  2  #include <stdio.h>
  3  #include <sys/epoll.h>
  4  /* Step: epoll stores interest in the kernel; this lesson uses level triggering. */
  5  static uint32_t interests(struct connection *c) {
  6      return (wants_read(c) ? EPOLLIN : 0U) | (wants_write(c) ? EPOLLOUT : 0U);
  7  }
  8  static void remove_client(int queue, struct connection *c) {
  9      if (epoll_ctl(queue, EPOLL_CTL_DEL, c->fd, NULL) < 0) perror("epoll del");
 10      connection_close(c);
 11  }
 12  int main(int argc, char **argv) {
 13      if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
 14      net_init(); int listener = net_listen("127.0.0.1", argv[1]);
 15      if (listener < 0) die("listen");
 16      if (nonblocking(listener) < 0) { close_fd(listener); die("nonblocking"); }
 17      int queue = epoll_create1(EPOLL_CLOEXEC);
 18      if (queue < 0) { close_fd(listener); die("epoll_create1"); }
 19      struct epoll_event event = {.events = EPOLLIN, .data.u32 = REACTOR_CLIENTS};
 20      if (epoll_ctl(queue, EPOLL_CTL_ADD, listener, &event) < 0) {
 21          close_fd(queue); close_fd(listener); die("epoll add listener");
 22      }
 23      static struct connection clients[REACTOR_CLIENTS];
 24      for (int i = 0; i < REACTOR_CLIENTS; ++i) clients[i].fd = -1;
 25      printf("epoll echo on %s\n", argv[1]); fflush(stdout);
 26      for (;;) {
 27          struct epoll_event ready[REACTOR_CLIENTS + 1];
 28          int count = epoll_wait(queue, ready, REACTOR_CLIENTS + 1, -1);
 29          if (count < 0 && errno == EINTR) continue;
 30          if (count < 0) { perror("epoll_wait"); break; }
 31          int accepting = 0;
 32          /* Step: Finish this batch of old client events before assigning a free slot to a new peer. */
 33          for (int j = 0; j < count; ++j) {
 34              unsigned index = ready[j].data.u32;
 35              if (index == REACTOR_CLIENTS) { accepting = 1; continue; }
 36              struct connection *c = &clients[index];
 37              uint32_t flags = ready[j].events;
 38              if ((flags & EPOLLERR) || connection_step(c, flags & (EPOLLIN | EPOLLHUP), flags & EPOLLOUT) < 0) {
 39                  remove_client(queue, c); continue;
 40              }
 41              event.events = interests(c); event.data.u32 = index;
 42              if (epoll_ctl(queue, EPOLL_CTL_MOD, c->fd, &event) < 0) {
 43                  perror("epoll mod"); remove_client(queue, c);
 44              }
 45          }
 46          if (accepting) {
 47              int fd = accept(listener, NULL, NULL);
 48              if (fd < 0) {
 49                  if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) perror("accept");
 50                  continue;
 51              }
 52              int index = connection_add(clients, fd);
 53              if (index < 0) continue;
 54              event.events = EPOLLIN; event.data.u32 = (unsigned)index;
 55              if (epoll_ctl(queue, EPOLL_CTL_ADD, fd, &event) < 0) {
 56                  perror("epoll add"); connection_close(&clients[index]);
 57              }
 58          }
 59      }
 60      for (int i = 0; i < REACTOR_CLIENTS; ++i) connection_close(&clients[i]);
 61      close_fd(queue); close_fd(listener); return 1;
 62  }
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
