# Viva questions

## 1. Can fd 0 be a valid socket?

Yes. Test for fd < 0 on failure.

## 2. Can a client bind before connect?

Yes, to request a particular local endpoint; it is not required for ordinary clients.

## 3. Is 0.0.0.0 a good client destination for a wildcard listener?

Use an actual reachable server address or loopback. The wildcard expresses local bind policy.

## 4. Does UDP use listen/accept?

Not for the datagram service shown here.

## 5. Does SO_REUSEADDR replace stopping an active listener?

No.

## 6. When does a fork parent close its accepted descriptor?

After creating the child handler; the child retains its reference.

## 7. When does a thread acceptor close a successfully handed-off descriptor?

It does not; the worker owns and closes it.

## 8. Why reset socklen_t before recvfrom?

It is capacity on input and actual length on output.

## 9. Does a NUL byte terminate a TCP payload?

Only if the application protocol defines that rule; the socket API does not.

## 10. Can a TCP peer still receive after SHUT_WR?

Yes.

## 11. What does EAGAIN mean in a nonblocking reactor?

No progress is possible now for that operation; preserve state and wait for appropriate readiness.

## 12. What should happen to queued output after incoming EOF?

If the protocol permits replies, drain it before closing.

## 13. What is the difference between an empty frame and EOF?

An empty frame has a parsed header declaring length zero; EOF is a receive condition.

## 14. What does getaddrinfo return on failure?

An EAI error code; use gai_strerror, with errno relevant for EAI_SYSTEM.

## 15. Are SO_RCVTIMEO and poll timeout the same setting?

No. Socket options affect certain blocking I/O operations; poll receives its own wait timeout argument.
