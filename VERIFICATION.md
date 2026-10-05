# Verification report — version 1.0.0

Validation date: **2026-10-05**. These are results from the supplied Linux workspace,
not a claim that a remote CI service or the reader's machine has been tested.

## Recorded checks

| Check | Result |
|---|---|
| C compilation | All 31 executables compiled with C11, -Wall, -Wextra, -Wpedantic and -Werror |
| Integration suite | All 22 test methods passed; methods include subcases for multiple server implementations and CLI validation |
| UndefinedBehaviorSanitizer | All 31 executables rebuilt with instrumentation; the same 22 test methods passed |
| IPv6 loopback | IPv6 test ran successfully in this environment |
| Project-local builds | All seven project Makefiles completed successfully |
| Compiler-command catalogue | All 31 documented direct compiler commands passed syntax checks |
| Local documentation | Relative file/fragment links, code-fence balance and all 14 chapter templates checked |

See the actual [build log](verification/build.log), [integration log](verification/tests.log),
[sanitizer log](verification/ubsan.log), [documentation log](verification/docs.log)
and [environment](verification/environment.txt). Run `make test check-docs` locally
to reproduce the functional checks after building.

## What the tests exercise

- Standalone TCP client/server and repeated binary echo sessions.
- Fork/thread/select/poll/epoll concurrency with an idle client present.
- Progress of normal peers while another peer is a slow reader.
- Fragmented, coalesced, empty, binary, oversized and truncated framed messages.
- UDP echo including an empty datagram; the broadcast sender's local unicast path.
- Nonblocking connect success/refusal, local timeout behavior and handled EPIPE.
- IPv6 echo, both TCP chat directions and a UDP chat peer.
- Group chat split/coalesced lines and oversized input rejection.
- Empty/binary file transfer, local SHA-256 comparison, overwrite refusal and
  removal of a partial output after handled truncation.
- HTTP GET/HEAD/routes/errors and fragmented/oversized headers.
- Invalid command-line invocation handling.

## Limits of verification

The Windows/WSL instructions were checked against Microsoft's installation guide;
WSL installation itself was not performed in this Linux workspace. Actual LAN
broadcast reception is topology-dependent and remains a manual lab. Tests use
loopback and are not WAN, packet-loss, fuzzing, load-capacity or production
security certification. Sanitizer coverage is limited to exercised paths; the
supplied sanitizer target uses UBSan, not a claim of full memory/race analysis.

Mermaid diagrams use simple flowchart, sequence and state syntax. Their source
files and Markdown fences were inspected; an external Mermaid renderer was not
available in the build environment, so visual rendering was not independently
executed. GitHub or a Mermaid-capable Markdown viewer can display them; plain
Markdown viewers retain readable diagram source.

The binaries are rebuilt by the learner and are excluded from the ZIP. The
provided CI workflow is ready for a future repository but has not run on a remote
GitHub repository. Project limitations are explicit in the
[protocol guide](docs/protocols-and-limits.md).
