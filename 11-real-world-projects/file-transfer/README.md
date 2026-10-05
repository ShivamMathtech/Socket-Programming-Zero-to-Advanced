# Project 5 — Binary File Transfer

[All projects](../README.md) · [Course home](../../README.md)

## Problem Statement

Transfer arbitrary file bytes and report completion only after a receiver acknowledgement.

## Requirements

One new destination file; 32 KiB chunks; 64 MiB receiver cap; binary-safe data; explicit zero-length end marker; remove partial files on handled errors.

Prerequisites: Chapters 00–05 for echo/datagrams, 06–09 for concurrency and queue
management. Review the matching chapter before modifying project code.

## Architecture

Sender reads local file chunks and sends length-prefixed records. Receiver validates each length and total budget, writes fully, fsyncs/closes, then sends OK. The network never controls the output path.

## Workflow

1. Start the receiving/listening endpoint with the documented port.
2. Start the peer or client with the matching endpoint.
3. Exchange the protocol's bounded records or byte stream.
4. Observe completion and close semantics rather than assuming one read is a message.
5. Exercise the documented failure cases and record the result.

## Implementation

- [sender.c](sender.c)
- [receiver.c](receiver.c)
- [net.c](../../common/net.c)

Canonical chapter implementations are reused where linked above; each project
Makefile builds those real sources. Shared helper contracts are documented in
[common/README.md](../../common/README.md). No generated executable is required in
the ZIP: build for your own Linux environment.

## Compilation

From the repository root:

```bash
make build/file_sender build/file_receiver
```

Alternatively, from this project directory run `make`; its Makefile calls the root
build and outputs executables into the root `build/` directory.

## Execution

Run these from the repository root. Terminal labels indicate separate shells.

```bash
# Terminal A (received.bin must not already exist)
./build/file_receiver 9000 received.bin
# Terminal B
./build/file_sender 127.0.0.1 9000 README.md
# After both complete
sha256sum README.md received.bin
```

**Expected behavior:** Both endpoints report byte totals. The two locally computed SHA-256 digests match. The C protocol itself does not transmit a digest.

## Testing

```bash
python3 tests/test_course.py CourseTests.test_17_file_transfer_binary_and_empty CourseTests.test_18_file_transfer_refuses_overwrite CourseTests.test_19_file_transfer_truncation_cleanup
```

Tests include a binary file with NUL bytes, an empty file, an existing destination and a truncated chunk. Forced process termination can leave a partial file; handled truncation removes it. A completed file survives a failed acknowledgement send.

## Possible Improvements

Use a temporary file and atomic rename, add transfer IDs for reconciliation, transmit/verify a cryptographic digest, then define resumability. Authentication/encryption require a maintained TLS library.

## Completion rubric

- Explain every socket's owner and who closes it.
- State what separates messages or signals stream completion.
- Demonstrate a normal case and an input/disconnect boundary.
- Explain one implementation limit without claiming an unimplemented feature.
- Save your own commands and observed results in the [lab notebook](../../docs/lab-notebook.md).
