# file_sender — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../11-real-world-projects/file-transfer/sender.c)

## What does this code do?

Run the file sender implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–4 | Include declarations for the concrete OS/library APIs used below. |
| 5–18 | The sender never transmits a filename, so the peer cannot choose an output path. |
| 19–29 | A zero-length frame marks a complete file; socket EOF alone is not success. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 8 | `fopen` | Open a local input in binary mode and check for failure. |
| 9 | `net_init` | Install the course SIGPIPE policy so send failures return normally for handling. |
| 9 | `net_connect` | Resolve and try blocking connection candidates; success returns an owned descriptor. |
| 10 | `fclose` | Close the FILE stream and inspect flush/close failure where relevant. |
| 12 | `set_timeout` | Set per-operation blocking I/O timeouts; this is not a whole-transaction deadline. |
| 14 | `fread` | Read binary file bytes into a bounded chunk; inspect ferror when a short final read occurs. |
| 15 | `frame_send` | Serialize a bounded four-byte length and body; an empty body still has a header. |
| 20 | `frame_send` | Serialize a bounded four-byte length and body; an empty body still has a header. |
| 22 | `recv_exact` | Read a defined byte count while distinguishing boundary EOF from truncation. |
| 27 | `fclose` | Close the FILE stream and inspect flush/close failure where relevant. |
| 28 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <stdio.h>
  3  #include <string.h>
  4  #include <sys/socket.h>
  5  /* Step: The sender never transmits a filename, so the peer cannot choose an output path. */
  6  int main(int argc, char **argv) {
  7      if (argc != 4) { fprintf(stderr, "usage: %s HOST PORT INPUT_FILE\n", argv[0]); return 1; }
  8      FILE *input = fopen(argv[3], "rb"); if (!input) die("input file");
  9      net_init(); int fd = net_connect(argv[1], argv[2], SOCK_STREAM);
 10      if (fd < 0) { fclose(input); die("connect"); }
 11      int status = 1; unsigned char data[32768]; unsigned long long total = 0;
 12      if (set_timeout(fd, 15) < 0) { perror("timeout"); goto done; }
 13      for (;;) {
 14          size_t n = fread(data, 1, sizeof data, input);
 15          if (n && frame_send(fd, data, (uint32_t)n) < 0) { perror("send chunk"); goto done; }
 16          total += n;
 17          if (n < sizeof data) { if (ferror(input)) { perror("read file"); goto done; } break; }
 18      }
 19      /* Step: A zero-length frame marks a complete file; socket EOF alone is not success. */
 20      if (frame_send(fd, data, 0) < 0) { perror("end marker"); goto done; }
 21      char ack[2];
 22      if (recv_exact(fd, ack, sizeof ack) != 1 || memcmp(ack, "OK", 2)) {
 23          fprintf(stderr, "receiver did not confirm complete file\n"); goto done;
 24      }
 25      printf("Transferred %llu bytes; receiver acknowledged\n", total); status = 0;
 26  done:
 27      if (fclose(input) != 0) { perror("fclose"); status = 1; }
 28      close_fd(fd); return status;
 29  }
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
