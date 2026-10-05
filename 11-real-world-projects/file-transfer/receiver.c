#include "net.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/socket.h>
#include <unistd.h>
/* Step: O_EXCL refuses overwriting an existing file, including a pre-existing symbolic link. */
int main(int argc, char **argv) {
    if (argc != 3) { fprintf(stderr, "usage: %s PORT NEW_OUTPUT_FILE\n", argv[0]); return 1; }
    int output = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (output < 0) die("create new output");
    net_init(); int listener = net_listen("127.0.0.1", argv[1]), peer = -1, status = 1;
    unsigned long long total = 0; int committed = 0;
    if (listener < 0) { perror("listen"); goto done; }
    printf("File receiver on %s; one connection\n", argv[1]); fflush(stdout);
    peer = accept_retry(listener);
    if (peer < 0 || set_timeout(peer, 15) < 0) { perror("accept/timeout"); goto done; }
    for (;;) {
        unsigned char data[32768]; uint32_t n;
        int result = frame_recv(peer, data, sizeof data, &n);
        if (result != 1) { fprintf(stderr, "truncated or invalid transfer\n"); goto done; }
        if (n == 0) break;
        /* Step: Bound total disk usage at 64 MiB and loop over short file writes too. */
        if (total + n > 64ULL * 1024 * 1024) { fprintf(stderr, "64 MiB limit exceeded\n"); goto done; }
        size_t written = 0;
        while (written < n) {
            ssize_t count = write(output, data + written, n - written);
            if (count < 0 && errno == EINTR) continue;
            if (count <= 0) { perror("write file"); goto done; }
            written += (size_t)count;
        }
        total += n;
    }
    /* Step: Confirm only after all bytes are flushed and the file descriptor closes successfully. */
    if (fsync(output) < 0) { perror("fsync"); goto done; }
    if (close(output) < 0) { output = -1; perror("close output"); goto done; }
    output = -1; committed = 1;
    if (send_all(peer, "OK", 2) < 0) { perror("acknowledgement"); goto done; }
    printf("Received %llu bytes\n", total); status = 0;
done:
    close_fd(output); close_fd(peer); close_fd(listener);
    /* Step: A partial transfer is removed; a complete file survives an acknowledgement failure. */
    if (!committed && unlink(argv[2]) < 0) perror("remove partial file");
    return status;
}
