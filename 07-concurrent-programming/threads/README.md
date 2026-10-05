# Thread lifetime

## Concept and implementation

A worker writes a local variable owned by main. main joins before reading or returning, so the variable remains live and the worker completion is synchronized. POSIX thread errors are returned directly.

Read the [numbered walkthrough](../../docs/walkthroughs/thread_demo.md) before
modifying the implementation. It links the actual source and explains the
operations and failure paths.

## Build and run

Commands below run from the repository root:

```bash
make build/thread_demo
./build/thread_demo 
```

For an echo server, use the TCP client in another terminal. For the connect demo,
a server must already be running. Programs with no arguments are local
experiments and exit automatically.

## Laboratory

Explain which lifetime bug would appear if main returned immediately instead of joining.

Record the exact command, observed return values and explanation. Extend one
variable at a time and retain all descriptor cleanup.

## Limits and checks

Review [protocol limits](../../docs/protocols-and-limits.md). Run the relevant
case in [tests/test_course.py](../../tests/test_course.py), or `make test` for the
complete suite. Tests establish local functional behavior, not production scaling.
