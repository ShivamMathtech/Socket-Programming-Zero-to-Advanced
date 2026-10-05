# tcp_server — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../02-tcp-server/server.c)

## What does this code do?

Run the tcp server implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–8 | Include declarations for the concrete OS/library APIs used below. |
| 9–21 | This first server is standalone and deliberately serves one connection. |
| 22–35 | Bind only loopback. htons converts the integer port to network order. |
| 36–44 | accept returns a DIFFERENT descriptor. The listening socket stays open. |
| 45–55 | send may be short, so retain an offset until all bytes are echoed. |
| 56–60 | One cleanup path releases descriptors on success and on every failure. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 14 | `strtol` | Parse numeric CLI text and validate end pointer, errno and range before narrowing. |
| 20 | `socket` | Allocate a descriptor; -1 fails, while zero and positive descriptor numbers are valid. |
| 24 | `setsockopt` | Set an option with the correct level, value type and byte size; inspect failure immediately. |
| 29 | `htons` | Encode a 16-bit port in network byte order. |
| 30 | `htonl` | Encode an unsigned 32-bit value in network byte order. |
| 31 | `bind` | Assign the local endpoint using the provided family-specific address size; failure needs cleanup. |
| 34 | `listen` | Enable a passive stream listener; the backlog is a pending-queue request, not a worker limit. |
| 37 | `accept` | Obtain a connected descriptor distinct from the listener. Set its ownership and mode explicitly. |
| 41 | `recv` | Fill at most the supplied capacity. Check signed return before using it as a length; positive-length TCP zero means EOF. |
| 48 | `send` | Offer this byte range to the kernel. Only a positive returned prefix has progressed; retain any suffix. |
| 57 | `close` | Release the owned descriptor once. This Linux code does not blindly retry close failures. |
| 58 | `close` | Release the owned descriptor once. This Linux code does not blindly retry close failures. |

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
  9  /* Step: This first server is standalone and deliberately serves one connection. */
 10  int main(int argc, char **argv) {
 11      if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
 12      char *end;
 13      errno = 0;
 14      long port = strtol(argv[1], &end, 10);
 15      if (errno || end == argv[1] || *end || port < 1 || port > 65535) {
 16          fprintf(stderr, "PORT must be 1..65535\n"); return 1;
 17      }
 18      if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) { perror("signal"); return 1; }
 19      int listener = -1, peer = -1, status = 1;
 20      listener = socket(AF_INET, SOCK_STREAM, 0);
 21      if (listener < 0) { perror("socket"); goto done; }
 22      /* Step: Bind only loopback. htons converts the integer port to network order. */
 23      int one = 1;
 24      if (setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one) < 0) {
 25          perror("setsockopt"); goto done;
 26      }
 27      struct sockaddr_in address = {0};
 28      address.sin_family = AF_INET;
 29      address.sin_port = htons((uint16_t)port);
 30      address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
 31      if (bind(listener, (struct sockaddr *)&address, sizeof address) < 0) {
 32          perror("bind"); goto done;
 33      }
 34      if (listen(listener, 8) < 0) { perror("listen"); goto done; }
 35      printf("Listening on 127.0.0.1:%ld; one connection\n", port); fflush(stdout);
 36      /* Step: accept returns a DIFFERENT descriptor. The listening socket stays open. */
 37      do { peer = accept(listener, NULL, NULL); } while (peer < 0 && errno == EINTR);
 38      if (peer < 0) { perror("accept"); goto done; }
 39      unsigned char buffer[4096];
 40      for (;;) {
 41          ssize_t received = recv(peer, buffer, sizeof buffer, 0);
 42          if (received < 0 && errno == EINTR) continue;
 43          if (received < 0) { perror("recv"); goto done; }
 44          if (received == 0) break;
 45          /* Step: send may be short, so retain an offset until all bytes are echoed. */
 46          size_t sent = 0;
 47          while (sent < (size_t)received) {
 48              ssize_t n = send(peer, buffer + sent, (size_t)received - sent, 0);
 49              if (n < 0 && errno == EINTR) continue;
 50              if (n <= 0) { perror("send"); goto done; }
 51              sent += (size_t)n;
 52          }
 53      }
 54      status = 0;
 55  done:
 56      /* Step: One cleanup path releases descriptors on success and on every failure. */
 57      if (peer >= 0 && close(peer) < 0) { perror("close peer"); status = 1; }
 58      if (listener >= 0 && close(listener) < 0) { perror("close listener"); status = 1; }
 59      return status;
 60  }
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
