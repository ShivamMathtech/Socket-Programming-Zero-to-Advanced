# Build and run catalogue

All commands run from the repository root. `make -j2` builds every target;
`make list` lists target/source mappings. `programs.json` is the machine-readable
manifest. C sources use only the standard library/POSIX/Linux facilities.

The direct commands below do the same compilation as Make. Run `mkdir -p build`
first. Start a matching server before running a network client; do not run two
listeners on the same port simultaneously. The common TCP echo clients are not
interchangeable with the framed client unless the server uses that framing.

| Target | Source | Walkthrough |
|---|---|---|
| byte_order | [00-prerequisites/byte-order.c](../00-prerequisites/byte-order.c) | [Explain](walkthroughs/byte_order.md) |
| socket_info | [01-socket-fundamentals/examples/socket-info.c](../01-socket-fundamentals/examples/socket-info.c) | [Explain](walkthroughs/socket_info.md) |
| tcp_server | [02-tcp-server/server.c](../02-tcp-server/server.c) | [Explain](walkthroughs/tcp_server.md) |
| tcp_client | [03-tcp-client/client.c](../03-tcp-client/client.c) | [Explain](walkthroughs/tcp_client.md) |
| frame_server | [04-tcp-client-server/server.c](../04-tcp-client-server/server.c) | [Explain](walkthroughs/frame_server.md) |
| frame_client | [04-tcp-client-server/client.c](../04-tcp-client-server/client.c) | [Explain](walkthroughs/frame_client.md) |
| udp_server | [05-udp-programming/udp-server.c](../05-udp-programming/udp-server.c) | [Explain](walkthroughs/udp_server.md) |
| udp_client | [05-udp-programming/udp-client.c](../05-udp-programming/udp-client.c) | [Explain](walkthroughs/udp_client.md) |
| udp_broadcast | [05-udp-programming/broadcast.c](../05-udp-programming/broadcast.c) | [Explain](walkthroughs/udp_broadcast.md) |
| iterative_server | [06-multiple-client-server/iterative-server/server.c](../06-multiple-client-server/iterative-server/server.c) | [Explain](walkthroughs/iterative_server.md) |
| fork_server | [06-multiple-client-server/fork-server/server.c](../06-multiple-client-server/fork-server/server.c) | [Explain](walkthroughs/fork_server.md) |
| thread_server | [06-multiple-client-server/thread-server/server.c](../06-multiple-client-server/thread-server/server.c) | [Explain](walkthroughs/thread_server.md) |
| fork_demo | [07-concurrent-programming/processes/fork-demo.c](../07-concurrent-programming/processes/fork-demo.c) | [Explain](walkthroughs/fork_demo.md) |
| thread_demo | [07-concurrent-programming/threads/thread-demo.c](../07-concurrent-programming/threads/thread-demo.c) | [Explain](walkthroughs/thread_demo.md) |
| counter | [07-concurrent-programming/synchronization/counter.c](../07-concurrent-programming/synchronization/counter.c) | [Explain](walkthroughs/counter.md) |
| select_server | [08-io-multiplexing/select/server.c](../08-io-multiplexing/select/server.c) | [Explain](walkthroughs/select_server.md) |
| poll_server | [08-io-multiplexing/poll/server.c](../08-io-multiplexing/poll/server.c) | [Explain](walkthroughs/poll_server.md) |
| epoll_server | [08-io-multiplexing/epoll/server.c](../08-io-multiplexing/epoll/server.c) | [Explain](walkthroughs/epoll_server.md) |
| connect_timeout | [09-advanced-sockets/blocking-vs-nonblocking/connect.c](../09-advanced-sockets/blocking-vs-nonblocking/connect.c) | [Explain](walkthroughs/connect_timeout.md) |
| socket_options | [09-advanced-sockets/socket-options/options.c](../09-advanced-sockets/socket-options/options.c) | [Explain](walkthroughs/socket_options.md) |
| receive_timeout | [09-advanced-sockets/timeouts/timeout.c](../09-advanced-sockets/timeouts/timeout.c) | [Explain](walkthroughs/receive_timeout.md) |
| broken_pipe | [09-advanced-sockets/error-handling/broken-pipe.c](../09-advanced-sockets/error-handling/broken-pipe.c) | [Explain](walkthroughs/broken_pipe.md) |
| ipv6_server | [09-advanced-sockets/ipv6/server.c](../09-advanced-sockets/ipv6/server.c) | [Explain](walkthroughs/ipv6_server.md) |
| ipv6_client | [09-advanced-sockets/ipv6/client.c](../09-advanced-sockets/ipv6/client.c) | [Explain](walkthroughs/ipv6_client.md) |
| tcp_chat_server | [11-real-world-projects/tcp-chat/server.c](../11-real-world-projects/tcp-chat/server.c) | [Explain](walkthroughs/tcp_chat_server.md) |
| tcp_chat_client | [11-real-world-projects/tcp-chat/client.c](../11-real-world-projects/tcp-chat/client.c) | [Explain](walkthroughs/tcp_chat_client.md) |
| udp_chat | [11-real-world-projects/udp-chat/peer.c](../11-real-world-projects/udp-chat/peer.c) | [Explain](walkthroughs/udp_chat.md) |
| multi_chat | [11-real-world-projects/multi-client-chat/server.c](../11-real-world-projects/multi-client-chat/server.c) | [Explain](walkthroughs/multi_chat.md) |
| file_sender | [11-real-world-projects/file-transfer/sender.c](../11-real-world-projects/file-transfer/sender.c) | [Explain](walkthroughs/file_sender.md) |
| file_receiver | [11-real-world-projects/file-transfer/receiver.c](../11-real-world-projects/file-transfer/receiver.c) | [Explain](walkthroughs/file_receiver.md) |
| http_server | [11-real-world-projects/simple-http-server/server.c](../11-real-world-projects/simple-http-server/server.c) | [Explain](walkthroughs/http_server.md) |

