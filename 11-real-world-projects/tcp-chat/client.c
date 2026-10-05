#include "net.h"
#include "chat.h"
#include <stdio.h>
#include <sys/socket.h>
/* Step: The client and server share the same duplex behavior after establishment. */
int main(int argc, char **argv) {
    if (argc != 3) { fprintf(stderr, "usage: %s HOST PORT\n", argv[0]); return 1; }
    net_init(); int fd = net_connect(argv[1], argv[2], SOCK_STREAM);
    if (fd < 0) die("connect");
    int result = chat_session(fd);
    if (result < 0) perror("chat");
    close_fd(fd); return result < 0 ? 1 : 0;
}
