# counter — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../07-concurrent-programming/synchronization/counter.c)

## What does this code do?

Run the counter implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–4 | Include declarations for the concrete OS/library APIs used below. |
| 5–25 | Increment is read-modify-write, so all threads must hold the same mutex. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 12 | `pthread_mutex_lock` | Acquire the agreed mutex before accessing shared mutable state. |
| 14 | `pthread_mutex_unlock` | Release the mutex so other workers can enter the critical section. |
| 20 | `pthread_create` | Start a worker with stable argument storage; its nonzero return is an error number directly. |
| 21 | `pthread_join` | Wait for a joinable worker, keeping its argument/result storage alive until completion. |
| 23 | `pthread_mutex_destroy` | Destroy a mutex only after users have finished. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include <pthread.h>
  2  #include <stdio.h>
  3  #include <stdlib.h>
  4  #include <string.h>
  5  /* Step: Increment is read-modify-write, so all threads must hold the same mutex. */
  6  static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
  7  static unsigned counter;
  8  static void check(int code) { if (code) { fprintf(stderr, "%s\n", strerror(code)); exit(1); } }
  9  static void *increment(void *unused) {
 10      (void)unused;
 11      for (unsigned i = 0; i < 100000; ++i) {
 12          check(pthread_mutex_lock(&lock));
 13          ++counter;
 14          check(pthread_mutex_unlock(&lock));
 15      }
 16      return NULL;
 17  }
 18  int main(void) {
 19      pthread_t workers[4];
 20      for (int i = 0; i < 4; ++i) check(pthread_create(&workers[i], NULL, increment, NULL));
 21      for (int i = 0; i < 4; ++i) check(pthread_join(workers[i], NULL));
 22      printf("counter=%u expected=400000\n", counter);
 23      check(pthread_mutex_destroy(&lock));
 24      return counter == 400000 ? 0 : 1;
 25  }
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
