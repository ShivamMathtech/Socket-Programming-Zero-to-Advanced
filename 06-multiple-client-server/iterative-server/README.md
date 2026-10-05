# Iterative echo

## Concept and implementation

The accept loop handles a connection until it closes or a 15-second receive/send operation timeout fails. Only then is the next client served. A completed TCP handshake does not imply that the application is already reading that peer.

Read the [numbered walkthrough](../../docs/walkthroughs/iterative_server.md) before
modifying the implementation. It links the actual source and explains the
operations and failure paths.

## Build and run

Commands below run from the repository root:

```bash
make build/iterative_server
./build/iterative_server 9000
```

For an echo server, use the TCP client in another terminal. For the connect demo,
a server must already be running. Programs with no arguments are local
experiments and exit automatically.

## Laboratory

Keep one client idle and try a second. Observe delayed service; close the first and observe progress.

Record the exact command, observed return values and explanation. Extend one
variable at a time and retain all descriptor cleanup.

## Limits and checks

Review [protocol limits](../../docs/protocols-and-limits.md). Run the relevant
case in [tests/test_course.py](../../tests/test_course.py), or `make test` for the
complete suite. Tests establish local functional behavior, not production scaling.
