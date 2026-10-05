#include "net.h"
#include "chat.h"
#include <stdio.h>
/* Step: Accept exactly one peer, then run a duplex terminal session. */
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
    net_init(); int listener = net_listen("127.0.0.1", argv[1]);
    if (listener < 0) die("listen");
    fprintf(stderr, "Chat listening on %s\n", argv[1]);
    int peer = accept_retry(listener); close_fd(listener);
    if (peer < 0) die("accept");
    int result = chat_session(peer);
    if (result < 0) perror("chat");
    close_fd(peer); return result < 0 ? 1 : 0;
}
