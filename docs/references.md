# Primary references and further reading

Reference review date: **2026-10-05**. The course uses original explanations and
implementations; these sources define protocols or document platform APIs.
Installed manual pages are useful for checking your actual system version.

| Source | Use in this course |
|---|---|
| [RFC 9293 — TCP](https://www.rfc-editor.org/rfc/rfc9293.html) | TCP service and connection/state semantics |
| [RFC 768 — UDP](https://www.rfc-editor.org/rfc/rfc768.html) | UDP datagram protocol |
| [RFC 9110 — HTTP Semantics](https://www.rfc-editor.org/rfc/rfc9110.html) | Reading beyond the small HTTP teaching subset |
| [Linux man-pages: socket(7)](https://man7.org/linux/man-pages/man7/socket.7.html) | Linux socket overview and options |
| [Linux man-pages: recv(2)](https://man7.org/linux/man-pages/man2/recv.2.html) | Receive counts, EOF, flags and datagram truncation |
| [Linux man-pages: send(2)](https://man7.org/linux/man-pages/man2/send.2.html) | Send behavior and errors |
| [Linux man-pages: select(2)](https://man7.org/linux/man-pages/man2/select.2.html) | Descriptor sets and readiness |
| [Linux man-pages: poll(2)](https://man7.org/linux/man-pages/man2/poll.2.html) | Event arrays, hangup and returned flags |
| [Linux man-pages: epoll(7)](https://man7.org/linux/man-pages/man7/epoll.7.html) | Linux interest/ready lists and trigger modes |
| [Linux man-pages: connect(2)](https://man7.org/linux/man-pages/man2/connect.2.html) | Establishment and SO_ERROR completion checks |
| [Linux man-pages: close(2)](https://man7.org/linux/man-pages/man2/close.2.html) | Linux descriptor release and retry caveats |
| [Microsoft — Install WSL](https://learn.microsoft.com/en-us/windows/wsl/install) | Windows setup |

The reviewed core references were TCP, recv/send, socket options, select/poll,
epoll and WSL installation. The other links are primary-source reading pointers,
not claims that every referenced page was downloaded or every OS variation tested.
The complete API reference uses conventional teaching signatures; consult the
installed headers for compilation and the platform manual for detailed contracts.

Useful local commands:

```bash
man 2 socket
man 2 accept
man 2 recv
man 2 poll
man 7 epoll
man 3 getaddrinfo
man 3 pthread_create
```

If manual pages are missing on Ubuntu, install `man-db manpages-dev`.
