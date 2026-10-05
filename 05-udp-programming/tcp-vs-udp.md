# TCP versus UDP


| Property | TCP | UDP |
|---|---|---|
| Application abstraction | Ordered byte stream | Independent datagrams |
| Peer setup | Transport connection | No TCP-style handshake |
| Message boundaries | Application must define | Preserved per datagram |
| Delivery/retransmission | Transport handles ordered reliable stream delivery | Application must choose any retry policy |
| Read of zero bytes | EOF for positive-length stream receive | May be an empty datagram |
| Oversized receive buffer issue | Unread bytes remain in stream | Excess datagram bytes can be discarded |
| Multicast/broadcast | Not a stream connection feature | Supported through appropriate IP facilities |
| Course example | Framed echo/file transfer | Datagram echo/chat |

Neither protocol is universally “faster.” Application latency, congestion,
reliability requirements, packet sizes and loss behavior determine suitability.
UDP applications still need to behave responsibly under congestion; removing TCP
does not remove that engineering obligation.

**Reasoning exercise:** choose TCP for a simple reliable file transfer. Choose UDP
for a lesson about independent sensor readings, then explicitly decide whether
missing old readings should be retried or superseded by new readings.
