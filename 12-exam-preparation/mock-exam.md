# Mock practical exam — 100 points

Suggested format: 90 minutes. Use your local compiler and manual pages, but do not
open the course solutions until you have written your answers.

## Part A — Concepts, 20 points

1. Explain socket/port/descriptor and accept ownership. (5)
2. Explain TCP framing versus UDP boundaries. (5)
3. Distinguish EOF, EAGAIN and EINTR. (5)
4. Explain send success versus application acknowledgement. (5)

## Part B — Reasoning, 20 points

1. Draw the state needed to preserve a partial frame and partial reply. (10)
2. Explain why a slow reader must not grow an unbounded queue. (10)

## Part C — Debugging, 20 points

Find and fix one binary-string bug, one thread-argument race and one permanent
writable-interest busy loop. Explain the failure mechanism before proposing a
change. Use the three corresponding problems in practice-problems.md.

## Part D — Practical, 40 points

Build a loopback framed echo service or extend the supplied one with a one-byte
message type. Bound every input; define unknown-type behavior; show one normal
exchange, a fragmented header and a premature disconnect.

## Marking guide

- Part A: award points for correct distinctions, not just names. Relevant worked
  answers are in [important questions](important-questions.md).
- Part B: parser stage, expected body length, accumulated count, output offset,
  incoming-EOF flag and capacity policy should be explicit.
- Part C: identify the root cause (8), propose correct lifetime/count/interest
  policy (8), and describe an executable regression check (4).
- Part D: valid wire protocol (8), correct stream I/O (10), length bounds (6),
  cleanup/error handling (6), the three demonstrations (6), explanation (4).

A program that compiles but trusts a remote length does not receive full
correctness credit. A test log should show actual commands and observed behavior.
