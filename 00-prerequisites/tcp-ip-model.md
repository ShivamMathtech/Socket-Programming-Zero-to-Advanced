# TCP/IP model


The Internet protocol stack is commonly taught as application, transport,
Internet and link layers. It groups some OSI responsibilities together and is
closer to the protocols used in this course.

| Layer | Examples | Typical evidence |
|---|---|---|
| Application | Course framing, chat, HTTP subset | Parser logs and request bytes |
| Transport | TCP, UDP | Ports, stream state, retransmission/datagrams |
| Internet | IPv4, IPv6, ICMP | Addresses and routes |
| Link | Loopback, Ethernet, Wi-Fi | Interface state and local captures |

**Worked example:** your file-transfer sender opens an input file and makes framed
chunks. TCP moves their bytes in order; IP addresses the peer; a link carries
packets locally. The receiver still needs to check chunk lengths and completion.
TCP cannot know whether your application intended a zero-length end marker.

**Exercise:** why can ping succeed while connect fails? ICMP reachability does not
prove a TCP listener exists on the requested port or that TCP is allowed on that
path.
