# Chapter 02 — Worked practice solutions

[Return to chapter](README.md)

## Beginner

**Task:** Label each descriptor after accept succeeds.

**Solution:** The original descriptor remains a listener; the returned descriptor carries bytes for the one connected peer.

## Intermediate

**Task:** Add a cumulative received-byte count and send an input containing repeated small writes.

**Solution:** Increase an unsigned counter only for positive recv results. The count should match application bytes, independently of how many recv calls occurred.

## Advanced

**Task:** Explain why a server that calls send once per recv can corrupt a large echo exchange.

**Solution:** send may accept only a prefix. Save the number sent and retry the remaining suffix; on failure close that peer instead of pretending the exchange completed.

## Mini-project acceptance checklist

Goal: Extend the server to uppercase only ASCII a..z while leaving every other byte unchanged. Retain partial-write handling and binary-safe lengths.

- State the input/wire format and the relevant capacity or resource bound.
- Retain descriptor cleanup on success and on each new error path.
- Demonstrate a normal case and the chapter's failure case: calling recv on the listener.
- Explain the observation in your own words and include exact commands.
- Rebuild with warnings enabled and rerun relevant existing checks.

These are extension exercises, not claims that an additional application beyond
the supplied implementation has already been added. The runnable reference for
this chapter is linked in its main guide.
