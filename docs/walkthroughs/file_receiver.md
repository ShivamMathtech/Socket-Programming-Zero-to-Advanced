# file_receiver — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../11-real-world-projects/file-transfer/receiver.c)

## What does this code do?

Run the file receiver implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–6 | Include declarations for the concrete OS/library APIs used below. |
| 7–22 | O_EXCL refuses overwriting an existing file, including a pre-existing symbolic link. |
| 23–33 | Bound total disk usage at 64 MiB and loop over short file writes too. |
| 34–41 | Confirm only after all bytes are flushed and the file descriptor closes successfully. |
| 42–45 | A partial transfer is removed; a complete file survives an acknowledgement failure. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 10 | `open` | Create/open a local path with the explicit policy flags; O_EXCL prevents overwriting an existing destination. |
| 12 | `net_init` | Install the course SIGPIPE policy so send failures return normally for handling. |
| 12 | `net_listen` | Resolve/bind/listen through the documented helper; the caller owns a successful descriptor. |
| 16 | `accept_retry` | Retry an interrupted accept while retaining the listener. |
| 17 | `set_timeout` | Set per-operation blocking I/O timeouts; this is not a whole-transaction deadline. |
| 20 | `frame_recv` | Validate the length before filling the caller body buffer; report frame, EOF or error distinctly. |
| 27 | `write` | Write a prefix to the file descriptor and advance by the returned count. |
| 35 | `fsync` | Request persistence of completed output before sending the file acknowledgement; errors prevent acknowledgement. |
| 36 | `close` | Release the owned descriptor once. This Linux code does not blindly retry close failures. |
| 38 | `send_all` | Complete a blocking byte send by retaining offsets, or fail the exchange. |
| 41 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 41 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 41 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 43 | `unlink` | Remove the partial file on a handled transfer failure; a committed file is retained. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <errno.h>
  3  #include <fcntl.h>
  4  #include <stdio.h>
  5  #include <sys/socket.h>
  6  #include <unistd.h>
  7  /* Step: O_EXCL refuses overwriting an existing file, including a pre-existing symbolic link. */
  8  int main(int argc, char **argv) {
  9      if (argc != 3) { fprintf(stderr, "usage: %s PORT NEW_OUTPUT_FILE\n", argv[0]); return 1; }
 10      int output = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, 0600);
 11      if (output < 0) die("create new output");
 12      net_init(); int listener = net_listen("127.0.0.1", argv[1]), peer = -1, status = 1;
 13      unsigned long long total = 0; int committed = 0;
 14      if (listener < 0) { perror("listen"); goto done; }
 15      printf("File receiver on %s; one connection\n", argv[1]); fflush(stdout);
 16      peer = accept_retry(listener);
 17      if (peer < 0 || set_timeout(peer, 15) < 0) { perror("accept/timeout"); goto done; }
 18      for (;;) {
 19          unsigned char data[32768]; uint32_t n;
 20          int result = frame_recv(peer, data, sizeof data, &n);
 21          if (result != 1) { fprintf(stderr, "truncated or invalid transfer\n"); goto done; }
 22          if (n == 0) break;
 23          /* Step: Bound total disk usage at 64 MiB and loop over short file writes too. */
 24          if (total + n > 64ULL * 1024 * 1024) { fprintf(stderr, "64 MiB limit exceeded\n"); goto done; }
 25          size_t written = 0;
 26          while (written < n) {
 27              ssize_t count = write(output, data + written, n - written);
 28              if (count < 0 && errno == EINTR) continue;
 29              if (count <= 0) { perror("write file"); goto done; }
 30              written += (size_t)count;
 31          }
 32          total += n;
 33      }
 34      /* Step: Confirm only after all bytes are flushed and the file descriptor closes successfully. */
 35      if (fsync(output) < 0) { perror("fsync"); goto done; }
 36      if (close(output) < 0) { output = -1; perror("close output"); goto done; }
 37      output = -1; committed = 1;
 38      if (send_all(peer, "OK", 2) < 0) { perror("acknowledgement"); goto done; }
 39      printf("Received %llu bytes\n", total); status = 0;
 40  done:
 41      close_fd(output); close_fd(peer); close_fd(listener);
 42      /* Step: A partial transfer is removed; a complete file survives an acknowledgement failure. */
 43      if (!committed && unlink(argv[2]) < 0) perror("remove partial file");
 44      return status;
 45  }
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
