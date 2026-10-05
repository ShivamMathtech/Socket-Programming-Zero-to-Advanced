# Protocols, ownership and limits

| Program family | Wire format | Termination | Resource policy |
|---|---|---|---|
| Raw TCP echo | Arbitrary bytes | Peer write-half closes; server drains echo | Minimal server one client; later blocking peers 15 s per operation |
| Framed echo | 4-byte big-endian unsigned length, then bytes | Clean EOF between headers | At most 1 MiB per frame, sequential clients |
| UDP echo | One request datagram, one identical reply | No connection EOF | Client sends at most 1200 bytes; server receive buffer covers UDP payload sizes |
| TCP chat | Raw terminal bytes | EOF from peer ends session | Blocking writes with 15 s operation timeout |
| UDP chat | Each input read becomes a datagram | Ctrl-D exits that local process | 1200-byte outbound chunks, no retries or delivery receipts |
| Group chat | Newline-delimited input; `peer#N: text` output | EOF drains current queue, drops unfinished line | 32 peers, 512 bytes excluding newline, 16 KiB output per peer |
| File transfer | Repeated 4-byte chunk length + binary bytes; zero length ends file | Receiver replies two ASCII bytes `OK` | 32768-byte chunks, 64 MiB file maximum, one connection |
| HTTP teaching server | GET/HEAD request line + CRLF headers | One response then connection close | 8192 header bytes, 3 s per blocking operation, fixed routes |

## Buffering and framing

TCP carries a sequence of bytes. Send and receive call boundaries do not survive
as message boundaries. The framed protocol first reads exactly four bytes, uses
`ntohl` to interpret a length, checks that length, then reads the body. A body of
zero bytes is a real frame in the echo protocol; it is an end marker only in the
file-transfer protocol. Protocol meaning belongs to the application.

`frame_recv` returns 1 for a frame, 0 for EOF before a new header, and -1 for an
error or truncated record. A partial header/body at EOF is an error. A length
above the receiver's capacity is rejected before reading the body. The receiver
then closes the connection because it is no longer synchronized with that stream.

## Ownership and close

The accepting thread/process owns each new descriptor until ownership is handed
to a worker or event-loop slot. A forked child closes its inherited listener and
the parent closes its accepted descriptor. A thread shares the descriptor table,
so the accepting thread must not close the worker's descriptor.

Ctrl-C and SIGTERM use default process termination. The OS releases open
resources. These examples do not implement coordinated worker draining on
shutdown. The file receiver removes partial output on handled transfer failures;
a forced termination can leave a partial file for the learner to remove. An
atomic temporary-file-and-rename protocol is an extension, not an implemented
promise. Existing destination files are never overwritten by the receiver.

## Concurrency and fairness

The fork and thread servers cap active clients at 64. The three echo reactors
cap clients at 64 and keep 16 KiB output per connection. They stop reading when
that queue is full, allowing TCP backpressure to reach the sender. Work per
readiness event is bounded. The group chat drops a slow receiver if a broadcast
would overflow its queue; other clients can continue.

No idle deadline is implemented for the event-loop peers. An idle client can
occupy a slot indefinitely. Socket timeouts on blocking servers apply to one
operation, not a total transaction: a peer making tiny progress can extend total
time. The nonblocking connect demonstration uses a monotonic overall deadline.

## Deliberate project boundaries

HTTP is a teaching subset, not a standards-complete or hardened HTTP server: no
Host validation, request-body processing, persistent connections, TLS, chunking,
URL decoding or filesystem serving. Unknown routes return 404; there is no path
to arbitrary local files. A sequential client can delay other clients until an
operation times out. Do not use it as an Internet-facing production service.

File acknowledgement confirms the receiver completed its local write/flush/close
path. It is not authentication, a cryptographic end-to-end digest, or an
exactly-once transaction. If the acknowledgement is lost, a complete file may
exist while the sender reports failure. The integration suite checks local binary
content with SHA-256; the C wire protocol does not transmit a hash.

Linux `IPV6_V6ONLY` is explicitly enabled for IPv6 listeners. The IPv6 example
therefore listens on `::1` only, not IPv4-mapped addresses. Generic clients try
`getaddrinfo` candidates; their DNS resolution and blocking connects have no
end-to-end deadline. The separate connect-timeout demo takes a numeric IPv4
address, avoiding that ambiguity.
