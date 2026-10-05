# Broken pipe

## Concept and implementation

The program ignores SIGPIPE, closes one end of a local pair and checks EPIPE from sending on the other. This isolates a Linux error path; real TCP may report a reset or broken pipe depending on timing and state.

Read the [numbered walkthrough](../../docs/walkthroughs/broken_pipe.md) before
modifying the implementation. It links the actual source and explains the
operations and failure paths.

## Build and run

Commands below run from the repository root:

```bash
make build/broken_pipe
./build/broken_pipe 
```

For an echo server, use the TCP client in another terminal. For the connect demo,
a server must already be running. Programs with no arguments are local
experiments and exit automatically.

## Laboratory

Explain why blindly retrying an EPIPE send cannot recreate a healthy connection.

Record the exact command, observed return values and explanation. Extend one
variable at a time and retain all descriptor cleanup.

## Limits and checks

Review [protocol limits](../../docs/protocols-and-limits.md). Run the relevant
case in [tests/test_course.py](../../tests/test_course.py), or `make test` for the
complete suite. Tests establish local functional behavior, not production scaling.
