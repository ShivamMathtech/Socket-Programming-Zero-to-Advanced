# connect_timeout — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../09-advanced-sockets/blocking-vs-nonblocking/connect.c)

## What does this code do?

Run the connect timeout implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–9 | Include declarations for the concrete OS/library APIs used below. |
| 10–23 | A monotonic deadline survives wall-clock changes and repeated signal interruptions. |
| 24–35 | EINPROGRESS means establishment is pending, not failed and not yet successful. |
| 36–46 | Writable can mean failure. SO_ERROR is the authoritative completion result. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 13 | `clock_gettime` | Read monotonic elapsed time for an overall deadline unaffected by wall-clock adjustments. |
| 19 | `htons` | Encode a 16-bit port in network byte order. |
| 19 | `strtoul` | Parse a validated unsigned numeric port; subsequent conversion must fit its defined range. |
| 20 | `inet_pton` | Convert numeric address text; return 1 means valid conversion, not merely nonnegative. |
| 21 | `socket` | Allocate a descriptor; -1 fails, while zero and positive descriptor numbers are valid. |
| 23 | `nonblocking` | Set O_NONBLOCK while retaining prior status flags. |
| 25 | `connect` | Attempt peer association/establishment. Nonblocking TCP may report EINPROGRESS instead of completing. |
| 32 | `poll` | Wait on requested events and inspect returned revents, including hangup/error conditions. |
| 38 | `getsockopt` | Read an option into caller storage; length is capacity on entry and result size afterward. |
| 45 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <arpa/inet.h>
  3  #include <errno.h>
  4  #include <limits.h>
  5  #include <poll.h>
  6  #include <stdio.h>
  7  #include <stdlib.h>
  8  #include <sys/socket.h>
  9  #include <time.h>
 10  /* Step: A monotonic deadline survives wall-clock changes and repeated signal interruptions. */
 11  static long long milliseconds(void) {
 12      struct timespec now;
 13      if (clock_gettime(CLOCK_MONOTONIC, &now) < 0) die("clock_gettime");
 14      return (long long)now.tv_sec * 1000 + now.tv_nsec / 1000000;
 15  }
 16  int main(int argc, char **argv) {
 17      if (argc != 3 || !valid_port(argv[2])) { fprintf(stderr, "usage: %s IPv4 PORT\n", argv[0]); return 1; }
 18      struct sockaddr_in address = {0}; address.sin_family = AF_INET;
 19      address.sin_port = htons((uint16_t)strtoul(argv[2], NULL, 10));
 20      if (inet_pton(AF_INET, argv[1], &address.sin_addr) != 1) return 1;
 21      int fd = socket(AF_INET, SOCK_STREAM, 0), status = 1;
 22      if (fd < 0) die("socket");
 23      if (nonblocking(fd) < 0) { perror("fcntl"); goto done; }
 24      /* Step: EINPROGRESS means establishment is pending, not failed and not yet successful. */
 25      if (connect(fd, (struct sockaddr *)&address, sizeof address) < 0) {
 26          if (errno != EINPROGRESS) { perror("connect"); goto done; }
 27          long long deadline = milliseconds() + 3000;
 28          struct pollfd item = {.fd = fd, .events = POLLOUT};
 29          for (;;) {
 30              long long remaining = deadline - milliseconds();
 31              if (remaining <= 0) { fprintf(stderr, "connect deadline exceeded\n"); goto done; }
 32              int ready = poll(&item, 1, (int)remaining);
 33              if (ready < 0 && errno == EINTR) continue;
 34              if (ready < 0) { perror("poll"); goto done; }
 35              if (ready == 0) continue;
 36              /* Step: Writable can mean failure. SO_ERROR is the authoritative completion result. */
 37              int error = 0; socklen_t size = sizeof error;
 38              if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &size) < 0) { perror("SO_ERROR"); goto done; }
 39              if (error) { errno = error; perror("connect completion"); goto done; }
 40              break;
 41          }
 42      }
 43      puts("Connected before the 3 second deadline"); status = 0;
 44  done:
 45      close_fd(fd); return status;
 46  }
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
