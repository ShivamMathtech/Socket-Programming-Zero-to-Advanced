# IP addresses


IPv4 addresses have 32 bits; IPv6 addresses have 128 bits. Dotted decimal and
colon-separated hexadecimal are text representations. Socket APIs ultimately
need binary address structures and explicit address families.

| Address | Meaning for this course |
|---|---|
| 127.0.0.1 | IPv4 loopback endpoint |
| ::1 | IPv6 loopback endpoint |
| 0.0.0.0 as a bind address | All local IPv4 interfaces |
| localhost | Name resolved according to host configuration |

A wildcard bind address is not the remote destination you should give a client.
Use the server's actual reachable interface address or loopback as appropriate.
A name can resolve to multiple candidates; the generic helper tries them.

A CIDR prefix describes how many leading bits identify a network. Routing selects
a next hop based on the destination; it does not know your application framing.
NAT can rewrite endpoint addressing, which is one reason a local laboratory is
simpler than exposing a service across routers.

**Lab:** compare `ip route`, `ip -6 route` and `ip -brief address`. Commands report
real local configuration; this course does not assume a particular LAN subnet.
