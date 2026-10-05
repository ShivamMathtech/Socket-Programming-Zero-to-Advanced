# byte_order — numbered source walkthrough

[Walkthrough index](README.md) · [Actual source](../../00-prerequisites/byte-order.c)

## What does this code do?

Represent a port as explicit wire bytes.

## How the blocks work

| Source lines | Purpose and behavior |
|---|---|
| 1–4 | Include declarations for the concrete OS/library APIs used below. |
| 5–15 | Copy the representation to bytes without violating alignment or aliasing. |


## Line-by-line operations

Read each operation alongside the surrounding condition/loop in the numbered
listing below. If a line contains several operations, each appears separately.
The immediate `if`, return, or cleanup branch determines what happens on failure.

| Line | Operation | Explanation |
|---|---|---|
| 8 | `htons` | Encode a 16-bit port in network byte order. |
| 10 | `memcpy` | Copy an exact byte count; valid capacities and nonoverlapping ranges are required. |
| 13 | `ntohs` | Decode a network-order 16-bit value for host computation. |

## Numbered source

The numbers are annotations, not part of the C file. Build the linked source.

```text
  1  #include <arpa/inet.h>
  2  #include <stdint.h>
  3  #include <stdio.h>
  4  #include <string.h>
  5  /* Step: Copy the representation to bytes without violating alignment or aliasing. */
  6  int main(void) {
  7      uint16_t host = 9000;
  8      uint16_t network = htons(host);
  9      unsigned char bytes[sizeof network];
 10      memcpy(bytes, &network, sizeof bytes);
 11      printf("port=%u wire=%02x %02x roundtrip=%u\n",
 12             (unsigned)host, (unsigned)bytes[0], (unsigned)bytes[1],
 13             (unsigned)ntohs(network));
 14      return 0;
 15  }
```

## What if it fails?

Inspect each signed return before treating it as a byte count. Failed acquisition
must not be used as a descriptor. After ownership is acquired, every exit path
must close or explicitly transfer that resource. The source shows whether an
error ends the entire program, only a peer, or one retry attempt.

No network connection is created. memcpy copies representation without alignment assumptions.

## Modify it deliberately

Change the input to 443 and explain the resulting 01 bb bytes.

Refer to the [API reference](../socket-api.md) and
[protocol limits](../protocols-and-limits.md), then run a relevant integration
check and record the observation. Do not claim that merely compiling proves the
protocol works.
