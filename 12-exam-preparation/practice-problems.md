# Output prediction, debugging and programming

## Output prediction

1. **byte-order.c uses 9000.** Predict the wire bytes.
   **Answer:** 23 28 in hexadecimal; the roundtrip is 9000.
2. **socket-info prints its descriptor.** Can you predict the exact integer?
   **Answer:** no fixed number is guaranteed; descriptor allocation depends on
   already-open descriptors. Nonnegative indicates successful allocation.
3. **fork-demo sets 99 in the child.** What does the parent print?
   **Answer:** 7. The ordinary variable is process-private after fork.
4. **thread-demo joins after the worker assigns 99.** What is the final value?
   **Answer:** 99, with completion synchronization from join.
5. **counter runs four workers with 100000 protected increments.** Final count?
   **Answer:** 400000. Removing the mutex introduces a data race, so do not predict
   a defined “slightly smaller” result for the broken version.
6. **Two one-byte frames A and B arrive together.** How many frames are parsed?
   **Answer:** two; each has its own length header despite arriving in one batch.
7. **send("abc",3), send("def",3), recv(...,6).** Must recv return 6?
   **Answer:** no. It may return a currently available positive prefix. The
   complete stream content is abcdef if the connection completes normally.
8. **HEAD /health.** Does the HTTP server send the text `ok\n`?
   **Answer:** no body for HEAD; it reports Content-Length: 3, the GET body size.

## Debugging questions

### D1. Treating received bytes as a string

Faulty fragment (not a reference implementation):

```c
ssize_t n = recv(fd, buffer, sizeof buffer, 0);
printf("%s", buffer);
```

**Fix:** check n < 0 and n == 0 first. For n > 0 use fwrite with the returned
length, or reserve an extra byte and append NUL within capacity if the protocol
is explicitly textual. recv does not append a terminator.

### D2. Losing a partial send

Faulty fragment:

```c
send(fd, data, length, 0);
```

**Fix:** inspect the returned signed count. In blocking code, loop over remaining
bytes and EINTR as [send_all](../common/net.c) does. In a reactor, store the suffix
and resume on writable readiness. Do not busy-spin on EAGAIN.

### D3. Unsigned error conversion

Faulty fragment:

```c
size_t n = recv(fd, buffer, sizeof buffer, 0);
```

**Fix:** store the result in ssize_t. A -1 error converted to size_t can become a
huge positive value, causing out-of-bounds operations if later used as a length.

### D4. Racing thread arguments

Faulty design: every worker receives `&peer`, where peer is overwritten by the
accept loop.

**Fix:** give each worker stable owned argument storage or a synchronized queue.
See the [thread server](../06-multiple-client-server/thread-server/server.c). Main
must not close the handed-off descriptor because threads share the table.

### D5. Busy output events

Faulty design: register POLLOUT for every socket at every iteration, even when
there is nothing to send.

**Fix:** request writable events only for nonempty output queues. A normally
writable idle socket can otherwise wake the loop continuously.

### D6. Unsafe frame length

Faulty design: receive a uint32 length, then copy that many bytes into a 4096-byte
array without checking it.

**Fix:** convert network byte order, validate against capacity and protocol limit,
then read the body. Reject oversized input without attempting the unsafe copy.
See [frame_recv](../common/net.c).

## Programming problems with reference solutions

| Problem | Acceptance requirements | Reference and explanation |
|---|---|---|
| P1. Binary echo | All bytes preserved; EOF and partial send handled | [Iterative server](../06-multiple-client-server/iterative-server/server.c) + [helper](../common/net.c): explicit counts, no string assumptions |
| P2. Reusable request channel | Multiple bounded records on one connection | [Framed pair](../04-tcp-client-server/README.md): header/body loops and capacity checks |
| P3. Multi-client service | Idle client does not block another | [poll server](../08-io-multiplexing/poll/server.c): nonblocking sockets and per-peer queues |
| P4. File receiver | No overwrite; binary chunks; truncation cleanup | [Receiver](../11-real-world-projects/file-transfer/receiver.c): O_EXCL, validated chunks, end marker and acknowledgement |
| P5. Connect deadline | Failed completion is not treated as success | [Connect demo](../09-advanced-sockets/blocking-vs-nonblocking/connect.c): monotonic budget plus SO_ERROR |
| P6. Group chat | Split lines retained, oversize rejected, output bounded | [Chat server](../11-real-world-projects/multi-client-chat/server.c): per-peer input/output state |

Implement independently first, then compare with the reference. Award credit for
correct ownership, framing and failure handling, not for matching variable names.
