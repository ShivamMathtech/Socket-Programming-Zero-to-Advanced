# Chapter 01 — Worked practice solutions

[Return to chapter](README.md)

## Beginner

**Task:** Explain why a successful socket call does not prove a server is reachable.

**Solution:** It allocates a local endpoint; the program has not attempted routing or a TCP handshake.

## Intermediate

**Task:** Add a failing getsockopt experiment using an invalid descriptor. What should be checked?

**Solution:** Expect -1 and inspect errno immediately. Do not print the output value as though it were filled successfully.

## Advanced

**Task:** Explain why storing only a descriptor number in a long-lived asynchronous job is risky.

**Solution:** After close the number can be reused for another connection. Keep lifetime/ownership information and use generation tokens or deferred reuse where needed.

## Mini-project acceptance checklist

Goal: Write a socket-capability inspector that creates IPv4 TCP, IPv4 UDP and IPv6 TCP sockets, prints success/failure for each and closes every successful allocation.

- State the input/wire format and the relevant capacity or resource bound.
- Retain descriptor cleanup on success and on each new error path.
- Demonstrate a normal case and the chapter's failure case: assuming descriptor 3 is guaranteed.
- Explain the observation in your own words and include exact commands.
- Rebuild with warnings enabled and rerun relevant existing checks.

These are extension exercises, not claims that an additional application beyond
the supplied implementation has already been added. The runnable reference for
this chapter is linked in its main guide.
