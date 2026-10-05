#include "net.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
/* Step: The sender never transmits a filename, so the peer cannot choose an output path. */
int main(int argc, char **argv) {
    if (argc != 4) { fprintf(stderr, "usage: %s HOST PORT INPUT_FILE\n", argv[0]); return 1; }
    FILE *input = fopen(argv[3], "rb"); if (!input) die("input file");
    net_init(); int fd = net_connect(argv[1], argv[2], SOCK_STREAM);
    if (fd < 0) { fclose(input); die("connect"); }
    int status = 1; unsigned char data[32768]; unsigned long long total = 0;
    if (set_timeout(fd, 15) < 0) { perror("timeout"); goto done; }
    for (;;) {
        size_t n = fread(data, 1, sizeof data, input);
        if (n && frame_send(fd, data, (uint32_t)n) < 0) { perror("send chunk"); goto done; }
        total += n;
        if (n < sizeof data) { if (ferror(input)) { perror("read file"); goto done; } break; }
    }
    /* Step: A zero-length frame marks a complete file; socket EOF alone is not success. */
    if (frame_send(fd, data, 0) < 0) { perror("end marker"); goto done; }
    char ack[2];
    if (recv_exact(fd, ack, sizeof ack) != 1 || memcmp(ack, "OK", 2)) {
        fprintf(stderr, "receiver did not confirm complete file\n"); goto done;
    }
    printf("Transferred %llu bytes; receiver acknowledged\n", total); status = 0;
done:
    if (fclose(input) != 0) { perror("fclose"); status = 1; }
    close_fd(fd); return status;
}
