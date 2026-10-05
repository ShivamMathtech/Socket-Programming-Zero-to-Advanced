# Shared helpers — read these before using them

The first TCP server/client are standalone. Later programs reuse these small
helpers so application protocols stay visible instead of copying identical error
loops. This folder uses direct POSIX APIs and contains no networking framework.

| File | Responsibility | Read alongside |
|---|---|---|
| [net.h](net.h) / [net.c](net.c) | Endpoint resolution, bounded framing, blocking I/O | [net walkthrough](../docs/walkthroughs/common-net.md) |
| [reactor.h](reactor.h) | One bounded output queue and nonblocking progress per peer | [reactor walkthrough](../docs/walkthroughs/common-reactor.md) |
| [chat.h](chat.h) / [chat.c](chat.c) | Watch stdin and socket for a two-party terminal chat | [chat walkthrough](../docs/walkthroughs/common-chat.md) |

## Helper contracts

| Function | Input / output contract |
|---|---|
| net_init | Ignores SIGPIPE; errors become ordinary send return values |
| close_fd | Close nonnegative Linux descriptor once; log failure without retry |
| valid_port | Accept numeric 1..65535; reject named services and malformed input |
| net_bound | Try getaddrinfo candidates until bind works; free the list |
| net_listen | Bind then listen with backlog 64; close if listen fails |
| net_connect | Try candidate sockets with blocking connect; return connected fd |
| nonblocking | Read status flags; set existing flags OR O_NONBLOCK |
| set_timeout | Set both receive/send per-operation socket timeouts |
| send_all | Blocking loop until all bytes are accepted or a failure occurs |
| recv_exact | 1 complete, 0 EOF before requested bytes, -1 error/truncation |
| frame_send | Send 4-byte big-endian length and body; cap at 1 MiB |
| frame_recv | Validate length before body; distinguish empty record from EOF |
| echo_connection | Echo arbitrary positive recv prefixes until stream EOF |
| accept_retry | Retry an accept interrupted by a signal |
| connection_step | Make bounded nonblocking read/write progress and retain queued bytes |

`recv_exact(..., 0)` returns 1 because a zero-length request is already satisfied.
`frame_recv` translates EOF after a body length was announced into a truncation
error. Never use a blocking send_all loop as the output path for a nonblocking
reactor: preserve the unsent suffix and wait for later write readiness instead.

The common endpoint helper binds IPv6 sockets with IPV6_V6ONLY explicitly. Its
blocking getaddrinfo/connect calls do not share an overall deadline. Error errno
can be overwritten by later library calls, so the implementation saves it while
closing a failed candidate.

`chat_session` waits on stdin and socket with poll, but its bounded message writes
are still blocking with a 15-second operation timeout. It is a two-party teaching
chat, not the nonblocking multi-client server. Read [limits](../docs/protocols-and-limits.md)
before reusing helpers in a larger application.
