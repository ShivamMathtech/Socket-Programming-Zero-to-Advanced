# tcp_client — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../03-tcp-client/client.c)

## What does this code do?

Run the tcp client implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–8 | Include declarations for the concrete OS/library APIs used below. |
| 9–24 | The first client accepts a numeric IPv4 address and one short text message. |
| 25–33 | connect triggers active TCP establishment; the OS normally selects a local port. |
| 34–49 | Half-close signals end of request while keeping the receive direction open. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 13 | `strtol` | Parse numeric CLI text and validate end pointer, errno and range before narrowing. |
| 14 | `strlen` | Measure trusted/local NUL-terminated text only; never infer binary payload length this way. |
| 19 | `htons` | Encode a 16-bit port in network byte order. |
| 20 | `inet_pton` | Convert numeric address text; return 1 means valid conversion, not merely nonnegative. |
| 23 | `socket` | Allocate a descriptor; -1 fails, while zero and positive descriptor numbers are valid. |
| 26 | `connect` | Attempt peer association/establishment. Nonblocking TCP may report EINPROGRESS instead of completing. |
| 27 | `strlen` | Measure trusted/local NUL-terminated text only; never infer binary payload length this way. |
| 29 | `send` | Offer this byte range to the kernel. Only a positive returned prefix has progressed; retain any suffix. |
| 35 | `shutdown` | Change communication direction without releasing the descriptor; SHUT_WR preserves receiving. |
| 38 | `recv` | Fill at most the supplied capacity. Check signed return before using it as a length; positive-length TCP zero means EOF. |
| 42 | `fwrite` | Write an explicit byte count instead of assuming NUL-terminated network text. |
| 47 | `close` | Release the owned descriptor once. This Linux code does not blindly retry close failures. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include <arpa/inet.h>
  2  #include <errno.h>
  3  #include <signal.h>
  4  #include <stdio.h>
  5  #include <stdlib.h>
  6  #include <string.h>
  7  #include <sys/socket.h>
  8  #include <unistd.h>
  9  /* Step: The first client accepts a numeric IPv4 address and one short text message. */
 10  int main(int argc, char **argv) {
 11      if (argc != 4) { fprintf(stderr, "usage: %s IPv4 PORT MESSAGE\n", argv[0]); return 1; }
 12      char *end; errno = 0;
 13      long port = strtol(argv[2], &end, 10);
 14      if (errno || end == argv[2] || *end || port < 1 || port > 65535 || strlen(argv[3]) > 4096) {
 15          fprintf(stderr, "bad port or message longer than 4096 bytes\n"); return 1;
 16      }
 17      if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) { perror("signal"); return 1; }
 18      struct sockaddr_in address = {0};
 19      address.sin_family = AF_INET; address.sin_port = htons((uint16_t)port);
 20      if (inet_pton(AF_INET, argv[1], &address.sin_addr) != 1) {
 21          fprintf(stderr, "numeric IPv4 address required\n"); return 1;
 22      }
 23      int fd = socket(AF_INET, SOCK_STREAM, 0), status = 1;
 24      if (fd < 0) { perror("socket"); return 1; }
 25      /* Step: connect triggers active TCP establishment; the OS normally selects a local port. */
 26      if (connect(fd, (struct sockaddr *)&address, sizeof address) < 0) { perror("connect"); goto done; }
 27      size_t length = strlen(argv[3]), sent = 0;
 28      while (sent < length) {
 29          ssize_t n = send(fd, argv[3] + sent, length - sent, 0);
 30          if (n < 0 && errno == EINTR) continue;
 31          if (n <= 0) { perror("send"); goto done; }
 32          sent += (size_t)n;
 33      }
 34      /* Step: Half-close signals end of request while keeping the receive direction open. */
 35      if (shutdown(fd, SHUT_WR) < 0) { perror("shutdown"); goto done; }
 36      unsigned char buffer[4096];
 37      for (;;) {
 38          ssize_t n = recv(fd, buffer, sizeof buffer, 0);
 39          if (n < 0 && errno == EINTR) continue;
 40          if (n < 0) { perror("recv"); goto done; }
 41          if (n == 0) break;
 42          if (fwrite(buffer, 1, (size_t)n, stdout) != (size_t)n) { perror("stdout"); goto done; }
 43      }
 44      if (putchar('\n') == EOF) goto done;
 45      status = 0;
 46  done:
 47      if (close(fd) < 0) { perror("close"); status = 1; }
 48      return status;
 49  }
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
