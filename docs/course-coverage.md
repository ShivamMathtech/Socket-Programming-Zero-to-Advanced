# Specification coverage map

| Requested area | Implemented location |
|---|---|
| Polished README, badges, roadmap, progress tracker | [README](../README.md) |
| Beginner Linux and Windows setup | [Start here](../START_HERE.md) |
| Networking/IP/ports/OSI/TCP-IP/client-server concepts | [Chapter 00](../00-prerequisites/README.md) and its subject notes |
| Socket lifecycle and address structures | [Chapter 01](../01-socket-fundamentals/README.md) |
| Standalone TCP server/client | [Chapter 02](../02-tcp-server/README.md), [Chapter 03](../03-tcp-client/README.md) |
| Framing, partial I/O and interactive records | [Chapter 04](../04-tcp-client-server/README.md) |
| UDP echo/chat/broadcast | [Chapter 05](../05-udp-programming/README.md), [UDP chat](../11-real-world-projects/udp-chat/README.md) |
| Iterative/fork/thread progression | [Chapter 06](../06-multiple-client-server/README.md) |
| Process/thread memory and synchronization | [Chapter 07](../07-concurrent-programming/README.md) |
| select/poll/epoll | [Chapter 08](../08-io-multiplexing/README.md) |
| Blocking/nonblocking/options/timeouts/errors/IPv6 | [Chapter 09](../09-advanced-sockets/README.md) |
| ss/netstat/lsof/tcpdump/Wireshark/ping/ip | [Chapter 10](../10-network-debugging/README.md) |
| Seven working projects | [Chapter 11](../11-real-world-projects/README.md) |
| Exam questions, MCQs, viva, output/debug/programming | [Chapter 12](../12-exam-preparation/README.md) |
| Interviews and design reasoning | [Chapter 13](../13-interview-preparation/README.md) |
| Purpose/syntax/parameters/returns/usage/mistakes/examples | [API reference](socket-api.md) |
| Numbered real source explanations | [34 walkthroughs](walkthroughs/README.md) |
| Exact per-program compiler commands | [Catalogue](program-catalogue.md) |
| Contribution and behavior policy | [Contributing](../CONTRIBUTING.md), [Code of conduct](../CODE_OF_CONDUCT.md) |
| MIT license | [License](../LICENSE) |
| Real checks and stated limits | [Verification](../VERIFICATION.md) |

Every main chapter follows the requested learning template and has a separate
worked-solutions page with beginner, intermediate and advanced tasks. The seven
projects include problem statements, requirements, architecture, workflow,
implementation references, compile/run commands, tests and improvements.

Some projects intentionally reuse canonical chapter sources; their local
Makefiles build those implementations. Further mini-project modifications are
learning exercises with solutions/acceptance guidance, distinct from the seven
supplied reference projects. There are no invented build statistics, screenshots
or performance results.
