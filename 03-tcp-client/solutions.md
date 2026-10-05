# Chapter 03 — Worked practice solutions

[Return to chapter](README.md)

## Beginner

**Task:** What does shutdown(SHUT_WR) preserve?

**Solution:** The receive direction and the descriptor remain available; the client can read the server response.

## Intermediate

**Task:** Predict what happens if neither peer sends and both call recv.

**Solution:** Both can block indefinitely in these minimal examples. A protocol must decide who speaks first and how completion is represented.

## Advanced

**Task:** Why can sending an arbitrarily huge payload before reading an echo deadlock?

**Solution:** Both send buffers can fill: the server waits for the client to read echoed bytes while the client waits for the server to read more request bytes. Use bounded request/response frames or concurrent duplex I/O.

## Mini-project acceptance checklist

Goal: Create a request client that reports local and peer endpoints and a reply byte count while keeping all received output binary-safe.

- State the input/wire format and the relevant capacity or resource bound.
- Retain descriptor cleanup on success and on each new error path.
- Demonstrate a normal case and the chapter's failure case: using localhost with the numeric-only client.
- Explain the observation in your own words and include exact commands.
- Rebuild with warnings enabled and rerun relevant existing checks.

These are extension exercises, not claims that an additional application beyond
the supplied implementation has already been added. The runnable reference for
this chapter is linked in its main guide.
