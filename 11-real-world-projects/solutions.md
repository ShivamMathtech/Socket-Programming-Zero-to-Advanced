# Chapter 11 — Worked practice solutions

[Return to chapter](README.md)

## Beginner

**Task:** Choose the right base project for a binary sensor-log upload.

**Solution:** File transfer demonstrates binary chunks and completion acknowledgement. A text chat protocol would need additional binary framing.

## Intermediate

**Task:** Why does the HTTP project map only fixed routes?

**Solution:** It keeps the lesson focused on parsing/responding and avoids uncontrolled filesystem paths. A static-file server would need separate path normalization, access policy and streaming design.

## Advanced

**Task:** A file exists but the sender did not receive OK. Is it safe to retry blindly?

**Solution:** The transfer may have committed before acknowledgement was lost. Define transfer identifiers and idempotency/reconciliation policy before automatic retry; the supplied receiver refuses overwrites.

## Mini-project acceptance checklist

Goal: Complete all seven projects and maintain one lab notebook entry for a normal exchange and one failure boundary in each.

- State the input/wire format and the relevant capacity or resource bound.
- Retain descriptor cleanup on success and on each new error path.
- Demonstrate a normal case and the chapter's failure case: using a filename received from the network as an unchecked local path.
- Explain the observation in your own words and include exact commands.
- Rebuild with warnings enabled and rerun relevant existing checks.

These are extension exercises, not claims that an additional application beyond
the supplied implementation has already been added. The runnable reference for
this chapter is linked in its main guide.
