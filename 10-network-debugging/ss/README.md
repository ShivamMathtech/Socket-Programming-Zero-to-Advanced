# ss socket inspection

`ss` is the main socket-table tool used here.

```bash
ss -ltnp
ss -lunp
ss -tnp
ss -tan
```

`-l` selects listeners; `-t` TCP; `-u` UDP; `-n` numeric addresses; `-p` process
information; `-a` includes more states. Process details for other users may need
privileges. Start a server before looking for its listener.

Watch LISTEN while the server awaits a peer, and ESTABLISHED during an active
exchange. TIME-WAIT after connection close is transport bookkeeping, not proof
that your application leaked an open descriptor.
