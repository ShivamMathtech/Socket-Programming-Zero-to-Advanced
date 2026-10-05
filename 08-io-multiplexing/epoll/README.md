# Linux epoll readiness

## Concept and implementation

epoll_create1 makes an owned interest-queue descriptor. Clients are ADDed, interest is MODified as output changes, and DEL occurs before closing. The example uses level triggering and processes an old event batch before assigning newly accepted peers to freed slots.

Read the [numbered walkthrough](../../docs/walkthroughs/epoll_server.md) before
modifying the implementation. It links the actual source and explains the
operations and failure paths.

## Build and run

Commands below run from the repository root:

```bash
make build/epoll_server
./build/epoll_server 9000
```

For an echo server, use the TCP client in another terminal. For the connect demo,
a server must already be running. Programs with no arguments are local
experiments and exit automatically.

## Laboratory

Explain why adding EPOLLET without changing the bounded-progress design can stall a connection.

Record the exact command, observed return values and explanation. Extend one
variable at a time and retain all descriptor cleanup.

## Limits and checks

Review [protocol limits](../../docs/protocols-and-limits.md). Run the relevant
case in [tests/test_course.py](../../tests/test_course.py), or `make test` for the
complete suite. Tests establish local functional behavior, not production scaling.
