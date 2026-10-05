# poll readiness

## Concept and implementation

The pollfd array contains one listener entry plus fixed client slots. Negative fd entries are ignored. revents is distinct from requested events. POLLHUP can coexist with unread bytes, so the handler attempts progress before discarding a peer.

Read the [numbered walkthrough](../../docs/walkthroughs/poll_server.md) before
modifying the implementation. It links the actual source and explains the
operations and failure paths.

## Build and run

Commands below run from the repository root:

```bash
make build/poll_server
./build/poll_server 9000
```

For an echo server, use the TCP client in another terminal. For the connect demo,
a server must already be running. Programs with no arguments are local
experiments and exit automatically.

## Laboratory

Compare requested events and returned revents; show why a permanent POLLOUT request could cause busy looping.

Record the exact command, observed return values and explanation. Extend one
variable at a time and retain all descriptor cleanup.

## Limits and checks

Review [protocol limits](../../docs/protocols-and-limits.md). Run the relevant
case in [tests/test_course.py](../../tests/test_course.py), or `make test` for the
complete suite. Tests establish local functional behavior, not production scaling.
