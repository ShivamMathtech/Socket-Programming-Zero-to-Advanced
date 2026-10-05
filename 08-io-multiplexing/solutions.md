# Chapter 08 — Worked practice solutions

[Return to chapter](README.md)

## Beginner

**Task:** Why must select sets be rebuilt?

**Solution:** select changes them to the ready subset. Reusing that subset without reconstruction loses interest in other active descriptors.

## Intermediate

**Task:** A send returns 100 for a 500-byte queue. What state remains?

**Solution:** Remove only the first 100 bytes, retain the remaining 400, and keep writable interest. Do not resend the already accepted prefix.

## Advanced

**Task:** Why cannot this code enable EPOLLET unchanged?

**Solution:** It deliberately performs bounded reads/writes per event and can stop before EAGAIN. Edge triggering might not notify again while data remains; draining or a deliberate rearm/backpressure design is required.

## Mini-project acceptance checklist

Goal: Add monotonic idle deadlines to the poll server and verify that an idle slot expires while active peers continue receiving replies.

- State the input/wire format and the relevant capacity or resource bound.
- Retain descriptor cleanup on success and on each new error path.
- Demonstrate a normal case and the chapter's failure case: registering pollout/epollout all the time and spinning.
- Explain the observation in your own words and include exact commands.
- Rebuild with warnings enabled and rerun relevant existing checks.

These are extension exercises, not claims that an additional application beyond
the supplied implementation has already been added. The runnable reference for
this chapter is linked in its main guide.
