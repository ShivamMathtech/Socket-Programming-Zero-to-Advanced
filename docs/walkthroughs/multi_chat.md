# multi_chat — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../11-real-world-projects/multi-client-chat/server.c)

## What does this code do?

Run the multi chat implementation in its chapter or project.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–6 | Include declarations for the concrete OS/library APIs used below. |
| 7–13 | The wire protocol is newline-delimited, with at most 512 bytes before each newline. |
| 14–57 | Queue broadcasts separately for each receiver; disconnect a receiver whose queue overflows. |
| 58–79 | Parse all complete lines, retaining a split line across recv calls. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 13 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 17 | `snprintf` | Format into bounded storage; a result at least the capacity means truncation. |
| 20 | `memcpy` | Copy an exact byte count; valid capacities and nonoverlapping ranges are required. |
| 25 | `memcpy` | Copy an exact byte count; valid capacities and nonoverlapping ranges are required. |
| 30 | `net_init` | Install the course SIGPIPE policy so send failures return normally for handling. |
| 30 | `net_listen` | Resolve/bind/listen through the documented helper; the caller owns a successful descriptor. |
| 32 | `nonblocking` | Set O_NONBLOCK while retaining prior status flags. |
| 32 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 42 | `poll` | Wait on requested events and inspect returned revents, including hangup/error conditions. |
| 50 | `send` | Offer this byte range to the kernel. Only a positive returned prefix has progressed; retain any suffix. |
| 51 | `memmove` | Shift the remaining queued suffix safely even though source and destination overlap. |
| 55 | `recv` | Fill at most the supplied capacity. Check signed return before using it as a length; positive-length TCP zero means EOF. |
| 68 | `accept` | Obtain a connected descriptor distinct from the listener. Set its ownership and mode explicitly. |
| 70 | `nonblocking` | Set O_NONBLOCK while retaining prior status flags. |
| 70 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 73 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |
| 78 | `close_fd` | Close a valid descriptor once; negative unallocated descriptors are ignored. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include "net.h"
  2  #include <errno.h>
  3  #include <poll.h>
  4  #include <stdio.h>
  5  #include <string.h>
  6  #include <sys/socket.h>
  7  /* Step: The wire protocol is newline-delimited, with at most 512 bytes before each newline. */
  8  #define PEERS 32
  9  #define LINE 512
 10  #define QUEUE 16384
 11  struct peer { int fd, eof; unsigned id; size_t input_size, output_size; char input[LINE], output[QUEUE]; };
 12  static struct peer peers[PEERS];
 13  static void drop(struct peer *p) { close_fd(p->fd); p->fd = -1; p->input_size = p->output_size = 0; p->eof = 0; }
 14  /* Step: Queue broadcasts separately for each receiver; disconnect a receiver whose queue overflows. */
 15  static void broadcast(struct peer *sender) {
 16      char line[LINE + 64];
 17      int prefix = snprintf(line, sizeof line, "peer#%u: ", sender->id);
 18      if (prefix < 0 || (size_t)prefix >= sizeof line) return;
 19      size_t size = (size_t)prefix;
 20      memcpy(line + size, sender->input, sender->input_size); size += sender->input_size;
 21      line[size++] = '\n';
 22      for (int i = 0; i < PEERS; ++i) if (peers[i].fd >= 0) {
 23          struct peer *target = &peers[i];
 24          if (size > QUEUE - target->output_size) { drop(target); continue; }
 25          memcpy(target->output + target->output_size, line, size); target->output_size += size;
 26      }
 27  }
 28  int main(int argc, char **argv) {
 29      if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
 30      net_init(); int listener = net_listen("127.0.0.1", argv[1]);
 31      if (listener < 0) die("listen");
 32      if (nonblocking(listener) < 0) { close_fd(listener); die("nonblocking"); }
 33      for (int i = 0; i < PEERS; ++i) peers[i].fd = -1;
 34      unsigned next_id = 1;
 35      printf("Multi-chat on %s (32 peers, 512-byte lines)\n", argv[1]); fflush(stdout);
 36      for (;;) {
 37          struct pollfd events[PEERS + 1] = {0}; events[0].fd = listener; events[0].events = POLLIN;
 38          for (int i = 0; i < PEERS; ++i) {
 39              events[i + 1].fd = peers[i].fd;
 40              events[i + 1].events = (peers[i].eof ? 0 : POLLIN) | (peers[i].output_size ? POLLOUT : 0);
 41          }
 42          int ready = poll(events, PEERS + 1, -1);
 43          if (ready < 0 && errno == EINTR) continue;
 44          if (ready < 0) { perror("poll"); break; }
 45          for (int i = 0; i < PEERS; ++i) {
 46              struct peer *p = &peers[i]; short flags = events[i + 1].revents;
 47              if (p->fd < 0) continue;
 48              if (flags & (POLLERR | POLLNVAL)) { drop(p); continue; }
 49              if ((flags & POLLOUT) && p->output_size) {
 50                  ssize_t n = send(p->fd, p->output, p->output_size, 0);
 51                  if (n > 0) { p->output_size -= (size_t)n; memmove(p->output, p->output + n, p->output_size); }
 52                  else if (n == 0 || (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK)) { drop(p); continue; }
 53              }
 54              if (!p->eof && (flags & (POLLIN | POLLHUP))) {
 55                  char bytes[1024]; ssize_t n = recv(p->fd, bytes, sizeof bytes, 0);
 56                  if (n == 0) p->eof = 1;
 57                  else if (n < 0 && errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) { drop(p); continue; }
 58                  /* Step: Parse all complete lines, retaining a split line across recv calls. */
 59                  for (ssize_t j = 0; j < n && p->fd >= 0; ++j) {
 60                      if (bytes[j] == '\n') { broadcast(p); p->input_size = 0; }
 61                      else if (p->input_size == LINE) { drop(p); break; }
 62                      else p->input[p->input_size++] = bytes[j];
 63                  }
 64              }
 65              if (p->fd >= 0 && p->eof && !p->output_size) drop(p);
 66          }
 67          if (events[0].revents & POLLIN) {
 68              int fd = accept(listener, NULL, NULL);
 69              if (fd < 0) { if (errno != EINTR && errno != EAGAIN && errno != EWOULDBLOCK) perror("accept"); continue; }
 70              if (nonblocking(fd) < 0) { perror("nonblocking"); close_fd(fd); continue; }
 71              int slot;
 72              for (slot = 0; slot < PEERS; ++slot) if (peers[slot].fd < 0) break;
 73              if (slot == PEERS) close_fd(fd);
 74              else { peers[slot].fd = fd; peers[slot].id = next_id++; }
 75          }
 76      }
 77      for (int i = 0; i < PEERS; ++i) drop(&peers[i]);
 78      close_fd(listener); return 1;
 79  }
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
