# Scalability and backpressure

| Approach | State placement | Main trade-off |
|---|---|---|
| Iterative | One handler stack | Simple; idle client delays later service |
| Fork per client | Separate process stacks/address spaces | Isolation, descriptor inheritance and process cleanup |
| Thread per client | Thread stacks, shared memory | Easy blocking flow, synchronization and worker limits |
| select | Explicit connection objects plus bit sets | Portable pattern, FD_SETSIZE descriptor-number restriction |
| poll | Explicit objects plus fd arrays | Flexible descriptors, array scanning |
| epoll | Explicit objects plus registered kernel interest | Linux-specific, ready batches and interest-state management |

This table is architectural, not a measured speed ranking. A fair comparison
holds protocol, payload sizes, active/idle ratios, client count and hardware
constant and records errors as well as latency/throughput.

A fast sender and slow receiver create an output backlog. Without a bound, memory
grows until failure. The echo reactors pause reading at their output-capacity
limit. TCP's flow control then helps propagate pressure back to the sender. Group
chat cannot always pause every sender for one slow audience member, so it drops
that receiver when its queue would overflow.

Readiness loops solve waiting, not CPU scheduling. An expensive computation
inside the loop can delay every peer; bounded worker offload is a further design
step. Avoid unbounded task queues there too.
