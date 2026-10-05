# lsof, ip, ping and strace

```bash
lsof -nP -iTCP:9000
ip -brief address
ip route
ip -6 route
ping -c 2 127.0.0.1
strace -f -e trace=network ./build/tcp_server 9000
```

- **lsof:** identifies the process holding a socket; inspect before stopping it.
- **ip:** reveals actual interface addresses and routes; names/subnets vary.
- **ping:** probes ICMP reachability; it cannot verify a specific TCP listener.
- **strace:** reveals calls, arguments and returned errors. `-f` follows child
  activity when investigating the fork server.

Tracing changes timing. Use it to understand behavior, not to publish unaffected
performance results. Pair a trace with source line numbers in the walkthroughs.
