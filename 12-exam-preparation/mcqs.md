# Multiple-choice questions

Attempt all items before the answer key. Each item has one best answer under
the Linux/POSIX assumptions of this course.

## Q1. After accept succeeds, which descriptor carries that peer's data?

- **A.** The listening descriptor
- **B.** The new descriptor returned by accept
- **C.** The port number
- **D.** Any open descriptor

## Q2. A TCP sender calls send twice. The receiver must observe:

- **A.** Exactly two recv calls
- **B.** Exactly two packets
- **C.** The same byte sequence in order, with no guaranteed recv-call boundaries
- **D.** One NUL-terminated string

## Q3. send returns 7 after being asked to send 10 bytes. What is correct?

- **A.** Send all 10 again
- **B.** Advance the offset by 7 and retain the final 3
- **C.** Assume all 10 reached the peer application
- **D.** Close immediately without checking protocol

## Q4. A positive-length TCP recv returns zero. This usually means:

- **A.** Would block
- **B.** The buffer is full
- **C.** Orderly incoming EOF
- **D.** A 0-byte TCP application frame arrived

## Q5. A UDP recvfrom returns zero. Which interpretation is possible?

- **A.** A valid empty datagram
- **B.** The sender has performed an accepted-connection close
- **C.** The local port was unbound automatically
- **D.** All queued datagrams were concatenated

## Q6. Why must a select descriptor be less than FD_SETSIZE?

- **A.** To reserve ports
- **B.** Because FD_SET uses a bounded representation
- **C.** Because all Unix hosts allow only that many sockets
- **D.** Because TCP cannot support larger descriptor numbers

## Q7. Nonblocking connect returns EINPROGRESS. Next:

- **A.** Assume success and exit
- **B.** Repeatedly call connect in a busy loop
- **C.** Wait for completion and read SO_ERROR
- **D.** Call listen on the same connected client

## Q8. Which makes counter++ safe between concurrent threads?

- **A.** volatile alone
- **B.** A shared mutex policy covering conflicting accesses
- **C.** A sleep before every increment
- **D.** Printing the counter frequently

## Q9. What is the server backlog argument?

- **A.** Guaranteed maximum lifetime clients
- **B.** Requested pending-connection queue capacity subject to OS limits
- **C.** Maximum application payload
- **D.** Number of CPU cores

## Q10. A receive buffer is smaller than one UDP datagram. Usually:

- **A.** Remaining bytes are returned by the next recvfrom
- **B.** The excess bytes of that datagram are discarded
- **C.** UDP automatically changes to TCP
- **D.** The kernel appends a terminator

## Q11. Which socket option is relevant to IPv4 broadcast sending?

- **A.** SO_BROADCAST
- **B.** SO_TYPE
- **C.** SO_ERROR only
- **D.** IPV6_V6ONLY

## Q12. A reactor should request writable events:

- **A.** Permanently for every socket
- **B.** Only when its state has output requiring progress
- **C.** Only after the socket is closed
- **D.** Instead of reading any data

## Q13. Why use a monotonic clock for a request deadline?

- **A.** It synchronizes every host clock
- **B.** It avoids wall-clock adjustments affecting elapsed budgets
- **C.** It guarantees exact scheduling
- **D.** It makes DNS nonblocking

## Q14. Which event permits a file sender to claim the supplied application protocol completed?

- **A.** connect returned success
- **B.** send accepted the first chunk
- **C.** The receiver's OK acknowledgement arrived after the end marker
- **D.** The input filename was printed

## Q15. Why not add EPOLLET to the supplied epoll server unchanged?

- **A.** EPOLLET is unavailable on Linux
- **B.** The bounded progress loop can stop before EAGAIN and miss another edge
- **C.** EPOLLET automatically copies all remaining bytes
- **D.** TCP does not support nonblocking sockets

## Q16. Which expression safely prints received binary data after n > 0?

- **A.** printf("%s", buffer) without a terminator
- **B.** fwrite(buffer, 1, (size_t)n, stdout)
- **C.** strlen(buffer) as the received count
- **D.** sizeof(buffer) bytes regardless of n

## Answer key with explanations

| Question | Answer | Why |
|---|---|---|
| 1 | B | accept returns a connected socket; the listener remains available for further acceptance. |
| 2 | C | TCP is a byte stream. Applications impose record boundaries themselves. |
| 3 | B | Only the accepted prefix progressed. Resending it duplicates application bytes. |
| 4 | C | EOF is a transport receive condition, not an application frame. |
| 5 | A | UDP supports zero-length datagrams. |
| 6 | B | This is an API representation constraint, not a universal kernel socket-count limit. |
| 7 | C | Completion readiness can indicate success or failure; SO_ERROR distinguishes them. |
| 8 | B | The others provide no required synchronization for conflicting memory access. |
| 9 | B | Worker limits and admission policy are separate from listen backlog. |
| 10 | B | Datagram receive consumes the message, truncating it if the buffer is insufficient. |
| 11 | A | SO_BROADCAST enables sending to IPv4 broadcast destinations. |
| 12 | B | Permanent writable interest on idle sockets can generate a busy loop. |
| 13 | B | Monotonic elapsed time avoids wall-time jumps; it does not remove scheduling delay or DNS blocking. |
| 14 | C | This protocol defines acknowledgement after the receiver flush/close path. Lost acknowledgements remain ambiguous. |
| 15 | B | Edge-triggered operation requires an appropriate drain/rearm/backpressure design. |
| 16 | B | fwrite takes an explicit count; the other forms assume string meaning or include invalid bytes. |