## byte_order

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 00-prerequisites/byte-order.c -o build/byte_order -pthread
./build/byte_order
```

## socket_info

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 01-socket-fundamentals/examples/socket-info.c -o build/socket_info -pthread
./build/socket_info
```

## tcp_server

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 02-tcp-server/server.c -o build/tcp_server -pthread
./build/tcp_server 9000
```

## tcp_client

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 03-tcp-client/client.c -o build/tcp_client -pthread
./build/tcp_client 127.0.0.1 9000 "hello"
```

## frame_server

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 04-tcp-client-server/server.c common/net.c -o build/frame_server -pthread
./build/frame_server 9000
```

## frame_client

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 04-tcp-client-server/client.c common/net.c -o build/frame_client -pthread
./build/frame_client 127.0.0.1 9000
```

## udp_server

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 05-udp-programming/udp-server.c common/net.c -o build/udp_server -pthread
./build/udp_server 9001
```

## udp_client

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 05-udp-programming/udp-client.c common/net.c -o build/udp_client -pthread
./build/udp_client 127.0.0.1 9001 "hello"
```

## udp_broadcast

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 05-udp-programming/broadcast.c common/net.c -o build/udp_broadcast -pthread
./build/udp_broadcast 127.0.0.1 9001 "one local test datagram"
```

## iterative_server

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 06-multiple-client-server/iterative-server/server.c common/net.c -o build/iterative_server -pthread
./build/iterative_server 9000
```

## fork_server

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 06-multiple-client-server/fork-server/server.c common/net.c -o build/fork_server -pthread
./build/fork_server 9000
```

## thread_server

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 06-multiple-client-server/thread-server/server.c common/net.c -o build/thread_server -pthread
./build/thread_server 9000
```

## fork_demo

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 07-concurrent-programming/processes/fork-demo.c -o build/fork_demo -pthread
./build/fork_demo
```

## thread_demo

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 07-concurrent-programming/threads/thread-demo.c -o build/thread_demo -pthread
./build/thread_demo
```

## counter

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 07-concurrent-programming/synchronization/counter.c -o build/counter -pthread
./build/counter
```

## select_server

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 08-io-multiplexing/select/server.c common/net.c -o build/select_server -pthread
./build/select_server 9000
```

## poll_server

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 08-io-multiplexing/poll/server.c common/net.c -o build/poll_server -pthread
./build/poll_server 9000
```

## epoll_server

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 08-io-multiplexing/epoll/server.c common/net.c -o build/epoll_server -pthread
./build/epoll_server 9000
```

## connect_timeout

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 09-advanced-sockets/blocking-vs-nonblocking/connect.c common/net.c -o build/connect_timeout -pthread
./build/connect_timeout 127.0.0.1 9000
```

## socket_options

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 09-advanced-sockets/socket-options/options.c common/net.c -o build/socket_options -pthread
./build/socket_options
```

## receive_timeout

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 09-advanced-sockets/timeouts/timeout.c common/net.c -o build/receive_timeout -pthread
./build/receive_timeout
```

## broken_pipe

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 09-advanced-sockets/error-handling/broken-pipe.c common/net.c -o build/broken_pipe -pthread
./build/broken_pipe
```

## ipv6_server

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 09-advanced-sockets/ipv6/server.c common/net.c -o build/ipv6_server -pthread
./build/ipv6_server 9002
```

## ipv6_client

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 09-advanced-sockets/ipv6/client.c common/net.c -o build/ipv6_client -pthread
./build/ipv6_client ::1 9002 "hello"
```

## tcp_chat_server

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 11-real-world-projects/tcp-chat/server.c common/net.c common/chat.c -o build/tcp_chat_server -pthread
./build/tcp_chat_server 9000
```

## tcp_chat_client

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 11-real-world-projects/tcp-chat/client.c common/net.c common/chat.c -o build/tcp_chat_client -pthread
./build/tcp_chat_client 127.0.0.1 9000
```

## udp_chat

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 11-real-world-projects/udp-chat/peer.c common/net.c -o build/udp_chat -pthread
./build/udp_chat 9001 127.0.0.1 9002
```

## multi_chat

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 11-real-world-projects/multi-client-chat/server.c common/net.c -o build/multi_chat -pthread
./build/multi_chat 9000
```

## file_sender

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 11-real-world-projects/file-transfer/sender.c common/net.c -o build/file_sender -pthread
./build/file_sender 127.0.0.1 9000 README.md
```

## file_receiver

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 11-real-world-projects/file-transfer/receiver.c common/net.c -o build/file_receiver -pthread
./build/file_receiver 9000 received.bin
```

## http_server

```bash
cc -D_POSIX_C_SOURCE=200809L -Icommon -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror 11-real-world-projects/simple-http-server/server.c common/net.c -o build/http_server -pthread
./build/http_server 8080
```
