# Process memory

## Concept and implementation

The child modifies value=99 while the parent later prints its unchanged value=7. waitpid synchronizes child completion and reaps its exit status. No shared-memory mapping is used.

Read the [numbered walkthrough](../../docs/walkthroughs/fork_demo.md) before
modifying the implementation. It links the actual source and explains the
operations and failure paths.

## Build and run

Commands below run from the repository root:

```bash
make build/fork_demo
./build/fork_demo 
```

For an echo server, use the TCP client in another terminal. For the connect demo,
a server must already be running. Programs with no arguments are local
experiments and exit automatically.

## Laboratory

Replace the numeric value and predict both lines; output order here is constrained by waiting for the child.

Record the exact command, observed return values and explanation. Extend one
variable at a time and retain all descriptor cleanup.

## Limits and checks

Review [protocol limits](../../docs/protocols-and-limits.md). Run the relevant
case in [tests/test_course.py](../../tests/test_course.py), or `make test` for the
complete suite. Tests establish local functional behavior, not production scaling.
