# Chapter 10 — Worked practice solutions

[Return to chapter](README.md)

## Beginner

**Task:** Which command lists TCP listening sockets with numeric addresses?

**Solution:** ss -ltnp; process details may require additional privileges for other users' processes.

## Intermediate

**Task:** ss shows a listener but the client uses a different port. What changes?

**Solution:** Correct the configured endpoint in one command. Do not change packet filtering or protocol code for an address mismatch.

## Advanced

**Task:** TCP establishment succeeds but the client waits forever for a reply. What next?

**Solution:** Inspect the application protocol: request terminator/length, server parser state, request/reply ordering and output queue. A completed handshake alone cannot prove a complete request arrived.

## Mini-project acceptance checklist

Goal: Create a debugging report for three intentionally induced failures with hypothesis, evidence, root cause, correction and successful rerun.

- State the input/wire format and the relevant capacity or resource bound.
- Retain descriptor cleanup on success and on each new error path.
- Demonstrate a normal case and the chapter's failure case: using ping success as proof that a tcp service is running.
- Explain the observation in your own words and include exact commands.
- Rebuild with warnings enabled and rerun relevant existing checks.

These are extension exercises, not claims that an additional application beyond
the supplied implementation has already been added. The runnable reference for
this chapter is linked in its main guide.
