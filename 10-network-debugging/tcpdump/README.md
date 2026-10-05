# tcpdump laboratory

Install tcpdump if needed. In terminal A start a capture before running the
client/server exchange in other terminals:

```bash
sudo tcpdump -i lo -nn 'tcp port 9000'
```

Stop with Ctrl-C after the exchange. `-i lo` observes loopback; `-nn` avoids name
and service lookups. Look for SYN, SYN/ACK, ACK, data and FIN/RST according to the
actual exchange. Retransmissions and packet grouping depend on the environment.

To save your own capture:

```bash
sudo tcpdump -i lo -nn -w lab.pcap 'tcp port 9000'
```

Run an exchange, stop capture, then inspect that pcap with Wireshark. Do not infer
send-call boundaries from packet boundaries. Captures contain payload data; use
the course's nonsensitive local messages.
