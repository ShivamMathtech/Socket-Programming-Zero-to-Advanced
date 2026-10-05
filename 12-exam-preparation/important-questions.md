# Important exam questions — worked answers

[Exam chapter](README.md)

## Short questions

### S1. What is a socket?

A kernel-managed communication endpoint accessed through descriptor references. The descriptor is a process-local handle, not a transport port.

### S2. What does bind choose?

The local address and port. A server normally binds before listen; a client can let connect select a local endpoint implicitly.

### S3. What does accept return?

A new connected descriptor. The listener remains a separate socket that can accept more peers.

### S4. What happens if send writes fewer bytes than requested?

Advance by the returned count and preserve the remaining suffix. A nonblocking loop resumes after writable readiness; a blocking helper can continue immediately until complete or failed.

### S5. What does positive-length TCP recv returning zero mean?

Orderly end of the incoming byte stream. Any preceding bytes have been delivered to the application before that EOF result.

### S6. Why can zero from UDP recvfrom mean something different?

UDP permits empty datagrams. There is no accepted stream whose write-half EOF is being reported.

### S7. Does TCP preserve the sender's message boundaries?

No. The application needs framing such as fixed sizes, delimiters or validated length prefixes.

### S8. Why use network byte order?

An explicit wire representation permits machines with different host representations to interpret integers consistently. Convert ports and length fields intentionally.

### S9. Why ignore SIGPIPE in these examples?

It lets send report an ordinary error such as EPIPE instead of terminating the process before the program handles failure.

### S10. What is a zombie child?

A terminated child whose parent has not collected its exit status. waitpid reaps that bookkeeping.

### S11. Why is FD_SETSIZE a correctness constraint?

FD_SET is invalid for descriptors outside the bit set's supported range. It is a descriptor-number constraint, not just a count of clients.

### S12. Why does readiness not mean one full request arrived?

Readiness says an operation can progress or report a condition. Application record sizes and framing are separate.

### S13. Why can a timeout be different from a deadline?

An operation timeout applies to one blocking call; an absolute deadline bounds a sequence of operations under one shared time budget.

### S14. What are the consequences of a failed acknowledgement after file commit?

The receiver may hold a complete file while the sender reports failure. A safe retry policy needs application-level reconciliation/idempotency.

## Long questions

Use the points below as a marking guide; expand them with diagrams and actual code
references rather than memorizing a paragraph.

### L1. Explain the TCP server lifecycle with resource ownership.

Create a socket; configure options; bind a local endpoint; listen; accept connected sockets. Main owns the listener. A handler owns each accepted socket and loops on bytes with partial-write handling. EOF/error finishes that peer; close exactly once. A fork changes reference ownership differently from a thread. Include all acquisition failure cleanup paths.

### L2. Design a binary-safe framed protocol.

Use a precisely sized integer header in network order, cap the decoded length, read header/body fully and define valid empty messages. Distinguish clean boundary EOF from partial-record EOF. Test split headers, split bodies, multiple records per write, binary NUL, excessive lengths and disconnects.

### L3. Compare iterative, fork and thread servers.

Discuss service scheduling, memory isolation/sharing, descriptor ownership, worker admission, failure isolation and cleanup. The iterative server is simple but stalls behind an idle peer. Fork needs descriptor closure and child reaping. Threads need stable arguments, synchronization and lifecycle management. Do not invent a universal speed ranking.

### L4. Explain a nonblocking echo state machine.

Store a socket, queued output count and incoming-EOF flag. Read only with queue capacity, write only when output exists, preserve short-write suffixes and handle would-block without losing state. After EOF, drain replies before close. Set/clear readiness interest from current state and bound work per event.

### L5. Compare select, poll and epoll.

Explain descriptor sets versus arrays versus persistent kernel interest. Cover select's FD_SETSIZE requirement and destructive sets, poll revents/HUP handling, epoll add/mod/del and level/edge triggering. Costs depend on monitored/ready counts, registration churn and application work; correctness comes first.

### L6. Describe reliable behavior built over UDP.

Specify IDs, acknowledgement matching, retry/time limits, duplicate suppression, ordering if required and congestion-aware pacing. Clarify whether a newer state supersedes an older one. UDP connect alone supplies none of these application guarantees.

### L7. Debug a connected client that receives no response.

Confirm the endpoint and successful connect, then inspect sent byte counts, framing, parser state and request/reply ordering. Look for missing terminators, partial header/body, both peers waiting to read, or output interest not enabled. Use traces/captures to test a hypothesis, not to replace protocol reasoning.

### L8. Design a meaningful server comparison experiment.

Hold wire protocol, payload sizes, clients and host configuration constant. Verify correctness before timing. Vary concurrency and active/idle mix deliberately; collect repeated latency distributions, throughput, errors and resource use. Separate client scheduling and capture/tracing overhead from server behavior.
