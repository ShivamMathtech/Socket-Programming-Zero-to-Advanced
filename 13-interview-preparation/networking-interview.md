# Networking interview questions

Answer aloud first. The worked answers are intentionally concise; follow with
one concrete example from the code and one relevant limitation.

## 1. TCP versus UDP?

TCP exposes an ordered reliable byte stream; UDP exposes datagrams without a delivery/order guarantee. Choose from application semantics, not a universal speed slogan.

## 2. What happens in a TCP handshake?

The peers exchange SYN, SYN+ACK and ACK to establish connection state and sequence-space information. The kernel handles this, while accept retrieves an available established peer for the application.

## 3. IP versus MAC address?

IP supports network-layer addressing/routing; a MAC address is a link-layer addressing concept on technologies such as Ethernet. Routing across networks changes link-layer context.

## 4. What does localhost mean?

A host name normally resolved through local system configuration. It may yield IPv4 and IPv6 candidates; a numeric IPv4-only client does not resolve it.

## 5. Why can many TCP connections use server port 443?

Their peer endpoints distinguish the established connections. The listener local port is not a unique identifier for every accepted socket.

## 6. What is TIME-WAIT?

A TCP state retained by an endpoint after some close sequences to handle delayed/retransmitted traffic safely. It is not the same thing as a leaked application descriptor.

## 7. What does DNS do for a socket client?

Resolves a name to candidate addresses. It does not prove a service is alive, choose an application framing format or automatically obey a later socket timeout.

## 8. Why can a large UDP datagram be problematic?

IP fragmentation or path limits can impair delivery, and UDP does not add recovery. Choose message sizes and application behavior with actual path constraints in mind.

## 9. Why is ping insufficient for service monitoring?

It probes ICMP reachability; the application port or protocol can fail independently. A service check should validate the actual protocol response within a budget.

## 10. Does TCP provide confidentiality?

No. Use a maintained secure transport implementation such as TLS when confidentiality/authentication are required; those features are outside these plain-socket examples.

## 11. What changes when an application is behind NAT?

The externally observed endpoint may be translated, and unsolicited inbound reachability depends on mapping/firewall policy. Local bind success does not establish public reachability.

## 12. How do you define a network measurement?

State endpoints, payload/protocol, concurrency, timing method, repetitions and loss/error handling. A local roundtrip is not a one-way network latency measurement.
