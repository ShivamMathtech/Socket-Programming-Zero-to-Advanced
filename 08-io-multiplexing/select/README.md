# select readiness

## Concept and implementation

Read/write sets are reconstructed each iteration. nfds is largest descriptor plus one. Descriptor numbers at or above FD_SETSIZE are rejected before FD_SET. The shared reactor retains partial output and drains it after incoming EOF.

Read the [numbered walkthrough](../../docs/walkthroughs/select_server.md) before
modifying the implementation. It links the actual source and explains the
operations and failure paths.

## Build and run

Commands below run from the repository root:

```bash
make build/select_server
./build/select_server 9000
```

For an echo server, use the TCP client in another terminal. For the connect demo,
a server must already be running. Programs with no arguments are local
experiments and exit automatically.

## Laboratory

Trace a half-close while output remains queued; identify why write interest must remain active.

Record the exact command, observed return values and explanation. Extend one
variable at a time and retain all descriptor cleanup.

## Limits and checks

Review [protocol limits](../../docs/protocols-and-limits.md). Run the relevant
case in [tests/test_course.py](../../tests/test_course.py), or `make test` for the
complete suite. Tests establish local functional behavior, not production scaling.
