# fork_server — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../06-multiple-client-server/fork-server/server.c)

## What does this code do?

Run the fork server implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–6 | Include declarations for the concrete OS/library APIs used below. |
| 7–15 | The parent owns the listener; each child owns one connected descriptor. |
| 16–32 | Reap exited children regularly, including when no new clients arrive. |
| 33–44 | Close unused inherited descriptors in BOTH processes to avoid leaked references. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 10 | `net_init` | Install the course SIGPIPE policy so send failures return normally for handling. |
| 11 | `net_listen` | Resolve/bind/listen through the documented helper; the caller owns a successful descriptor. |
| 19 | `waitpid` | Collect a child completion; WNOHANG permits periodic reaping without blocking the accept loop. |
| 24 | `poll` | Wait on requested events and inspect returned revents, including hangup/error conditions. |
| 28 | `accept_retry` | Retry an interrupted accept while retaining the listener. |
| 30 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 31 | `fork` | Create parent/child control paths; both inherit descriptor references and must close unused copies. |
| 32 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 35 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 36 | `set_timeout` | Set per-operation blocking I/O timeouts; this is not a whole-transaction deadline. |
| 37 | `echo_connection` | Loop over binary receive prefixes and fully echo each one until EOF/failure. |
| 39 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 41 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 43 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <errno.h>
  3  #include <poll.h>
  4  #include <stdio.h>
  5  #include <sys/wait.h>
  6  #include <unistd.h>
  7  /* Step: The parent owns the listener; each child owns one connected descriptor. */
  8  int main(int argc, char **argv) {
  9      if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
 10      net_init();
 11      int listener = net_listen("127.0.0.1", argv[1]);
 12      if (listener < 0) die("listen");
 13      int active = 0;
 14      printf("Fork echo on %s; maximum 64 children\n", argv[1]); fflush(stdout);
 15      for (;;) {
 16          /* Step: Reap exited children regularly, including when no new clients arrive. */
 17          pid_t reaped;
 18          do {
 19              reaped = waitpid(-1, NULL, WNOHANG);
 20              if (reaped > 0) --active;
 21          } while (reaped > 0 || (reaped < 0 && errno == EINTR));
 22          if (reaped < 0 && errno != ECHILD) { perror("waitpid"); break; }
 23          struct pollfd item = {.fd = listener, .events = POLLIN};
 24          int ready = poll(&item, 1, 200);
 25          if (ready < 0 && errno == EINTR) continue;
 26          if (ready < 0) { perror("poll"); break; }
 27          if (!ready) continue;
 28          int peer = accept_retry(listener);
 29          if (peer < 0) { perror("accept"); break; }
 30          if (active >= 64) { close_fd(peer); continue; }
 31          pid_t child = fork();
 32          if (child < 0) { perror("fork"); close_fd(peer); continue; }
 33          /* Step: Close unused inherited descriptors in BOTH processes to avoid leaked references. */
 34          if (child == 0) {
 35              close_fd(listener);
 36              int result = set_timeout(peer, 15);
 37              if (result == 0) result = echo_connection(peer);
 38              if (result < 0) perror("child echo");
 39              close_fd(peer); _exit(result < 0 ? 1 : 0);
 40          }
 41          ++active; close_fd(peer);
 42      }
 43      close_fd(listener); return 1;
 44  }
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
