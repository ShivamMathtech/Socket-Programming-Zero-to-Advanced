# Socket API reference

Conventional C signatures below are teaching summaries. Include the appropriate
system header; refer to the local `man 2 NAME` / `man 3 NAME` page for the exact
platform contract. All example fragments belong inside a program with checked
return values; the linked course programs provide complete implementations.

Headers: `<sys/socket.h>` for core socket calls, `<arpa/inet.h>` for numeric
addresses/byte order, `<netdb.h>` for resolution, `<unistd.h>` for close,
`<sys/select.h>`, `<poll.h>`, `<sys/epoll.h>`, `<fcntl.h>`, `<sys/wait.h>` and
`<pthread.h>` for their respective APIs.

Unless stated otherwise, a system-call failure is -1 with errno. Read errno only
when the return value says an error occurred; unrelated successful calls need
not clear it. Retry EINTR only where the operation's semantics permit it. A
blocking helper and an event-loop state machine need different EAGAIN handling.

## socket

**Purpose:** Allocate an endpoint and a descriptor reference.

```c
int socket(int domain, int type, int protocol);
```

**Parameters:** domain is AF_INET/AF_INET6 here; type is SOCK_STREAM or SOCK_DGRAM; protocol 0 selects the normal compatible protocol.

**Return value:** Nonnegative descriptor on success; -1 with errno on failure.

**Typical usage and common mistakes:** Allocation does not connect. Descriptor 0 is valid. Assign an owner immediately.

Example operation (see full implementations for error handling):

```c
int fd = socket(AF_INET, SOCK_STREAM, 0);
```

## bind

**Purpose:** Choose the local address and transport port.

```c
int bind(int fd, const struct sockaddr *addr, socklen_t len);
```

**Parameters:** fd is the socket; addr points to a family-specific address; len is the size of that address structure.

**Return value:** 0 on success, -1 on failure.

**Typical usage and common mistakes:** Servers bind before listen or UDP receive. Use htons for ports. Loopback limits the listener to local traffic; INADDR_ANY is all local IPv4 interfaces.

Example operation (see full implementations for error handling):

```c
bind(fd, (struct sockaddr *)&address, sizeof address);
```

## listen

**Purpose:** Mark a stream socket as a passive listener.

```c
int listen(int fd, int backlog);
```

**Parameters:** fd is a bound stream socket; backlog requests pending-connection queue capacity, with OS-specific limits.

**Return value:** 0 on success, -1 on failure.

**Typical usage and common mistakes:** Not used with UDP. Backlog is not the maximum lifetime or active application-client count. The OS can cap it.

Example operation (see full implementations for error handling):

```c
listen(listener, 64);
```

## accept

**Purpose:** Retrieve one connected peer from a listening socket.

```c
int accept(int listener, struct sockaddr *addr, socklen_t *len);
```

**Parameters:** listener stays open; optional addr receives the peer address; initialize *len to its capacity, or pass NULL for both.

**Return value:** New connected descriptor on success; -1 on failure.

**Typical usage and common mistakes:** Send/recv on the new descriptor. A nonblocking listener may return EAGAIN. On Linux, set nonblocking on accepted sockets explicitly instead of assuming inheritance.

Example operation (see full implementations for error handling):

```c
int peer = accept(listener, NULL, NULL);
```

## connect

**Purpose:** Associate a socket with a peer; initiate active TCP establishment.

```c
int connect(int fd, const struct sockaddr *addr, socklen_t len);
```

**Parameters:** addr describes the remote endpoint. The OS usually selects an unbound client local endpoint.

**Return value:** 0 for synchronous success; -1 plus errno otherwise. Nonblocking TCP commonly gives EINPROGRESS.

**Typical usage and common mistakes:** After pending TCP establishment, wait then inspect SO_ERROR. UDP connect has no TCP handshake and provides no delivery guarantee.

Example operation (see full implementations for error handling):

```c
connect(fd, (struct sockaddr *)&peer, sizeof peer);
```

## send

**Purpose:** Offer bytes to a connected socket.

```c
ssize_t send(int fd, const void *buf, size_t len, int flags);
```

**Parameters:** buf holds at least len readable bytes; flags is usually 0 in this course.

**Return value:** Nonnegative byte count on success; -1 on failure. A stream count can be less than len.

**Typical usage and common mistakes:** Advance by the actual count; handle EINTR, EAGAIN and fatal errors differently. Success is local acceptance, not an application receipt. Ignore/handle SIGPIPE or use an appropriate per-call option.

