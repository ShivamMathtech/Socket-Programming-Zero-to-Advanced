# Project 3 — UDP Chat

[All projects](../README.md) · [Course home](../../README.md)

## Problem Statement

Exchange independent terminal datagrams between two locally bound peers.

## Requirements

Each side chooses a local port and the other peer endpoint; outgoing chunks <=1200 bytes; show incoming payloads; Ctrl-D exits locally.

Prerequisites: Chapters 00–05 for echo/datagrams, 06–09 for concurrency and queue
management. Review the matching chapter before modifying project code.

## Architecture

Each peer binds then connects its UDP socket to the chosen endpoint. poll watches stdin and the datagram socket. UDP connect chooses peer behavior without establishing reliability.

## Workflow

1. Start the receiving/listening endpoint with the documented port.
2. Start the peer or client with the matching endpoint.
3. Exchange the protocol's bounded records or byte stream.
4. Observe completion and close semantics rather than assuming one read is a message.
5. Exercise the documented failure cases and record the result.

## Implementation

- [peer.c](peer.c)
- [net.c](../../common/net.c)

Canonical chapter implementations are reused where linked above; each project
Makefile builds those real sources. Shared helper contracts are documented in
[common/README.md](../../common/README.md). No generated executable is required in
the ZIP: build for your own Linux environment.

## Compilation

From the repository root:

```bash
make build/udp_chat
```

Alternatively, from this project directory run `make`; its Makefile calls the root
build and outputs executables into the root `build/` directory.

## Execution

Run these from the repository root. Terminal labels indicate separate shells.

```bash
# Terminal A (wait until terminal B is running before typing)
./build/udp_chat 9001 127.0.0.1 9002
# Terminal B
./build/udp_chat 9002 127.0.0.1 9001
```

**Expected behavior:** Terminal input at A is displayed at B and vice versa. A local exit does not send a reliable remote shutdown notification.

## Testing

```bash
python3 tests/test_course.py CourseTests.test_14_udp_chat_peer
```

Try messages from both directions. Stop one peer and observe that sending success/ICMP error behavior is not a delivery receipt. There are no acknowledgements, retransmissions or duplicate suppression.

## Possible Improvements

Add message IDs and a deliberately bounded retry/acknowledgement policy. Document duplicate handling before claiming reliable chat.

## Completion rubric

- Explain every socket's owner and who closes it.
- State what separates messages or signals stream completion.
- Demonstrate a normal case and an input/disconnect boundary.
- Explain one implementation limit without claiming an unimplemented feature.
- Save your own commands and observed results in the [lab notebook](../../docs/lab-notebook.md).
