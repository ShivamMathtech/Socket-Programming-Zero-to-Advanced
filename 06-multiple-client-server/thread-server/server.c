#include "net.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Step: Threads share descriptors and memory. Protect only shared metadata with the mutex. */
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static unsigned active;
static void check(int code, const char *what) {
    if (code) { fprintf(stderr, "%s: %s\n", what, strerror(code)); exit(1); }
}
static void *serve(void *argument) {
    int fd = *(int *)argument; free(argument);
    if (set_timeout(fd, 15) < 0 || echo_connection(fd) < 0) perror("thread echo");
    close_fd(fd);
    check(pthread_mutex_lock(&lock), "lock");
    --active;
    check(pthread_mutex_unlock(&lock), "unlock");
    return NULL;
}
/* Step: Detached workers release thread resources automatically when they return. */
int main(int argc, char **argv) {
    if (argc != 2) { fprintf(stderr, "usage: %s PORT\n", argv[0]); return 1; }
    net_init();
    int listener = net_listen("127.0.0.1", argv[1]);
    if (listener < 0) die("listen");
    pthread_attr_t attributes;
    check(pthread_attr_init(&attributes), "attr init");
    check(pthread_attr_setdetachstate(&attributes, PTHREAD_CREATE_DETACHED), "detach state");
    printf("Thread echo on %s; maximum 64 workers\n", argv[1]); fflush(stdout);
    for (;;) {
        int peer = accept_retry(listener);
        if (peer < 0) { perror("accept"); break; }
        check(pthread_mutex_lock(&lock), "lock");
        int full = active >= 64;
        if (!full) ++active;
        check(pthread_mutex_unlock(&lock), "unlock");
        if (full) { close_fd(peer); continue; }
        /* Step: Heap storage prevents the classic address-of-loop-variable race. */
        int *argument = malloc(sizeof *argument);
        int code = 0;
        pthread_t worker;
        if (argument) { *argument = peer; code = pthread_create(&worker, &attributes, serve, argument); }
        if (!argument || code) {
            fprintf(stderr, "worker allocation/create failed%s%s\n", code ? ": " : "", code ? strerror(code) : "");
            free(argument); close_fd(peer);
            check(pthread_mutex_lock(&lock), "lock"); --active;
            check(pthread_mutex_unlock(&lock), "unlock");
        }
    }
    check(pthread_attr_destroy(&attributes), "attr destroy");
    close_fd(listener); return 1;
}
