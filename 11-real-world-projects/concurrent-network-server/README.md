# Project 7 — Concurrent Network Server

[All projects](../README.md) · [Course home](../../README.md)

## Problem Statement

Compare process, thread and readiness architectures on the same local echo workload.

## Requirements

Use the canonical echo variants; verify every reply; bound client load; report measurements from the actual run with setup and limitations.

Prerequisites: Chapters 00–05 for echo/datagrams, 06–09 for concurrency and queue
management. Review the matching chapter before modifying project code.

## Architecture

The same raw-byte echo protocol is implemented by fork_server, thread_server, poll_server and epoll_server. The Python load driver is a test client, not the server implementation.

## Workflow

1. Start the receiving/listening endpoint with the documented port.
2. Start the peer or client with the matching endpoint.
3. Exchange the protocol's bounded records or byte stream.
4. Observe completion and close semantics rather than assuming one read is a message.
5. Exercise the documented failure cases and record the result.

## Implementation

- [server.c](../../06-multiple-client-server/fork-server/server.c)
- [server.c](../../06-multiple-client-server/thread-server/server.c)
- [server.c](../../08-io-multiplexing/poll/server.c)
- [server.c](../../08-io-multiplexing/epoll/server.c)
- [load_test.py](load_test.py)

Canonical chapter implementations are reused where linked above; each project
Makefile builds those real sources. Shared helper contracts are documented in
[common/README.md](../../common/README.md). No generated executable is required in
the ZIP: build for your own Linux environment.

## Compilation

From the repository root:

```bash
make build/fork_server build/thread_server build/poll_server build/epoll_server
```

Alternatively, from this project directory run `make`; its Makefile calls the root
build and outputs executables into the root `build/` directory.

## Execution

Run these from the repository root. Terminal labels indicate separate shells.

```bash
# Terminal A: choose one server at a time
./build/thread_server 9000
# Terminal B
python3 11-real-world-projects/concurrent-network-server/load_test.py --port 9000 --clients 8
python3 11-real-world-projects/concurrent-network-server/load_test.py --port 9000 --clients 32
```

**Expected behavior:** The driver reports how many echoes matched and a measured local mean roundtrip. It explicitly labels this as a local exercise rather than a production throughput benchmark.

## Testing

```bash
python3 tests/test_course.py CourseTests.test_04_concurrent_echo_and_idle_peer CourseTests.test_05_event_loop_slow_reader
```

Hold a client idle and verify other clients progress. Keep payload/protocol and host constant when changing the server model. Tiny loopback tests include scheduling effects and cannot justify broad scalability claims.

## Possible Improvements

Add repeated trials, latency percentiles, CPU/memory observation and a mix of idle/active connections. First retain correctness and overload checks.

## Completion rubric

- Explain every socket's owner and who closes it.
- State what separates messages or signals stream completion.
- Demonstrate a normal case and an input/disconnect boundary.
- Explain one implementation limit without claiming an unimplemented feature.
- Save your own commands and observed results in the [lab notebook](../../docs/lab-notebook.md).
