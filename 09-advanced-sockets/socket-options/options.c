#include "net.h"
#include <stdio.h>
#include <sys/socket.h>
/* Step: Query the kernel after setting options; effective buffer sizes are implementation dependent. */
int main(void) {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) die("socket");
    int one = 1, value = 0;
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &one, sizeof one) < 0) { close_fd(fd); die("setsockopt"); }
    socklen_t size = sizeof value;
    if (getsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &value, &size) < 0) { close_fd(fd); die("getsockopt"); }
    printf("SO_KEEPALIVE=%d\n", value);
    size = sizeof value;
    if (getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &value, &size) < 0) { close_fd(fd); die("SO_RCVBUF"); }
    printf("SO_RCVBUF=%d (host-dependent)\n", value);
    close_fd(fd); return 0;
}
