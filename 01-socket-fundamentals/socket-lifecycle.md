# Socket lifecycle and ownership


A TCP listening path is socket, optional configuration, bind, listen and repeated
accept. A connected client path is socket, connect, data exchange and close.
Accepted sockets enter the data-exchange phase without a separate client-side
connect call in the server application.

Keep a resource ledger:

| Resource | Created by | Closed by |
|---|---|---|
| Listener | Main server | Main server |
| Accepted peer | accept loop | Handler/worker after ownership transfer |
| Client socket | Client main | Client cleanup path |
| epoll instance | Reactor startup | Reactor cleanup path |

Every failure after allocation needs a cleanup route. Initialize descriptors to
-1 before operations so a shared cleanup block can distinguish resources that
were never acquired. On Linux, report close errors without a blind retry.
