# Socket interview questions

Answer aloud first. The worked answers are intentionally concise; follow with
one concrete example from the code and one relevant limitation.

## 1. What is a socket?

A kernel communication endpoint referred to by a descriptor. Family, type, protocol and state determine which operations are valid.

## 2. Socket versus port?

A socket is endpoint state; a port is one numeric part of a transport endpoint. A descriptor number is a process-local reference to the socket.

## 3. What does bind do?

Selects a local endpoint. It does not establish a remote connection or create an application worker.

## 4. What does listen do?

Marks a stream socket as passive and requests pending-connection queue behavior; it does not start a user-space thread.

## 5. What does accept return and why a new descriptor?

It returns one connected peer so the listener can remain available independently. Each connection needs its own I/O state.

## 6. Blocking versus nonblocking?

Blocking operations may wait; nonblocking operations return would-block when progress is unavailable. Nonblocking code must retain progress and wait efficiently.

## 7. What does recv return zero mean?

For a positive-length TCP receive, orderly incoming EOF. For a zero requested length or a zero-length datagram, do not apply that conclusion.

## 8. How do you handle partial writes?

Track an offset into immutable pending data. Advance only by a positive result, handle interruption, and retain the suffix across readiness waits.

## 9. select versus poll versus epoll?

select uses destructive bounded bit sets, poll descriptor arrays, and Linux epoll registered interest/ready-event batches. Their correctness constraints and workload costs differ.

## 10. Why inspect SO_ERROR after nonblocking connect?

Writable/error readiness can report failed establishment too. SO_ERROR returns the completion error; zero means success.

## 11. Can a read-ready socket still return EAGAIN?

A competing reader or changed state can consume availability before your operation. Nonblocking handlers must tolerate that race.

## 12. How do you stop slow clients exhausting memory?

Use bounded queues and admission/deadline policies; pause input or disconnect a slow peer according to the protocol.

## 13. Why is one mutex around the whole worker poor design?

Blocking I/O under a shared lock can serialize all clients or contribute to deadlock. Protect only the shared state whose consistency requires it.

## 14. How do you avoid zombie children?

Periodically collect exits with waitpid or use a carefully designed child-lifecycle mechanism. Reaping only upon new traffic can leave zombies while idle.

## 15. Can a server lose track of a descriptor after close?

Its number can be reused. Event batches and asynchronous work need lifetime management so stale references do not operate on new peers.

## 16. What does shutdown add beyond close?

Direction-specific communication control such as write-half-close while reading a reply. close is still needed for descriptor release.

## 17. Why not send a struct directly?

Padding, alignment, representation, byte order and versioning can differ. Define a byte-level serialization format.

## 18. How would you add TLS?

Use a maintained library and integrate its stateful handshake/read/write requirements, certificate verification and deadlines. Plain send/recv examples are not automatically encrypted.

## 19. What is your protocol test strategy?

Normal data plus fragmentation, coalescing, empty records, max/oversized lengths, truncation, simultaneous peers, slow readers and ambiguous completion. Assert content and lifecycle, not arbitrary packet grouping.

## 20. What is the biggest limitation of this example suite?

It is a bounded local teaching course. Authentication/TLS, durable state, full production protocol compliance and comprehensive deployment hardening are separate work.
