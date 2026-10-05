# Client/server model


“Server” and “client” describe roles, not hardware categories. A server publishes
an endpoint and accepts/receives requests; a client initiates an exchange. Both
can send and receive once a TCP connection is established. One program can be a
server for one connection and a client for another.

For TCP, the listener is a rendezvous point. Each accepted peer gets its own
connected descriptor. For UDP, one bound descriptor can receive independent
messages from many senders without creating a per-peer accepted socket.

A request/response protocol must say who speaks first, what ends a request and
whether another request can follow. If both peers immediately block on recv,
neither has supplied bytes that can wake the other.

**Lab:** sketch the two terminal commands before running them. Circle the server
port in both. Label which process owns the listener and which owns each
connected descriptor. Then explain why two clients do not need to choose the
same local source port as the server.
