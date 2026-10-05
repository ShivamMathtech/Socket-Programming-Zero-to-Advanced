# Socket address structures


| Type | Role |
|---|---|
| sockaddr | Generic API pointer interface |
| sockaddr_in | IPv4 family, port and address |
| sockaddr_in6 | IPv6 family, port, address and related fields |
| sockaddr_storage | Storage with sufficient size/alignment for address families |
| socklen_t | Address/option byte length used by socket APIs |

Zero-initialize structures before setting fields. For IPv4, set sin_family,
sin_port in network byte order, and sin_addr from inet_pton or a correctly
converted constant. Do not store a hostname string in sin_addr.

Calls returning an address need capacity on entry. Reset a recvfrom source length
before every receive. For generic clients use getaddrinfo rather than guessing a
structure size; iterate its candidate addresses and free the result list.

**Exercise:** why does casting sockaddr_in* to sockaddr* not convert IPv4 to IPv6?
It changes the pointer type used by the C API; the structure's family and contents
still describe IPv4.
