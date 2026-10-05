# Framed protocol architecture


The parser has two logical states: awaiting a 4-byte header and awaiting its
validated body. In the blocking implementation these states live in the call
stack and offsets inside recv_exact. In a nonblocking version they would need to
live in a per-connection object across readiness events.

For length L, one wire record occupies 4+L bytes. If L=0, the header alone forms a
record. All unsigned length arithmetic must be checked against buffer capacity
before copying, allocating or computing offsets.

| Event | Receiver action |
|---|---|
| EOF before a header | Clean session end |
| EOF after a partial header | Protocol truncation error |
| Length > capacity | Reject and close |
| EOF during nonempty body | Protocol truncation error |
| Body complete | Process exactly L bytes |
| Another header follows immediately | Parse another record |

The test suite covers fragmented headers/bodies, coalesced records, embedded NUL,
empty messages, excessive lengths and partial-header EOF. This is parser
verification, not a throughput benchmark.