Example operation (see full implementations for error handling):

```c
ssize_t n = send(fd, bytes + offset, remaining, 0);
```

## recv

**Purpose:** Read available bytes into a bounded buffer.

```c
ssize_t recv(int fd, void *buf, size_t len, int flags);
```

**Parameters:** buf has at least len writable bytes. Use a positive len when interpreting TCP EOF.

**Return value:** Positive valid-byte count; zero for orderly stream EOF when positive length requested, or a valid zero-length datagram; -1 on error.

**Typical usage and common mistakes:** A returned prefix is not automatically NUL-terminated. Normal recv need not fill the buffer. Read a protocol-defined count instead of assuming one call gives one message.

Example operation (see full implementations for error handling):

```c
ssize_t n = recv(fd, data, sizeof data, 0);
```

## sendto

**Purpose:** Send a datagram to an explicit destination.

```c
ssize_t sendto(int fd, const void *buf, size_t len, int flags, const struct sockaddr *dest, socklen_t destlen);
```

**Parameters:** dest/destlen identify the receiver; buf/len define one datagram.

**Return value:** Bytes sent, or -1 on error. A too-large datagram can fail with EMSGSIZE.

**Typical usage and common mistakes:** Do not use a stream-style partial-send loop to split one UDP message. Retries can duplicate datagrams, so reliability belongs to the application protocol.

Example operation (see full implementations for error handling):

```c
sendto(fd, data, count, 0, (struct sockaddr *)&peer, sizeof peer);
```

## recvfrom

**Purpose:** Receive one datagram and optionally its source address.

```c
ssize_t recvfrom(int fd, void *buf, size_t len, int flags, struct sockaddr *src, socklen_t *srclen);
```

**Parameters:** Initialize *srclen with address capacity on each call. buf/len bound receive storage.

**Return value:** Received byte count or -1 on error; zero is a valid empty datagram.

**Typical usage and common mistakes:** A short buffer discards the excess datagram payload. The source endpoint is not authenticated identity. UDP has no connection EOF equivalent.

Example operation (see full implementations for error handling):

```c
recvfrom(fd, data, sizeof data, 0, (struct sockaddr *)&source, &source_len);
```

## close

**Purpose:** Release one descriptor reference.

```c
int close(int fd);
```

**Parameters:** fd must be owned and open. Other references can keep the underlying socket alive.

**Return value:** 0 on success; -1 on failure.

**Typical usage and common mistakes:** Do not double-close. On Linux, retrying a failed close can close a reused descriptor; report it without blindly retrying. Other platforms have their own documented semantics.

Example operation (see full implementations for error handling):

```c
close(fd);
```

## shutdown

**Purpose:** Disable one or both communication directions while retaining the descriptor.

```c
int shutdown(int fd, int how);
```

**Parameters:** how is SHUT_RD, SHUT_WR or SHUT_RDWR.

**Return value:** 0 on success; -1 on failure.

**Typical usage and common mistakes:** SHUT_WR allows request EOF followed by reading a reply. It does not replace close for resource release. It operates on socket state shared by references.

Example operation (see full implementations for error handling):

```c
shutdown(fd, SHUT_WR);
```

## setsockopt

**Purpose:** Configure an option at a socket or protocol level.

```c
int setsockopt(int fd, int level, int option, const void *value, socklen_t length);
```

**Parameters:** level selects SOL_SOCKET/IPPROTO_TCP/IPPROTO_IPV6 etc.; option determines the required value type/size.

**Return value:** 0 on success; -1 on failure.

**Typical usage and common mistakes:** Pass a pointer and the correct byte size. Set SO_REUSEADDR before bind. SO_BROADCAST is needed for IPv4 broadcast; SO_KEEPALIVE is not application liveness.

Example operation (see full implementations for error handling):

```c
setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof one);
```

## getsockopt

**Purpose:** Read an effective option or pending socket error.

```c
int getsockopt(int fd, int level, int option, void *value, socklen_t *length);
```

**Parameters:** *length is capacity on entry and returned size on success. SO_ERROR uses int storage.

**Return value:** 0 on success; -1 on failure.

**Typical usage and common mistakes:** For nonblocking connect, check both getsockopt return and the integer SO_ERROR value. Do not assume returned buffer sizes equal requested sizes.

Example operation (see full implementations for error handling):

```c
getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &size);
```

## select

**Purpose:** Wait on bounded descriptor sets for read/write/exception readiness.

