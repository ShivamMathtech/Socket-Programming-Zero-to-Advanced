# poll_server — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../08-io-multiplexing/poll/server.c)

## What does this code do?

Run the poll server implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–3 | Include declarations for the concrete OS/library APIs used below. |
| 4–25 | poll uses an array of descriptor/event pairs; negative descriptors are ignored. |
| 26–39 | HUP can coexist with buffered bytes; read them instead of dropping the connection immediately. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 7 | `net_init` | Install the course SIGPIPE policy so send failures return normally for handling. |
| 7 | `net_listen` | Resolve/bind/listen through the documented helper; the caller owns a successful descriptor. |
| 9 | `nonblocking` | Set O_NONBLOCK while retaining prior status flags. |
| 9 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 21 | `poll` | Wait on requested events and inspect returned revents, including hangup/error conditions. |
| 28 | `connection_step` | Make bounded progress without blocking, keeping unsent output and half-close state. |
| 29 | `connection_close` | Release a peer and reset its slot state for deliberate later reuse. |
| 32 | `accept` | Obtain a connected descriptor distinct from the listener. Set its ownership and mode explicitly. |
| 33 | `connection_add` | Admit one peer into a free bounded slot and set its socket nonblocking. |
| 37 | `connection_close` | Release a peer and reset its slot state for deliberate later reuse. |
| 38 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "reactor.h"
  2  #include <poll.h>
  3  #include <stdio.h>
  4  /* Step: poll uses an array of descriptor/event pairs; negative descriptors are ignored. */
  5  int main(int argc, char **argv) {
  6      if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
  7      net_init(); int listener = net_listen("127.0.0.1", argv[1]);
  8      if (listener < 0) die("listen");
  9      if (nonblocking(listener) < 0) { close_fd(listener); die("nonblocking"); }
 10      static struct connection clients[REACTOR_CLIENTS];
 11      for (int i = 0; i < REACTOR_CLIENTS; ++i) clients[i].fd = -1;
 12      printf("poll echo on %s\n", argv[1]); fflush(stdout);
 13      for (;;) {
 14          struct pollfd events[REACTOR_CLIENTS + 1] = {0};
 15          events[0].fd = listener; events[0].events = POLLIN;
 16          for (int i = 0; i < REACTOR_CLIENTS; ++i) {
 17              events[i + 1].fd = clients[i].fd;
 18              if (wants_read(&clients[i])) events[i + 1].events |= POLLIN;
 19              if (wants_write(&clients[i])) events[i + 1].events |= POLLOUT;
 20          }
 21          int ready = poll(events, REACTOR_CLIENTS + 1, -1);
 22          if (ready < 0 && errno == EINTR) continue;
 23          if (ready < 0) { perror("poll"); break; }
 24          for (int i = 0; i < REACTOR_CLIENTS; ++i) if (clients[i].fd >= 0) {
 25              short flags = events[i + 1].revents;
 26              /* Step: HUP can coexist with buffered bytes; read them instead of dropping the connection immediately. */
 27              if ((flags & (POLLERR | POLLNVAL)) ||
 28                  connection_step(&clients[i], flags & (POLLIN | POLLHUP), flags & POLLOUT) < 0)
 29                  connection_close(&clients[i]);
 30          }
 31          if (events[0].revents & POLLIN) {
 32              int fd = accept(listener, NULL, NULL);
 33              if (fd >= 0) (void)connection_add(clients, fd);
 34              else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) perror("accept");
 35          }
 36      }
 37      for (int i = 0; i < REACTOR_CLIENTS; ++i) connection_close(&clients[i]);
 38      close_fd(listener); return 1;
 39  }
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
