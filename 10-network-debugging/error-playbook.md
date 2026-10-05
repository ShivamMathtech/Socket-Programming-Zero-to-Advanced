# Error playbook

Always record the failed API and errno immediately. Fix the cause and reproduce
the smallest exchange; do not erase unrelated state to make an error disappear.

| Error / symptom | Why it can happen | Identify it | Fix and prevention |
|---|---|---|---|
| Address already in use / port occupied | Another listener or a bind-reuse/state conflict | `ss -ltnp`, `lsof -iTCP:9000` | Stop your own old server or choose another matching port; configure SO_REUSEADDR before bind for documented reuse cases |
| Connection refused | Reachable endpoint rejects establishment, commonly no listener | Verify server output and exact address/port with ss | Start the correct server first; check protocol and address family |
| Connection reset by peer | TCP peer/path reports a reset, possibly during abrupt shutdown | Inspect recv/send stage, peer logs and RST in capture | Handle as failed exchange; close peer, define whether retry is safe |
| Broken pipe | Writing after the socket can no longer send | Check send error and SIGPIPE policy | Ignore/handle SIGPIPE and handle EPIPE; recreate protocol session only when appropriate |
| Permission denied | Bind policy, file permissions or broadcast permission | Identify whether bind, open or sendto failed | Use an unprivileged lab port, correct file permissions or SO_BROADCAST; do not run the whole app as root by default |
| Bind failed / address unavailable | Local address not assigned or wrong family/structure | `ip -brief address`, check inet_pton and family | Bind an existing local address, or loopback for local labs |
| Connection timeout | No timely establishment; filtering/routing/unresponsive endpoint | Route, endpoint and handshake evidence | Use a defined deadline, fix path or endpoint; avoid unlimited retries |
| Receive timeout / would block | Peer idle or nonblocking socket has no data yet | Distinguish timeout mode from O_NONBLOCK | Resume on readiness or end the request per deadline policy |
| Connected but no reply | Parser waiting for delimiter/length/body, ordering mismatch | Compare wire format and syscall byte counts | Fix framing; do not assume one send equals one request |
| Server CPU busy while idle | Writable interest registered with no queued output | Trace repeated poll/epoll readiness | Register output interest only while output is pending |

For “port occupied,” inspect process ownership before stopping anything. The
course intentionally binds loopback/high ports, so a firewall change is not the
first response to an ordinary local setup mismatch.
