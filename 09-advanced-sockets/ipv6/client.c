#include "net.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
/* Step: This same client resolves IPv4 or IPv6; IPv6 literals need no URL brackets here. */
int main(int argc, char **argv) {
    if (argc != 4 || strlen(argv[3]) > 4096) { fprintf(stderr, "usage: %s HOST PORT MESSAGE\n", argv[0]); return 1; }
    net_init(); int fd = net_connect(argv[1], argv[2], SOCK_STREAM);
    if (fd < 0) die("connect");
    size_t length = strlen(argv[3]); char reply[4096]; int status = 1;
    if (set_timeout(fd, 3) < 0 || send_all(fd, argv[3], length) < 0) perror("send");
    else if (recv_exact(fd, reply, length) != 1) fprintf(stderr, "incomplete echo\n");
    else if (fwrite(reply, 1, length, stdout) == length && putchar('\n') != EOF) status = 0;
    close_fd(fd); return status;
}
