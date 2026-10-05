# IPv6 echo

## Concept and implementation

The listener binds ::1 and explicitly sets IPV6_V6ONLY. The generic client tries getaddrinfo candidates. This is an IPv6-only loopback lesson, not a claim that all operating systems use the same dual-stack defaults.

Read the [numbered walkthrough](../../docs/walkthroughs/ipv6_server.md) before
modifying the implementation. It links the actual source and explains the
operations and failure paths.

## Build and run

Commands below run from the repository root:

```bash
make build/ipv6_server
./build/ipv6_server 9002
```

For an echo server, use the TCP client in another terminal. For the connect demo,
a server must already be running. Programs with no arguments are local
experiments and exit automatically.

## Laboratory

Start ipv6_server 9002 and run ipv6_client ::1 9002 "hello". A disabled IPv6 loopback needs host configuration, not an IPv4 address substitution.

Record the exact command, observed return values and explanation. Extend one
variable at a time and retain all descriptor cleanup.

## Limits and checks

Review [protocol limits](../../docs/protocols-and-limits.md). Run the relevant
case in [tests/test_course.py](../../tests/test_course.py), or `make test` for the
complete suite. Tests establish local functional behavior, not production scaling.
