# Ports and endpoints


TCP and UDP use 16-bit port fields. A server commonly binds a stable port while a
client receives an ephemeral source port. The available ephemeral range and
privileged-bind policy are OS configuration details.

A connection is often described by protocol plus source/destination address and
port. Many accepted TCP connections can share one server local port because their
peer endpoints differ. A descriptor number is unrelated to the port number.

**Lab:** start `./build/iterative_server 9000`; inspect `ss -ltnp`. Run a client and
inspect `ss -tnp` during the exchange. The server local port is 9000; the client
source port is selected by the OS.

The course requires explicit numeric ports in 1..65535. Port 0 is useful for
asking the kernel to choose an ephemeral bind port, and the test harness uses
that mechanism, but the teaching C CLI intentionally rejects it so its startup
message always matches the chosen input port.

**Question:** can a UDP program bind port 9000 while the TCP example uses 9000?
Yes, because their transport-protocol port namespaces differ.
