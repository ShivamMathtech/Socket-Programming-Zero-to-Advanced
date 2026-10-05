# Project 6 — Simple HTTP Server

[All projects](../README.md) · [Course home](../../README.md)

## Problem Statement

Translate a small textual request into a well-framed HTTP response using raw sockets.

## Requirements

GET and HEAD; fixed / and /health routes; 404/405/400/431 responses; bounded 8 KiB request headers; one response per connection.

Prerequisites: Chapters 00–05 for echo/datagrams, 06–09 for concurrency and queue
management. Review the matching chapter before modifying project code.

## Architecture

The server accumulates bytes until CRLF CRLF. It validates a bounded request line, chooses a fixed response, formats Content-Length, writes headers/body and closes. There is no filesystem URL mapping.

## Workflow

1. Start the receiving/listening endpoint with the documented port.
2. Start the peer or client with the matching endpoint.
3. Exchange the protocol's bounded records or byte stream.
4. Observe completion and close semantics rather than assuming one read is a message.
5. Exercise the documented failure cases and record the result.

## Implementation

- [server.c](server.c)
- [net.c](../../common/net.c)

Canonical chapter implementations are reused where linked above; each project
Makefile builds those real sources. Shared helper contracts are documented in
[common/README.md](../../common/README.md). No generated executable is required in
the ZIP: build for your own Linux environment.

## Compilation

From the repository root:

```bash
make build/http_server
```

Alternatively, from this project directory run `make`; its Makefile calls the root
build and outputs executables into the root `build/` directory.

## Execution

Run these from the repository root. Terminal labels indicate separate shells.

```bash
# Terminal A
./build/http_server 8080
# Terminal B (install curl if needed)
curl -i http://127.0.0.1:8080/
curl -I http://127.0.0.1:8080/health
curl -i http://127.0.0.1:8080/missing
```

**Expected behavior:** / returns Socket Programming Lab, /health returns ok, and /missing returns 404. HEAD reports the corresponding Content-Length without sending a body.

## Testing

```bash
python3 tests/test_course.py CourseTests.test_20_http_routes_head_and_errors CourseTests.test_21_http_fragmented_headers
```

Test split headers, unknown paths, unsupported methods and oversized input. This is a teaching subset: no full Host validation, request bodies, keep-alive, TLS, HTTP/2 or production hardening. Its operation timeout is not a total slow-client deadline.

## Possible Improvements

Add a monotonic overall request deadline and a nonblocking parser before extending routes. A real Internet service should use a mature HTTP implementation and secure deployment configuration.

## Completion rubric

- Explain every socket's owner and who closes it.
- State what separates messages or signals stream completion.
- Demonstrate a normal case and an input/disconnect boundary.
- Explain one implementation limit without claiming an unimplemented feature.
- Save your own commands and observed results in the [lab notebook](../../docs/lab-notebook.md).