```c
int select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
```

**Parameters:** nfds is highest descriptor plus one. Sets are value-result. NULL timeout waits indefinitely; zero timeout polls.

**Return value:** Positive number of ready set bits, 0 timeout, -1 error.

**Typical usage and common mistakes:** Rebuild sets and timeout. FD_SET is invalid for negative or >=FD_SETSIZE values. Readability includes EOF; exception sets do not replace normal error handling.

Example operation (see full implementations for error handling):

```c
select(largest + 1, &reads, &writes, NULL, NULL);
```

## poll

**Purpose:** Wait on a descriptor/event array.

```c
int poll(struct pollfd *fds, nfds_t count, int timeout_ms);
```

**Parameters:** Each entry has fd, requested events and returned revents. A negative fd is ignored. -1 timeout waits indefinitely.

**Return value:** Number of entries with nonzero revents, 0 timeout, -1 error.

**Typical usage and common mistakes:** Check POLLERR/POLLNVAL/POLLHUP too. HUP may coexist with unread buffered data. Request POLLOUT only when application output exists.

Example operation (see full implementations for error handling):

```c
poll(items, item_count, -1);
```

## epoll_create

**Purpose:** Create a legacy Linux epoll instance.

```c
int epoll_create(int size);
```

**Parameters:** size must be positive; its historical sizing hint is ignored on modern Linux.

**Return value:** Epoll descriptor on success, -1 on failure.

**Typical usage and common mistakes:** Prefer epoll_create1 for flags. Close the epoll descriptor when finished; it is another owned resource.

Example operation (see full implementations for error handling):

```c
int ep = epoll_create(1);
```

## epoll_create1

**Purpose:** Create a Linux epoll instance with supported flags.

```c
int epoll_create1(int flags);
```

**Parameters:** flags is 0 or EPOLL_CLOEXEC. The latter closes the epoll descriptor on a successful exec.

**Return value:** Epoll descriptor or -1.

**Typical usage and common mistakes:** This course uses level triggering. EPOLL_CLOEXEC on the epoll descriptor does not automatically apply close-on-exec to all watched sockets.

Example operation (see full implementations for error handling):

```c
int ep = epoll_create1(EPOLL_CLOEXEC);
```

## epoll_ctl

**Purpose:** Add, update or remove interest in a descriptor.

```c
int epoll_ctl(int ep, int operation, int fd, struct epoll_event *event);
```

**Parameters:** operation is EPOLL_CTL_ADD/MOD/DEL; event supplies desired events and application data.

**Return value:** 0 on success, -1 on failure.

**Typical usage and common mistakes:** Keep event data lifetime valid. Enable EPOLLOUT for pending output, disable it when empty. Removing/closing and descriptor reuse require careful event-batch handling.

Example operation (see full implementations for error handling):

```c
epoll_ctl(ep, EPOLL_CTL_MOD, peer, &event);
```

## epoll_wait

**Purpose:** Retrieve readiness events from a Linux epoll instance.

```c
int epoll_wait(int ep, struct epoll_event *events, int maxevents, int timeout_ms);
```

**Parameters:** events has maxevents entries; maxevents > 0. -1 waits indefinitely.

**Return value:** Returned event count, 0 timeout, -1 error.

**Typical usage and common mistakes:** A ready event does not contain a complete application message. Level-triggered readiness can repeat. Edge triggering requires nonblocking progress/draining and a backpressure-aware rearm strategy.

Example operation (see full implementations for error handling):

```c
int count = epoll_wait(ep, ready, 65, -1);
```

## getaddrinfo

**Purpose:** Resolve presentation names/addresses into endpoint candidates.

```c
int getaddrinfo(const char *node, const char *service, const struct addrinfo *hints, struct addrinfo **result);
```

**Parameters:** Use a zeroed hints structure. ai_family/ai_socktype constrain results; service can be a numeric port.

**Return value:** 0 on success; an EAI_* error code otherwise.

**Typical usage and common mistakes:** Report gai_strerror(result_code), try candidates and freeaddrinfo the whole result list. Resolution can block; a socket connect deadline does not automatically time-bound DNS.

Example operation (see full implementations for error handling):

```c
int rc = getaddrinfo(host, port, &hints, &addresses);
```

## fcntl

**Purpose:** Inspect or modify descriptor/open-file-description flags.

```c
int fcntl(int fd, int operation, ...);
```

**Parameters:** F_GETFL retrieves file status flags; F_SETFL sets supported flags.

