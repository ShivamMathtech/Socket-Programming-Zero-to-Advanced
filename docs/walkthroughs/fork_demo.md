# fork_demo — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../07-concurrent-programming/processes/fork-demo.c)

## What does this code do?

Run the fork demo implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–5 | Include declarations for the concrete OS/library APIs used below. |
| 6–17 | fork copies the process address space; changing a private variable is not IPC. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 9 | `fork` | Create parent/child control paths; both inherit descriptor references and must close unused copies. |
| 13 | `waitpid` | Collect a child completion; WNOHANG permits periodic reaping without blocking the accept loop. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include <errno.h>
  2  #include <stdio.h>
  3  #include <stdlib.h>
  4  #include <sys/wait.h>
  5  #include <unistd.h>
  6  /* Step: fork copies the process address space; changing a private variable is not IPC. */
  7  int main(void) {
  8      int value = 7;
  9      pid_t pid = fork();
 10      if (pid < 0) { perror("fork"); return 1; }
 11      if (pid == 0) { value = 99; printf("child value=%d\n", value); fflush(stdout); _exit(0); }
 12      int status; pid_t result;
 13      do { result = waitpid(pid, &status, 0); } while (result < 0 && errno == EINTR);
 14      if (result < 0) { perror("waitpid"); return 1; }
 15      printf("parent value=%d\n", value);
 16      return WIFEXITED(status) && WEXITSTATUS(status) == 0 ? 0 : 1;
 17  }
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
