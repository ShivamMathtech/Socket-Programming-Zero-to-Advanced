# Project 1 — TCP Echo Server

[All projects](../README.md) · [Course home](../../README.md)

## Problem Statement

Verify a client/server byte path by returning exactly the bytes received, including binary data.

## Requirements

Bind loopback; accept sequential clients; echo every positive receive prefix; handle partial writes and peer EOF; close failed peers.

Prerequisites: Chapters 00–05 for echo/datagrams, 06–09 for concurrency and queue
management. Review the matching chapter before modifying project code.

## Architecture

The listener owns admission. A single connected peer is passed to echo_connection. The handler returns on EOF/error, and the accept loop continues.

## Workflow

1. Start the receiving/listening endpoint with the documented port.
2. Start the peer or client with the matching endpoint.
3. Exchange the protocol's bounded records or byte stream.
4. Observe completion and close semantics rather than assuming one read is a message.
5. Exercise the documented failure cases and record the result.

## Implementation

- [server.c](../../06-multiple-client-server/iterative-server/server.c)
- [client.c](../../03-tcp-client/client.c)
- [net.c](../../common/net.c)

Canonical chapter implementations are reused where linked above; each project
Makefile builds those real sources. Shared helper contracts are documented in
[common/README.md](../../common/README.md). No generated executable is required in
the ZIP: build for your own Linux environment.

## Compilation

From the repository root:

```bash
make build/iterative_server build/tcp_client
```

Alternatively, from this project directory run `make`; its Makefile calls the root
build and outputs executables into the root `build/` directory.

## Execution

Run these from the repository root. Terminal labels indicate separate shells.

```bash
# Terminal A
./build/iterative_server 9000
# Terminal B
./build/tcp_client 127.0.0.1 9000 "echo project"
```

**Expected behavior:** The client prints echo project. The iterative server keeps running for subsequent connections.

## Testing

```bash
python3 tests/test_course.py CourseTests.test_03_raw_echo_variants
```

Test repeated sessions, embedded NUL bytes and a client write-half-close. An idle peer delays other clients until it closes or the per-operation timeout fails.

## Possible Improvements

Add a byte counter, then implement a framed transform service. Preserve the distinction between stream bytes and message records.

## Completion rubric

- Explain every socket's owner and who closes it.
- State what separates messages or signals stream completion.
- Demonstrate a normal case and an input/disconnect boundary.
- Explain one implementation limit without claiming an unimplemented feature.
- Save your own commands and observed results in the [lab notebook](../../docs/lab-notebook.md).