**Return value:** Operation-dependent nonnegative result or -1.

**Typical usage and common mistakes:** Get existing flags first, then OR O_NONBLOCK; do not accidentally remove other status flags. O_NONBLOCK is shared through the open file description.

Example operation (see full implementations for error handling):

```c
fcntl(fd, F_SETFL, old_flags | O_NONBLOCK);
```

## htons / ntohs / htonl / ntohl

**Purpose:** Convert unsigned 16- and 32-bit integers between host and network byte order.

```c
uint16_t htons(uint16_t x); uint16_t ntohs(uint16_t x); uint32_t htonl(uint32_t x); uint32_t ntohl(uint32_t x);
```

**Parameters:** Use 16-bit values for ports and 32-bit values for the course frame length.

**Return value:** Converted value; no failure code.

**Typical usage and common mistakes:** Do not double-convert or send raw structs as wire records. memcpy encodings into byte arrays when alignment is uncertain.

Example operation (see full implementations for error handling):

```c
uint32_t header = htonl(length);
```

## inet_pton / inet_ntop

**Purpose:** Convert numeric address text and network address bytes.

```c
int inet_pton(int family, const char *src, void *dst); const char *inet_ntop(int family, const void *src, char *dst, socklen_t size);
```

**Parameters:** family selects IPv4/IPv6; storage must match the family; text buffer needs adequate capacity.

**Return value:** inet_pton: 1 valid, 0 invalid text, -1 error. inet_ntop: dst on success, NULL on failure.

**Typical usage and common mistakes:** These do not resolve hostnames. IPv6 literals used as socket address text do not include URL square brackets.

Example operation (see full implementations for error handling):

```c
inet_pton(AF_INET, "127.0.0.1", &address.sin_addr);
```

## fork / waitpid

**Purpose:** Create a child process and reap process completion.

```c
pid_t fork(void); pid_t waitpid(pid_t pid, int *status, int options);
```

**Parameters:** fork has no args; waitpid(-1, ..., WNOHANG) checks any exited child without waiting.

**Return value:** fork: 0 in child, child PID in parent, -1 failure. waitpid: child PID, 0 for no available exit with WNOHANG, -1 error.

**Typical usage and common mistakes:** Close unused inherited descriptors in both processes. Reap even when no new client arrives. Ordinary memory writes do not update the other process.

Example operation (see full implementations for error handling):

```c
pid_t child = fork();
```

## pthread_create / pthread_join

**Purpose:** Start a thread and wait for a joinable thread to finish.

```c
int pthread_create(pthread_t *t, const pthread_attr_t *attr, void *(*start)(void *), void *arg); int pthread_join(pthread_t t, void **result);
```

**Parameters:** Keep arg storage valid until consumed; choose joinable or detached lifecycle explicitly.

**Return value:** 0 success, otherwise an error number directly.

**Typical usage and common mistakes:** Use strerror(code), not assumed errno. Do not pass an accept-loop variable address that another iteration can overwrite. Compile/link with -pthread.

Example operation (see full implementations for error handling):

```c
int code = pthread_create(&thread, NULL, worker, argument);
```

## pthread_mutex_lock / pthread_mutex_unlock

**Purpose:** Protect a shared critical section and establish synchronization.

```c
int pthread_mutex_lock(pthread_mutex_t *m); int pthread_mutex_unlock(pthread_mutex_t *m);
```

**Parameters:** Use the same initialized mutex for all accesses protected by that policy.

**Return value:** 0 success, otherwise a direct error code.

**Typical usage and common mistakes:** No global lock should be held over unbounded network I/O. volatile is not a substitute. Consistent lock ordering avoids cycles.

Example operation (see full implementations for error handling):

```c
pthread_mutex_lock(&lock);
```

## socketpair

**Purpose:** Create two locally connected sockets for controlled experiments.

```c
int socketpair(int domain, int type, int protocol, int pair[2]);
```

**Parameters:** This course uses AF_UNIX, SOCK_STREAM, 0; pair receives two descriptors.

**Return value:** 0 success, -1 failure.

**Typical usage and common mistakes:** Close both descriptors. This isolates timeout/EPIPE semantics but does not test IP routing or a TCP handshake.

Example operation (see full implementations for error handling):

```c
socketpair(AF_UNIX, SOCK_STREAM, 0, pair);
```

## Source navigation

See [program catalogue](program-catalogue.md), [numbered walkthroughs](walkthroughs/README.md),
and [primary references](references.md).
