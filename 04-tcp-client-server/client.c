#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
/* Step: Read one input line, send a length-prefixed frame, then receive one frame. */
int main(int argc, char **argv) {
    if (argc != 3) { fprintf(stderr, "usage: %s HOST PORT < lines.txt\n", argv[0]); return 1; }
    net_init();
    int fd = net_connect(argv[1], argv[2], SOCK_STREAM);
    if (fd < 0) die("connect");
    if (set_timeout(fd, 15) < 0) { close_fd(fd); die("timeout"); }
    char input[4096], output[4096];
    int status = 0;
    while (fgets(input, sizeof input, stdin)) {
        uint32_t length = (uint32_t)strlen(input), received;
        if (frame_send(fd, input, length) < 0 ||
            frame_recv(fd, output, sizeof output, &received) != 1) {
            fprintf(stderr, "frame exchange failed or peer closed\n"); status = 1; break;
        }
        /* Step: fwrite uses the received length, so a remote NUL is not a terminator. */
        if (fwrite(output, 1, received, stdout) != received) { status = 1; break; }
        fflush(stdout);
    }
    if (ferror(stdin)) status = 1;
    close_fd(fd); return status;
}
