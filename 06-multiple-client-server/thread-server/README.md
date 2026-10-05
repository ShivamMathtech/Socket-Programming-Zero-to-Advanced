# Thread echo

## Concept and implementation

Workers are detached. A heap argument gives each worker its own descriptor value; active-worker bookkeeping is mutex-protected. The accept loop releases the mutex before network I/O or worker creation. Threads share the descriptor table.

Read the [numbered walkthrough](../../docs/walkthroughs/thread_server.md) before
modifying the implementation. It links the actual source and explains the
operations and failure paths.

## Build and run

Commands below run from the repository root:

```bash
make build/thread_server
./build/thread_server 9000
```

For an echo server, use the TCP client in another terminal. For the connect demo,
a server must already be running. Programs with no arguments are local
experiments and exit automatically.

## Laboratory

Explain the difference between freeing the argument allocation and closing its contained socket descriptor.

Record the exact command, observed return values and explanation. Extend one
variable at a time and retain all descriptor cleanup.

## Limits and checks

Review [protocol limits](../../docs/protocols-and-limits.md). Run the relevant
case in [tests/test_course.py](../../tests/test_course.py), or `make test` for the
complete suite. Tests establish local functional behavior, not production scaling.
