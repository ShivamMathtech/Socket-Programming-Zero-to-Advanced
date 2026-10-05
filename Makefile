CC ?= cc
CPPFLAGS += -D_POSIX_C_SOURCE=200809L -Icommon
CFLAGS ?= -std=c11 -O2 -g -Wall -Wextra -Wpedantic -Werror
LDLIBS += -pthread
PROGRAMS := byte_order socket_info tcp_server tcp_client frame_server frame_client udp_server udp_client udp_broadcast iterative_server fork_server thread_server fork_demo thread_demo counter select_server poll_server epoll_server connect_timeout socket_options receive_timeout broken_pipe ipv6_server ipv6_client tcp_chat_server tcp_chat_client udp_chat multi_chat file_sender file_receiver http_server
BINS := $(addprefix build/,$(PROGRAMS))
.PHONY: all clean test check-docs sanitize list
all: $(BINS)
build:
	mkdir -p build
build/byte_order: 00-prerequisites/byte-order.c  | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 00-prerequisites/byte-order.c -o $@ $(LDFLAGS) $(LDLIBS)
build/socket_info: 01-socket-fundamentals/examples/socket-info.c  | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 01-socket-fundamentals/examples/socket-info.c -o $@ $(LDFLAGS) $(LDLIBS)
build/tcp_server: 02-tcp-server/server.c  | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 02-tcp-server/server.c -o $@ $(LDFLAGS) $(LDLIBS)
build/tcp_client: 03-tcp-client/client.c  | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 03-tcp-client/client.c -o $@ $(LDFLAGS) $(LDLIBS)
build/frame_server: 04-tcp-client-server/server.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 04-tcp-client-server/server.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/frame_client: 04-tcp-client-server/client.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 04-tcp-client-server/client.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/udp_server: 05-udp-programming/udp-server.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 05-udp-programming/udp-server.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/udp_client: 05-udp-programming/udp-client.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 05-udp-programming/udp-client.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/udp_broadcast: 05-udp-programming/broadcast.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 05-udp-programming/broadcast.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/iterative_server: 06-multiple-client-server/iterative-server/server.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 06-multiple-client-server/iterative-server/server.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/fork_server: 06-multiple-client-server/fork-server/server.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 06-multiple-client-server/fork-server/server.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/thread_server: 06-multiple-client-server/thread-server/server.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 06-multiple-client-server/thread-server/server.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/fork_demo: 07-concurrent-programming/processes/fork-demo.c  | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 07-concurrent-programming/processes/fork-demo.c -o $@ $(LDFLAGS) $(LDLIBS)
build/thread_demo: 07-concurrent-programming/threads/thread-demo.c  | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 07-concurrent-programming/threads/thread-demo.c -o $@ $(LDFLAGS) $(LDLIBS)
build/counter: 07-concurrent-programming/synchronization/counter.c  | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 07-concurrent-programming/synchronization/counter.c -o $@ $(LDFLAGS) $(LDLIBS)
build/select_server: 08-io-multiplexing/select/server.c common/net.c common/net.h common/reactor.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 08-io-multiplexing/select/server.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/poll_server: 08-io-multiplexing/poll/server.c common/net.c common/net.h common/reactor.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 08-io-multiplexing/poll/server.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/epoll_server: 08-io-multiplexing/epoll/server.c common/net.c common/net.h common/reactor.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 08-io-multiplexing/epoll/server.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/connect_timeout: 09-advanced-sockets/blocking-vs-nonblocking/connect.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 09-advanced-sockets/blocking-vs-nonblocking/connect.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/socket_options: 09-advanced-sockets/socket-options/options.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 09-advanced-sockets/socket-options/options.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/receive_timeout: 09-advanced-sockets/timeouts/timeout.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 09-advanced-sockets/timeouts/timeout.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/broken_pipe: 09-advanced-sockets/error-handling/broken-pipe.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 09-advanced-sockets/error-handling/broken-pipe.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/ipv6_server: 09-advanced-sockets/ipv6/server.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 09-advanced-sockets/ipv6/server.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/ipv6_client: 09-advanced-sockets/ipv6/client.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 09-advanced-sockets/ipv6/client.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/tcp_chat_server: 11-real-world-projects/tcp-chat/server.c common/net.c common/chat.c common/net.h common/chat.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 11-real-world-projects/tcp-chat/server.c common/net.c common/chat.c -o $@ $(LDFLAGS) $(LDLIBS)
build/tcp_chat_client: 11-real-world-projects/tcp-chat/client.c common/net.c common/chat.c common/net.h common/chat.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 11-real-world-projects/tcp-chat/client.c common/net.c common/chat.c -o $@ $(LDFLAGS) $(LDLIBS)
build/udp_chat: 11-real-world-projects/udp-chat/peer.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 11-real-world-projects/udp-chat/peer.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/multi_chat: 11-real-world-projects/multi-client-chat/server.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 11-real-world-projects/multi-client-chat/server.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/file_sender: 11-real-world-projects/file-transfer/sender.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 11-real-world-projects/file-transfer/sender.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/file_receiver: 11-real-world-projects/file-transfer/receiver.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 11-real-world-projects/file-transfer/receiver.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
build/http_server: 11-real-world-projects/simple-http-server/server.c common/net.c common/net.h | build
	$(CC) $(CPPFLAGS) $(CFLAGS) 11-real-world-projects/simple-http-server/server.c common/net.c -o $@ $(LDFLAGS) $(LDLIBS)
test: all
	python3 tests/test_course.py
check-docs:
	python3 tools/check_docs.py
list:
	@python3 tools/list_programs.py
sanitize:
	$(MAKE) clean
	$(MAKE) CFLAGS="-std=c11 -O1 -g -Wall -Wextra -Wpedantic -Werror -fsanitize=undefined -fno-omit-frame-pointer" LDFLAGS="-fsanitize=undefined" test
clean:
	rm -rf build
