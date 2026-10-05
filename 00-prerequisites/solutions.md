# Chapter 00 — Worked practice solutions

[Return to chapter](README.md)

## Beginner

**Task:** Identify the TCP and UDP endpoints for two programs using 127.0.0.1:9000.

**Solution:** They share an IP and numeric port but use different transport protocols; TCP and UDP demultiplexing is separate.

## Intermediate

**Task:** Encode port 8080 as two network-order bytes without looking at the program.

**Solution:** 8080 decimal is 0x1f90. The transmitted bytes are 1f then 90; htons performs the conversion.

## Advanced

**Task:** Design a five-byte record: one type byte followed by a uint32 length. Why not send a C struct?

**Solution:** Write the type at offset 0, convert the uint32 with htonl and memcpy four bytes at offset 1. Struct padding, alignment and host byte order are not a portable wire format.

## Mini-project acceptance checklist

Goal: Create a byte-order inspector for a port and a four-byte length. Reject values outside their defined ranges and explain each output byte.

- State the input/wire format and the relevant capacity or resource bound.
- Retain descriptor cleanup on success and on each new error path.
- Demonstrate a normal case and the chapter's failure case: confusing an ip address with a mac address.
- Explain the observation in your own words and include exact commands.
- Rebuild with warnings enabled and rerun relevant existing checks.

These are extension exercises, not claims that an additional application beyond
the supplied implementation has already been added. The runnable reference for
this chapter is linked in its main guide.
