# Nonblocking connect

## Concept and implementation

EINPROGRESS leads to poll, and completion readiness leads to getsockopt(SO_ERROR). A monotonic deadline is recomputed after interruptions. The example accepts numeric IPv4 and avoids making an unsupported claim about DNS deadlines.

Read the [numbered walkthrough](../../docs/walkthroughs/connect_timeout.md) before
modifying the implementation. It links the actual source and explains the
operations and failure paths.

## Build and run

Commands below run from the repository root:

```bash
make build/connect_timeout
./build/connect_timeout 127.0.0.1 9000
```

For an echo server, use the TCP client in another terminal. For the connect demo,
a server must already be running. Programs with no arguments are local
experiments and exit automatically.

## Laboratory

Start an echo server on 9000; connect once successfully, then stop it and observe a refused completion.

Record the exact command, observed return values and explanation. Extend one
variable at a time and retain all descriptor cleanup.

## Limits and checks

Review [protocol limits](../../docs/protocols-and-limits.md). Run the relevant
case in [tests/test_course.py](../../tests/test_course.py), or `make test` for the
complete suite. Tests establish local functional behavior, not production scaling.
