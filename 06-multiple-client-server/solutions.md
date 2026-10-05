# Chapter 06 — Worked practice solutions

[Return to chapter](README.md)

## Beginner

**Task:** Which unused socket does the forked child close?

**Solution:** Its inherited listening descriptor. It serves only the accepted peer.

## Intermediate

**Task:** Why is &peer from the accept loop unsafe as a thread argument?

**Solution:** The loop can replace peer before the worker reads it, and the argument lifetime is not unique to that worker. Allocate a separate argument or pass through a synchronized queue.

## Advanced

**Task:** What happens when the worker limit is reached?

**Solution:** This implementation closes the newly accepted connection rather than spawning more work. A production protocol may return an overload response, but that response itself needs bounded I/O.

## Mini-project acceptance checklist

Goal: Build a concurrent echo service with a configurable worker cap and an overload counter. Demonstrate that one idle client does not prevent a second client receiving an echo.

- State the input/wire format and the relevant capacity or resource bound.
- Retain descriptor cleanup on success and on each new error path.
- Demonstrate a normal case and the chapter's failure case: leaving the parent's connected descriptor open after fork.
- Explain the observation in your own words and include exact commands.
- Rebuild with warnings enabled and rerun relevant existing checks.

These are extension exercises, not claims that an additional application beyond
the supplied implementation has already been added. The runnable reference for
this chapter is linked in its main guide.
