# Mutex-protected counter

## Concept and implementation

Four workers each perform 100000 protected increments. Every conflicting access follows one mutex policy, and main reads after joins. The deterministic total is 400000; thread scheduling order is not specified.

Read the [numbered walkthrough](../../docs/walkthroughs/counter.md) before
modifying the implementation. It links the actual source and explains the
operations and failure paths.

## Build and run

Commands below run from the repository root:

```bash
make build/counter
./build/counter 
```

For an echo server, use the TCP client in another terminal. For the connect demo,
a server must already be running. Programs with no arguments are local
experiments and exit automatically.

## Laboratory

Propose a private-per-thread accumulation design and compare its synchronization requirements.

Record the exact command, observed return values and explanation. Extend one
variable at a time and retain all descriptor cleanup.

## Limits and checks

Review [protocol limits](../../docs/protocols-and-limits.md). Run the relevant
case in [tests/test_course.py](../../tests/test_course.py), or `make test` for the
complete suite. Tests establish local functional behavior, not production scaling.
