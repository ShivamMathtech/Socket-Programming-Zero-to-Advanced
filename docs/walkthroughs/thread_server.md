# thread_server — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../06-multiple-client-server/thread-server/server.c)

## What does this code do?

Run the thread server implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–5 | Include declarations for the concrete OS/library APIs used below. |
| 6–20 | Threads share descriptors and memory. Protect only shared metadata with the mutex. |
| 21–38 | Detached workers release thread resources automatically when they return. |
| 39–53 | Heap storage prevents the classic address-of-loop-variable race. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 13 | `free` | Release heap storage once it is no longer needed. |
| 14 | `set_timeout` | Set per-operation blocking I/O timeouts; this is not a whole-transaction deadline. |
| 14 | `echo_connection` | Loop over binary receive prefixes and fully echo each one until EOF/failure. |
| 15 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 16 | `pthread_mutex_lock` | Acquire the agreed mutex before accessing shared mutable state. |
| 18 | `pthread_mutex_unlock` | Release the mutex so other workers can enter the critical section. |
| 24 | `net_init` | Install the course SIGPIPE policy so send failures return normally for handling. |
| 25 | `net_listen` | Resolve/bind/listen through the documented helper; the caller owns a successful descriptor. |
| 28 | `pthread_attr_init` | Initialize thread attribute storage before configuring worker lifecycle. |
| 29 | `pthread_attr_setdetachstate` | Make worker resources self-releasing on completion; detached threads are not joined. |
| 32 | `accept_retry` | Retry an interrupted accept while retaining the listener. |
| 34 | `pthread_mutex_lock` | Acquire the agreed mutex before accessing shared mutable state. |
| 37 | `pthread_mutex_unlock` | Release the mutex so other workers can enter the critical section. |
| 38 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 40 | `malloc` | Allocate storage whose lifetime survives this loop iteration; transfer or release ownership explicitly. |
| 43 | `pthread_create` | Start a worker with stable argument storage; its nonzero return is an error number directly. |
| 46 | `free` | Release heap storage once it is no longer needed. |
| 46 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 47 | `pthread_mutex_lock` | Acquire the agreed mutex before accessing shared mutable state. |
| 48 | `pthread_mutex_unlock` | Release the mutex so other workers can enter the critical section. |
| 51 | `pthread_attr_destroy` | Release attribute-object resources after it is no longer needed. |
| 52 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <pthread.h>
  3  #include <stdio.h>
  4  #include <stdlib.h>
  5  #include <string.h>
  6  /* Step: Threads share descriptors and memory. Protect only shared metadata with the mutex. */
  7  static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  8  static unsigned active;
  9  static void check(int code, const char *what) {
 10      if (code) { fprintf(stderr, "%s: %s\n", what, strerror(code)); exit(1); }
 11  }
 12  static void *serve(void *argument) {
 13      int fd = *(int *)argument; free(argument);
 14      if (set_timeout(fd, 15) < 0 || echo_connection(fd) < 0) perror("thread echo");
 15      close_fd(fd);
 16      check(pthread_mutex_lock(&lock), "lock");
 17      --active;
 18      check(pthread_mutex_unlock(&lock), "unlock");
 19      return NULL;
 20  }
 21  /* Step: Detached workers release thread resources automatically when they return. */
 22  int main(int argc, char **argv) {
 23      if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
 24      net_init();
 25      int listener = net_listen("127.0.0.1", argv[1]);
 26      if (listener < 0) die("listen");
 27      pthread_attr_t attributes;
 28      check(pthread_attr_init(&attributes), "attr init");
 29      check(pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED), "detach state");
 30      printf("Thread echo on %s; maximum 64 workers\n", argv[1]); fflush(stdout);
 31      for (;;) {
 32          int peer = accept_retry(listener);
 33          if (peer < 0) { perror("accept"); break; }
 34          check(pthread_mutex_lock(&lock), "lock");
 35          int full = active >= 64;
 36          if (!full) ++active;
 37          check(pthread_mutex_unlock(&lock), "unlock");
 38          if (full) { close_fd(peer); continue; }
 39          /* Step: Heap storage prevents the classic address-of-loop-variable race. */
 40          int *argument = malloc(sizeof *argument);
 41          int code = 0;
 42          pthread_t worker;
 43          if (argument) { *argument = peer; code = pthread_create(&worker, &attributes, serve, argument); }
 44          if (!argument || code) {
 45              fprintf(stderr, "worker allocation/create failed%s%s\n", code ? ": " : "", code ? strerror(code) : "");
 46              free(argument); close_fd(peer);
 47              check(pthread_mutex_lock(&lock), "lock"); --active;
 48              check(pthread_mutex_unlock(&lock), "unlock");
 49          }
 50      }
 51      check(pthread_attr_destroy(&attributes), "attr destroy");
 52      close_fd(listener); return 1;
 53  }
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
