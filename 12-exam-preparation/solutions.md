# Chapter 12 — Worked practice solutions

[Return to chapter](README.md)

## Beginner

**Task:** Explain all three outcomes of a positive-length TCP recv.

**Solution:** Positive count: that many valid bytes; zero: orderly incoming EOF; -1: inspect errno for interruption, would-block or a failure.

## Intermediate

**Task:** Predict the wire bytes for port 443 and a 5-byte frame body length.

**Solution:** Port: 01 bb. uint32 body length: 00 00 00 05.

## Advanced

**Task:** Design an exam experiment that proves TCP does not retain message boundaries without relying on one particular packet segmentation.

**Solution:** Write a parser that reads fixed chosen chunk sizes while the sender uses different write sizes; verify the reconstructed byte stream. The proof depends on API semantics, not expecting one packet capture layout.

## Mini-project acceptance checklist

Goal: Take the supplied mock exam, mark against the rubric and rerun the example associated with each missed concept.

- State the input/wire format and the relevant capacity or resource bound.
- Retain descriptor cleanup on success and on each new error path.
- Demonstrate a normal case and the chapter's failure case: claiming a universal fixed recv chunk size.
- Explain the observation in your own words and include exact commands.
- Rebuild with warnings enabled and rerun relevant existing checks.

These are extension exercises, not claims that an additional application beyond
the supplied implementation has already been added. The runnable reference for
this chapter is linked in its main guide.
