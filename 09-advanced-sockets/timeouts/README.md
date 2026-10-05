# Receive timeout

## Concept and implementation

A local connected socketpair isolates timeout behavior. One peer remains open but sends no data; the other times out with EAGAIN/EWOULDBLOCK after an approximately one-second blocking wait. Scheduler timing means it is not an exact stopwatch.

Read the [numbered walkthrough](../../docs/walkthroughs/receive_timeout.md) before
modifying the implementation. It links the actual source and explains the
operations and failure paths.

## Build and run

Commands below run from the repository root:

```bash
make build/receive_timeout
./build/receive_timeout 
```

For an echo server, use the TCP client in another terminal. For the connect demo,
a server must already be running. Programs with no arguments are local
experiments and exit automatically.

## Laboratory

Distinguish the open-but-idle timeout from EOF after closing the other peer.

Record the exact command, observed return values and explanation. Extend one
variable at a time and retain all descriptor cleanup.

## Limits and checks

Review [protocol limits](../../docs/protocols-and-limits.md). Run the relevant
case in [tests/test_course.py](../../tests/test_course.py), or `make test` for the
complete suite. Tests establish local functional behavior, not production scaling.
