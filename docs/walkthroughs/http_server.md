# http_server — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../11-real-world-projects/simple-http-server/server.c)

## What does this code do?

Run the http server implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–5 | Include declarations for the concrete OS/library APIs used below. |
| 6–18 | This is a deliberately small HTTP subset: GET and HEAD, two fixed routes, close after response. |
| 19–38 | TCP can split headers anywhere. Accumulate until CRLF CRLF, with an 8 KiB limit. |
| 39–56 | No URL maps to a filesystem path. Unknown paths always return 404. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 8 | `strlen` | Measure trusted/local NUL-terminated text only; never infer binary payload length this way. |
| 9 | `snprintf` | Format into bounded storage; a result at least the capacity means truncation. |
| 14 | `send_all` | Complete a blocking byte send by retaining offsets, or fail the exchange. |
| 15 | `send_all` | Complete a blocking byte send by retaining offsets, or fail the exchange. |
| 21 | `recv` | Fill at most the supplied capacity. Check signed return before using it as a length; positive-length TCP zero means EOF. |
| 25 | `memchr` | Search only the specified received bytes, without assuming a terminator. |
| 27 | `strstr` | Search the explicitly terminated accumulated HTTP buffer for a protocol delimiter. |
| 29 | `strstr` | Search the explicitly terminated accumulated HTTP buffer for a protocol delimiter. |
| 30 | `strstr` | Search the explicitly terminated accumulated HTTP buffer for a protocol delimiter. |
| 34 | `sscanf` | Parse bounded request-line fields; width limits protect arrays and the extra conversion detects trailing tokens. |
| 46 | `net_init` | Install the course SIGPIPE policy so send failures return normally for handling. |
| 46 | `net_listen` | Resolve/bind/listen through the documented helper; the caller owns a successful descriptor. |
| 50 | `accept_retry` | Retry an interrupted accept while retaining the listener. |
| 52 | `set_timeout` | Set per-operation blocking I/O timeouts; this is not a whole-transaction deadline. |
| 53 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 55 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <errno.h>
  3  #include <stdio.h>
  4  #include <string.h>
  5  #include <sys/socket.h>
  6  /* Step: This is a deliberately small HTTP subset: GET and HEAD, two fixed routes, close after response. */
  7  static int response(int fd, int status, const char *reason, const char *body, int head) {
  8      char headers[512]; size_t length = strlen(body);
  9      int n = snprintf(headers, sizeof headers,
 10          "HTTP/1.1 %d %s\r\nContent-Type: text/plain; charset=utf-8\r\n"
 11          "Content-Length: %zu\r\nConnection: close\r\nX-Content-Type-Options: nosniff\r\n%s\r\n",
 12          status, reason, length, status == 405 ? "Allow: GET, HEAD\r\n" : "");
 13      if (n < 0 || (size_t)n >= sizeof headers) { errno = EOVERFLOW; return -1; }
 14      if (send_all(fd, headers, (size_t)n) < 0) return -1;
 15      return head ? 0 : send_all(fd, body, length);
 16  }
 17  static int serve(int fd) {
 18      char request[8193]; size_t used = 0;
 19      /* Step: TCP can split headers anywhere. Accumulate until CRLF CRLF, with an 8 KiB limit. */
 20      while (used < sizeof request - 1) {
 21          ssize_t n = recv(fd, request + used, sizeof request - 1 - used, 0);
 22          if (n < 0 && errno == EINTR) continue;
 23          if (n < 0) return -1;
 24          if (n == 0) return used ? response(fd,400,"Bad Request","Incomplete request\n",0) : 0;
 25          if (memchr(request + used, '\0', (size_t)n)) return response(fd,400,"Bad Request","Invalid NUL byte\n",0);
 26          used += (size_t)n; request[used] = '\0';
 27          if (strstr(request, "\r\n\r\n")) break;
 28      }
 29      if (!strstr(request, "\r\n\r\n")) return response(fd,431,"Request Header Fields Too Large","Header limit exceeded\n",0);
 30      char *end = strstr(request, "\r\n");
 31      if (!end) return response(fd,400,"Bad Request","Missing request line\n",0);
 32      *end = '\0';
 33      char method[16], path[1024], version[16], extra;
 34      if (sscanf(request, "%15s %1023s %15s %c", method, path, version, &extra) != 3 ||
 35          (strcmp(version,"HTTP/1.0") && strcmp(version,"HTTP/1.1")))
 36          return response(fd,400,"Bad Request","Bad request line\n",0);
 37      int head = !strcmp(method,"HEAD");
 38      if (strcmp(method,"GET") && !head) return response(fd,405,"Method Not Allowed","Use GET or HEAD\n",0);
 39      /* Step: No URL maps to a filesystem path. Unknown paths always return 404. */
 40      if (!strcmp(path,"/")) return response(fd,200,"OK","Socket Programming Lab\n",head);
 41      if (!strcmp(path,"/health")) return response(fd,200,"OK","ok\n",head);
 42      return response(fd,404,"Not Found","Not found\n",head);
 43  }
 44  int main(int argc, char **argv) {
 45      if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
 46      net_init(); int listener = net_listen("127.0.0.1",argv[1]);
 47      if (listener < 0) die("listen");
 48      printf("HTTP lab: http://127.0.0.1:%s\n",argv[1]); fflush(stdout);
 49      for (;;) {
 50          int peer = accept_retry(listener);
 51          if (peer < 0) { perror("accept"); break; }
 52          if (set_timeout(peer,3) < 0 || serve(peer) < 0) perror("HTTP client");
 53          close_fd(peer);
 54      }
 55      close_fd(listener); return 1;
 56  }
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
