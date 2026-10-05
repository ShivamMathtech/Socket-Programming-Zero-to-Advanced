# select_server — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../08-io-multiplexing/select/server.c)

## What does this code do?

Run the select server implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–4 | Include declarations for the concrete OS/library APIs used below. |
| 5–26 | select takes disposable bit sets; rebuild them before every wait. |
| 27–42 | Process existing peers before accepting, so descriptor reuse cannot consume stale readiness. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 8 | `net_init` | Install the course SIGPIPE policy so send failures return normally for handling. |
| 9 | `net_listen` | Resolve/bind/listen through the documented helper; the caller owns a successful descriptor. |
| 11 | `nonblocking` | Set O_NONBLOCK while retaining prior status flags. |
| 11 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 24 | `select` | Wait with disposable bit sets; highest descriptor plus one bounds the scan. |
| 30 | `connection_step` | Make bounded progress without blocking, keeping unsent output and half-close state. |
| 31 | `connection_close` | Release a peer and reset its slot state for deliberate later reuse. |
| 34 | `accept` | Obtain a connected descriptor distinct from the listener. Set its ownership and mode explicitly. |
| 35 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 36 | `connection_add` | Admit one peer into a free bounded slot and set its socket nonblocking. |
| 40 | `connection_close` | Release a peer and reset its slot state for deliberate later reuse. |
| 41 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "reactor.h"
  2  #include <stdio.h>
  3  #include <stdlib.h>
  4  #include <sys/select.h>
  5  /* Step: select takes disposable bit sets; rebuild them before every wait. */
  6  int main(int argc, char **argv) {
  7      if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
  8      net_init();
  9      int listener = net_listen("127.0.0.1", argv[1]);
 10      if (listener < 0) die("listen");
 11      if (listener >= FD_SETSIZE || nonblocking(listener) < 0) { close_fd(listener); return 1; }
 12      static struct connection clients[REACTOR_CLIENTS];
 13      for (int i = 0; i < REACTOR_CLIENTS; ++i) clients[i].fd = -1;
 14      printf("select echo on %s\n", argv[1]); fflush(stdout);
 15      for (;;) {
 16          fd_set reads, writes; FD_ZERO(&reads); FD_ZERO(&writes);
 17          FD_SET(listener, &reads); int largest = listener;
 18          for (int i = 0; i < REACTOR_CLIENTS; ++i) if (clients[i].fd >= 0) {
 19              int fd = clients[i].fd;
 20              if (wants_read(&clients[i])) FD_SET(fd, &reads);
 21              if (wants_write(&clients[i])) FD_SET(fd, &writes);
 22              if (fd > largest) largest = fd;
 23          }
 24          int ready = select(largest + 1, &reads, &writes, NULL, NULL);
 25          if (ready < 0 && errno == EINTR) continue;
 26          if (ready < 0) { perror("select"); break; }
 27          /* Step: Process existing peers before accepting, so descriptor reuse cannot consume stale readiness. */
 28          for (int i = 0; i < REACTOR_CLIENTS; ++i) if (clients[i].fd >= 0) {
 29              int fd = clients[i].fd;
 30              if (connection_step(&clients[i], FD_ISSET(fd, &reads), FD_ISSET(fd, &writes)) < 0)
 31                  connection_close(&clients[i]);
 32          }
 33          if (FD_ISSET(listener, &reads)) {
 34              int fd = accept(listener, NULL, NULL);
 35              if (fd >= FD_SETSIZE) close_fd(fd);
 36              else if (fd >= 0) (void)connection_add(clients, fd);
 37              else if (errno != EAGAIN && errno != EWOULDBLOCK && errno != EINTR) perror("accept");
 38          }
 39      }
 40      for (int i = 0; i < REACTOR_CLIENTS; ++i) connection_close(&clients[i]);
 41      close_fd(listener); return 1;
 42  }
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
