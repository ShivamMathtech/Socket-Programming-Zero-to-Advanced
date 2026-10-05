# C readiness checklist


Before Chapter 01, make sure you can explain these expressions:

| C construct | Why it matters |
|---|---|
| `char buffer[4096]` | Allocates storage, not a guaranteed initialized string |
| `sizeof buffer` | Capacity for an array in its declaring scope |
| `sizeof pointer` | Size of a pointer, not the pointed-to allocation |
| `&length` | Passes the address of mutable output storage |
| `(struct sockaddr *)&address` | Uses the API's generic address pointer type |
| `ssize_t n` | Signed count that can represent -1 |
| `size_t used` | Unsigned storage/byte count after validating a return |
| `errno` | Error detail valid only after a reported failure |
| `goto done` | A clear centralized resource cleanup path in these C programs |

A successful receive does not append NUL. Use fwrite(buffer,1,n,stdout) for binary
bytes, or explicitly reserve and set a terminator before string operations.
Never cast a negative received count into an unsigned byte length.

**Readiness task:** write a small program that prints four bytes including an
embedded zero with fwrite. Compare with strlen on the same data and explain why
the lengths differ. You are ready when buffer storage and string meaning feel
like different concepts.
