# Chapter 05 — Worked practice solutions

[Return to chapter](README.md)

## Beginner

**Task:** Explain why UDP echo does not call accept.

**Solution:** Datagrams are delivered to a bound endpoint independently; there is no accepted stream connection per sender.

## Intermediate

**Task:** Use a 4-byte receive buffer with an 8-byte datagram. What remains?

**Solution:** Only the received prefix fits; excess bytes of that datagram are discarded. The next receive reads the next datagram.

## Advanced

**Task:** Design an idempotent UDP temperature query with a bounded retry policy.

**Solution:** Include request ID and sensor ID, retry a small fixed number with timeouts/backoff, match response IDs and suppress duplicates. Do not treat retries as proof that a failed response meant the request was never processed.

## Mini-project acceptance checklist

Goal: Build a sequence-numbered UDP echo monitor reporting sent IDs, received IDs and local RTT only for matched responses. Label missing replies as missing, not as measured one-way loss.

- State the input/wire format and the relevant capacity or resource bound.
- Retain descriptor cleanup on success and on each new error path.
- Demonstrate a normal case and the chapter's failure case: assuming udp is guaranteed to be faster.
- Explain the observation in your own words and include exact commands.
- Rebuild with warnings enabled and rerun relevant existing checks.

These are extension exercises, not claims that an additional application beyond
the supplied implementation has already been added. The runnable reference for
this chapter is linked in its main guide.
