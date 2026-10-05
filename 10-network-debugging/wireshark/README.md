# Wireshark laboratory

Open a capture you created with tcpdump, or choose the interface carrying your
own lab traffic. A capture filter and a display filter are different syntaxes.
For already captured traffic, use display filters:

```text
tcp.port == 9000
udp.port == 9001
tcp.flags.syn == 1
```

Find the handshake and inspect the client/server port pair. Use **Follow TCP
Stream** to inspect reconstructed application bytes, then compare with individual
packet segments. For the framed protocol, locate the four-byte length before the
body. Do not expect the GUI to infer the custom application protocol automatically.

On Windows with WSL, capturing in the Ubuntu environment on lo is often simpler
for this lab than guessing a Windows virtual adapter. The actual topology controls
which interface can observe which packets.
