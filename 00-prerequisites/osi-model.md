# OSI model as a reasoning aid


| Layer | Main concern | Connection to the course |
|---|---|---|
| 7 Application | Meaning of requests/responses | Chat, file and HTTP protocols |
| 6 Presentation | Representation | Explicit byte encoding; TLS is beyond scope |
| 5 Session | Conversation organization | Application state and message ordering |
| 4 Transport | End-to-end transport | TCP and UDP |
| 3 Network | Addressing/routing | IPv4 and IPv6 |
| 2 Data link | Local delivery | Ethernet/Wi-Fi frames |
| 1 Physical | Signals | Cable/radio medium |

This is a conceptual model. Real protocols do not always fit one layer perfectly,
and the BSD/POSIX socket API is not an implementation of seven separate layers.
Use the model to ask better debugging questions, not to force every syscall into
an artificial box.

**Exercise:** classify “wrong port,” “malformed frame length,” and “unplugged link.”
The first concerns the selected transport endpoint, the second application
representation/protocol, and the third physical/link connectivity.
