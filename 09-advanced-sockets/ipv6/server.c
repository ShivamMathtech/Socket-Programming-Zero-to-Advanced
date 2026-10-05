#include "net.h"
#include <stdio.h>
/* Step: getaddrinfo and sockaddr_storage avoid hard-coding IPv4 structure sizes in shared code. */
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
    net_init(); int listener = net_listen("::1", argv[1]);
    if (listener < 0) die("IPv6 listen");
    printf("IPv6 echo on [::1]:%s\n", argv[1]); fflush(stdout);
    for (;;) {
        int fd = accept_retry(listener);
        if (fd < 0) { perror("accept"); break; }
        if (set_timeout(fd, 15) < 0 || echo_connection(fd) < 0) perror("IPv6 echo");
        close_fd(fd);
    }
    close_fd(listener); return 1;
}
