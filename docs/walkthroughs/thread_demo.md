# thread_demo — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../07-concurrent-programming/threads/thread-demo.c)

## What does this code do?

Run the thread demo implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–3 | Include declarations for the concrete OS/library APIs used below. |
| 4–14 | join establishes completion before main reads the worker's result. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 9 | `pthread_create` | Start a worker with stable argument storage; its nonzero return is an error number directly. |
| 11 | `pthread_join` | Wait for a joinable worker, keeping its argument/result storage alive until completion. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include <pthread.h>
  2  #include <stdio.h>
  3  #include <string.h>
  4  /* Step: join establishes completion before main reads the worker's result. */
  5  static void *worker(void *argument) { *(int *)argument = 99; return NULL; }
  6  int main(void) {
  7      int value = 7;
  8      pthread_t thread;
  9      int code = pthread_create(&thread, NULL, worker, &value);
 10      if (code) { fprintf(stderr, "create: %s\n", strerror(code)); return 1; }
 11      code = pthread_join(thread, NULL);
 12      if (code) { fprintf(stderr, "join: %s\n", strerror(code)); return 1; }
 13      printf("shared value=%d\n", value); return 0;
 14  }
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
