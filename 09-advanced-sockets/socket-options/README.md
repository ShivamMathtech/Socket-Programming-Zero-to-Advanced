# Socket options

## Concept and implementation

The program sets and reads SO_KEEPALIVE, then reports SO_RCVBUF. The receive-buffer value is host dependent. Setting a keepalive flag does not prove the peer application is responsive, nor does it replace an application request deadline.

Read the [numbered walkthrough](../../docs/walkthroughs/socket_options.md) before
modifying the implementation. It links the actual source and explains the
operations and failure paths.

## Build and run

Commands below run from the repository root:

```bash
make build/socket_options
./build/socket_options 
```

For an echo server, use the TCP client in another terminal. For the connect demo,
a server must already be running. Programs with no arguments are local
experiments and exit automatically.

## Laboratory

Read effective values after changing one option and report the OS along with observations.

Record the exact command, observed return values and explanation. Extend one
variable at a time and retain all descriptor cleanup.

## Limits and checks

Review [protocol limits](../../docs/protocols-and-limits.md). Run the relevant
case in [tests/test_course.py](../../tests/test_course.py), or `make test` for the
complete suite. Tests establish local functional behavior, not production scaling.
