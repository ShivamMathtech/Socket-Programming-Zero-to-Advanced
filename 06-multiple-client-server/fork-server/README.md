# Fork echo

## Concept and implementation

Each accepted connection has a child process. Parent and child close opposite unused descriptors. The parent polls the listener with a 200 ms timeout so it can reap exited children even when no new client arrives. It refuses new workers at 64 active children.

Read the [numbered walkthrough](../../docs/walkthroughs/fork_server.md) before
modifying the implementation. It links the actual source and explains the
operations and failure paths.

## Build and run

Commands below run from the repository root:

```bash
make build/fork_server
./build/fork_server 9000
```

For an echo server, use the TCP client in another terminal. For the connect demo,
a server must already be running. Programs with no arguments are local
experiments and exit automatically.

## Laboratory

Connect multiple peers, then close them. Confirm children are reaped rather than accumulating as zombies.

Record the exact command, observed return values and explanation. Extend one
variable at a time and retain all descriptor cleanup.

## Limits and checks

Review [protocol limits](../../docs/protocols-and-limits.md). Run the relevant
case in [tests/test_course.py](../../tests/test_course.py), or `make test` for the
complete suite. Tests establish local functional behavior, not production scaling.
