# Networking basics


A host can have several interfaces and addresses. Two programs on the same host
can communicate through loopback without a cable or Wi-Fi. Two remote hosts need
compatible addressing and a route; a switch/router/firewall may sit between them.

| Unit | Meaning | Example |
|---|---|---|
| Application message | Your protocol's semantic record | A chat line |
| Transport data | TCP byte stream or UDP datagram | Framed request bytes |
| IP packet | Network-layer delivery unit | IPv4 source/destination |
| Link frame | Local-link delivery unit | Ethernet frame |

Do not use these names interchangeably. TCP segment boundaries need not match
application send boundaries, and captures can be affected by offload behavior.

**Lab:** run `ip -brief address` and identify `lo`. Run `ping -c 2 127.0.0.1` if
ping is installed. Explain why this checks a local ICMP path, not your TCP server.

**Self-check:** if the server listens on 127.0.0.1, can another laptop connect by
using its own 127.0.0.1? No: that address points back to that other laptop.
