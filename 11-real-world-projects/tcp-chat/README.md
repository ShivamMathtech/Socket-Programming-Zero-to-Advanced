# Project 2 — TCP Chat Application

[All projects](../README.md) · [Course home](../../README.md)

## Problem Statement

Let two terminal users exchange messages without waiting for an enforced request/reply turn.

## Requirements

One TCP connection; watch stdin and socket; print received bytes; Ctrl-D half-closes outgoing data; report errors.

Prerequisites: Chapters 00–05 for echo/datagrams, 06–09 for concurrency and queue
management. Review the matching chapter before modifying project code.

## Architecture

The server accepts one peer. Both ends then use chat_session, whose poll watches keyboard and network. A terminal line becomes raw bytes, not a special TCP record.

## Workflow

1. Start the receiving/listening endpoint with the documented port.
2. Start the peer or client with the matching endpoint.
3. Exchange the protocol's bounded records or byte stream.
4. Observe completion and close semantics rather than assuming one read is a message.
5. Exercise the documented failure cases and record the result.

## Implementation

- [server.c](server.c)
- [client.c](client.c)
- [chat.c](../../common/chat.c)

Canonical chapter implementations are reused where linked above; each project
Makefile builds those real sources. Shared helper contracts are documented in
[common/README.md](../../common/README.md). No generated executable is required in
the ZIP: build for your own Linux environment.

## Compilation

From the repository root:

```bash
make build/tcp_chat_server build/tcp_chat_client
```

Alternatively, from this project directory run `make`; its Makefile calls the root
build and outputs executables into the root `build/` directory.

## Execution

Run these from the repository root. Terminal labels indicate separate shells.

```bash
# Terminal A
./build/tcp_chat_server 9000
# Terminal B
./build/tcp_chat_client 127.0.0.1 9000
```

**Expected behavior:** Type a line in either terminal. It appears at the other endpoint. Ctrl-D stops local input; peer EOF ends the session.

## Testing

```bash
python3 tests/test_course.py CourseTests.test_12_tcp_chat_server_duplex CourseTests.test_13_tcp_chat_client
```

Test each direction independently. The chat write helper is blocking with a 15-second operation timeout; one slow write can delay keyboard servicing in this two-party example.

## Possible Improvements

Add an explicit /quit record using a framed protocol, then introduce names and a clear escaping rule instead of silently interpreting arbitrary text as commands.

## Completion rubric

- Explain every socket's owner and who closes it.
- State what separates messages or signals stream completion.
- Demonstrate a normal case and an input/disconnect boundary.
- Explain one implementation limit without claiming an unimplemented feature.
- Save your own commands and observed results in the [lab notebook](../../docs/lab-notebook.md).
