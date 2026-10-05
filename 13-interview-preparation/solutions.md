# Chapter 13 — Worked practice solutions

[Return to chapter](README.md)

## Beginner

**Task:** Answer “socket versus port” in two sentences.

**Solution:** A socket is a communication endpoint represented by kernel state and descriptor references. A port is a transport-layer identifier used with addresses/protocol to demultiplex traffic.

## Intermediate

**Task:** Explain how a peer half-close affects a queued-response server.

**Solution:** Stop expecting new incoming bytes after EOF, but preserve and send already queued replies before closing the connection.

## Advanced

**Task:** Propose a fair service design for many idle connections and occasional expensive requests.

**Solution:** Use a nonblocking readiness loop for connections and a bounded worker queue for CPU work, with explicit ownership of socket writes, request IDs, deadlines and overload rejection. Measure with representative idle/active ratios.

## Mini-project acceptance checklist

Goal: Present a five-minute design defense for the concurrent server project, including one overload scenario and one ambiguous retry scenario.

- State the input/wire format and the relevant capacity or resource bound.
- Retain descriptor cleanup on success and on each new error path.
- Demonstrate a normal case and the chapter's failure case: jumping to an api before clarifying requirements.
- Explain the observation in your own words and include exact commands.
- Rebuild with warnings enabled and rerun relevant existing checks.

These are extension exercises, not claims that an additional application beyond
the supplied implementation has already been added. The runnable reference for
this chapter is linked in its main guide.
