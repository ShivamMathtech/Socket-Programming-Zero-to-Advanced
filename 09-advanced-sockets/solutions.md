# Chapter 09 — Worked practice solutions

[Return to chapter](README.md)

## Beginner

**Task:** What is the difference between EAGAIN and EOF?

**Solution:** EAGAIN means the requested nonblocking operation cannot progress now; a positive-length TCP recv returning zero means orderly end of the incoming stream.

## Intermediate

**Task:** Why use CLOCK_MONOTONIC for elapsed deadlines?

**Solution:** Wall time can jump due to clock adjustments. A monotonic elapsed-time clock avoids those jumps for a process deadline.

## Advanced

**Task:** A hostname-based client must finish within 2 s. Does this connect demo solve that whole problem?

**Solution:** No. It accepts a numeric address and does not budget DNS resolution or multiple address attempts. A complete design must include those steps in the same deadline and choose a resolver strategy that can honor it.

## Mini-project acceptance checklist

Goal: Build a deadline-aware framed request client whose DNS/address assumptions are explicit and whose header/body exchange shares one overall budget.

- State the input/wire format and the relevant capacity or resource bound.
- Retain descriptor cleanup on success and on each new error path.
- Demonstrate a normal case and the chapter's failure case: treating einprogress as immediate success.
- Explain the observation in your own words and include exact commands.
- Rebuild with warnings enabled and rerun relevant existing checks.

These are extension exercises, not claims that an additional application beyond
the supplied implementation has already been added. The runnable reference for
this chapter is linked in its main guide.
