# Project 4 — Multi-Client Chat Server

[All projects](../README.md) · [Course home](../../README.md)

## Problem Statement

Broadcast complete text lines to all connected participants while isolating slow readers.

## Requirements

32 clients maximum; 512 input bytes before newline; 16 KiB output queue per peer; preserve split/coalesced lines; drop overflowed peers.

Prerequisites: Chapters 00–05 for echo/datagrams, 06–09 for concurrency and queue
management. Review the matching chapter before modifying project code.

## Architecture

A poll loop owns every socket. Each peer stores input parsing state and an output queue. Complete lines become peer#N-prefixed broadcasts. Each recipient advances its own send offset.

## Workflow

1. Start the receiving/listening endpoint with the documented port.
2. Start the peer or client with the matching endpoint.
3. Exchange the protocol's bounded records or byte stream.
4. Observe completion and close semantics rather than assuming one read is a message.
5. Exercise the documented failure cases and record the result.

## Implementation

- [server.c](server.c)
- [net.c](../../common/net.c)
- [client.c](../tcp-chat/client.c)

Canonical chapter implementations are reused where linked above; each project
Makefile builds those real sources. Shared helper contracts are documented in
[common/README.md](../../common/README.md). No generated executable is required in
the ZIP: build for your own Linux environment.

## Compilation

From the repository root:

```bash
make build/multi_chat build/tcp_chat_client
```

Alternatively, from this project directory run `make`; its Makefile calls the root
build and outputs executables into the root `build/` directory.

## Execution

Run these from the repository root. Terminal labels indicate separate shells.

```bash
# Terminal A
./build/multi_chat 9000
# Terminal B
./build/tcp_chat_client 127.0.0.1 9000
# Terminal C
./build/tcp_chat_client 127.0.0.1 9000
```

**Expected behavior:** A line typed by a participant appears to every participant, including the sender, prefixed with a server-assigned peer number.

## Testing

```bash
python3 tests/test_course.py CourseTests.test_15_multi_chat_split_lines CourseTests.test_16_multi_chat_oversize_line
```

Split a line across writes, coalesce two lines, then send 513 bytes without newline. The oversized peer closes. An unfinished line at EOF is discarded; queued complete output is drained. Peer numbers are ephemeral, not authenticated identities.

## Possible Improvements

Add authenticated identity through a maintained secure transport/authentication design, persistent rooms or idle deadlines. Keep output limits explicit; do not add unbounded broadcast queues.

## Completion rubric

- Explain every socket's owner and who closes it.
- State what separates messages or signals stream completion.
- Demonstrate a normal case and an input/disconnect boundary.
- Explain one implementation limit without claiming an unimplemented feature.
- Save your own commands and observed results in the [lab notebook](../../docs/lab-notebook.md).
