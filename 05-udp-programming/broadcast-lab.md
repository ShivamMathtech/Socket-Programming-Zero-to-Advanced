# Manual IPv4 broadcast lab


The broadcast sender emits one datagram after enabling SO_BROADCAST. Automated
checks exercise its send path against loopback unicast; a real broadcast requires
a suitable local interface/network and is a manual lab.

1. Use `ip -4 address` to find the actual broadcast address of your lab interface.
   Do not assume another person's subnet or broadcast value.
2. For this one experiment, copy udp-server.c and change its bind host from
   `127.0.0.1` to `0.0.0.0`. Compile the copy as a separate binary using common/net.c.
3. Run that receiver on port 9001. On a host in the same lab broadcast domain,
   run the sender with the actual broadcast address:

   ```bash
   ./build/udp_broadcast <your-lab-broadcast-address> 9001 "hello lab"
   ```

4. Observe the receiver with tcpdump or add an explicit received-length print.
   The sender does not wait for replies; its “Sent” message means local send
   success, not proof that any receiver processed it.

Stop the receiver afterward. Firewalls, virtual switches and WSL networking can
block or contain broadcast. IPv6 uses multicast instead; this example implements
IPv4 broadcast only. The placeholder above must be replaced before running.
