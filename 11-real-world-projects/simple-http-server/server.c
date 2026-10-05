#include "net.h"
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
/* Step: This is a deliberately small HTTP subset: GET and HEAD, two fixed routes, close after response. */
static int response(int fd, int status, const char *reason, const char *body, int head) {
    char headers[512]; size_t length = strlen(body);
    int n = snprintf(headers, sizeof headers,
        "HTTP/1.1 %d %s\r\nContent-Type: text/plain; charset=utf-8\r\n"
        "Content-Length: %zu\r\nConnection: close\r\nX-Content-Type-Options: nosniff\r\n%s\r\n",
        status, reason, length, status == 405 ? "Allow: GET, HEAD\r\n" : "");
    if (n < 0 || (size_t)n >= sizeof headers) { errno = EOVERFLOW; return -1; }
    if (send_all(fd, headers, (size_t)n) < 0) return -1;
    return head ? 0 : send_all(fd, body, length);
}
static int serve(int fd) {
    char request[8193]; size_t used = 0;
    /* Step: TCP can split headers anywhere. Accumulate until CRLF CRLF, with an 8 KiB limit. */
    while (used < sizeof request - 1) {
        ssize_t n = recv(fd, request + used, sizeof request - 1 - used, 0);
        if (n < 0 && errno == EINTR) continue;
        if (n < 0) return -1;
        if (n == 0) return used ? response(fd,400,"Bad Request","Incomplete request\n",0) : 0;
        if (memchr(request + used, '\0', (size_t)n)) return response(fd,400,"Bad Request","Invalid NUL byte\n",0);
        used += (size_t)n; request[used] = '\0';
        if (strstr(request, "\r\n\r\n")) break;
    }
    if (!strstr(request, "\r\n\r\n")) return response(fd,431,"Request Header Fields Too Large","Header limit exceeded\n",0);
    char *end = strstr(request, "\r\n");
    if (!end) return response(fd,400,"Bad Request","Missing request line\n",0);
    *end = '\0';
    char method[16], path[1024], version[16], extra;
    if (sscanf(request, "%15s %1023s %15s %c", method, path, version, &extra) != 3 ||
        (strcmp(version,"HTTP/1.0") && strcmp(version,"HTTP/1.1")))
        return response(fd,400,"Bad Request","Bad request line\n",0);
    int head = !strcmp(method,"HEAD");
    if (strcmp(method,"GET") && !head) return response(fd,405,"Method Not Allowed","Use GET or HEAD\n",0);
    /* Step: No URL maps to a filesystem path. Unknown paths always return 404. */
    if (!strcmp(path,"/")) return response(fd,200,"OK","Socket Programming Lab\n",head);
    if (!strcmp(path,"/health")) return response(fd,200,"OK","ok\n",head);
    return response(fd,404,"Not Found","Not found\n",head);
}
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
    net_init(); int listener = net_listen("127.0.0.1",argv[1]);
    if (listener < 0) die("listen");
    printf("HTTP lab: http://127.0.0.1:%s\n",argv[1]); fflush(stdout);
    for (;;) {
        int peer = accept_retry(listener);
        if (peer < 0) { perror("accept"); break; }
        if (set_timeout(peer,3) < 0 || serve(peer) < 0) perror("HTTP client");
        close_fd(peer);
    }
    close_fd(listener); return 1;
}
