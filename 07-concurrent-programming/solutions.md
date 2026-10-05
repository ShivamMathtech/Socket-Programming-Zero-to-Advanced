# Chapter 07 — Worked practice solutions

[Return to chapter](README.md)

## Beginner

**Task:** Why does the parent print 7 in fork-demo?

**Solution:** The child modifies its own private address-space copy, not the parent's variable.

## Intermediate

**Task:** Is the thread-demo assignment safe without a mutex?

**Solution:** Yes in this program: main reads only after pthread_join establishes that the worker completed; no overlapping unsynchronized read/write occurs.

## Advanced

**Task:** Two chat operations acquire user-lock and room-lock in opposite orders. What is the fix?

**Solution:** Define one global lock acquisition order or redesign ownership to avoid nested locks. Merely increasing timeouts does not remove the deadlock cycle.

## Mini-project acceptance checklist

Goal: Create a small worker group that computes per-connection byte totals locally and merges them under a mutex only when a connection finishes.

- State the input/wire format and the relevant capacity or resource bound.
- Retain descriptor cleanup on success and on each new error path.
- Demonstrate a normal case and the chapter's failure case: using volatile as a lock.
- Explain the observation in your own words and include exact commands.
- Rebuild with warnings enabled and rerun relevant existing checks.

These are extension exercises, not claims that an additional application beyond
the supplied implementation has already been added. The runnable reference for
this chapter is linked in its main guide.
