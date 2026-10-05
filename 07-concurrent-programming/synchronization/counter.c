#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* Step: Increment is read-modify-write, so all threads must hold the same mutex. */
static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
static unsigned counter;
static void check(int code) { if (code) { fprintf(stderr, "%s\n", strerror(code)); exit(1); } }
static void *increment(void *unused) {
    (void)unused;
    for (unsigned i = 0; i < 100000; ++i) {
        check(pthread_mutex_lock(&lock));
        ++counter;
        check(pthread_mutex_unlock(&lock));
    }
    return NULL;
}
int main(void) {
    pthread_t workers[4];
    for (int i = 0; i < 4; ++i) check(pthread_create(&workers[i], NULL, increment, NULL));
    for (int i = 0; i < 4; ++i) check(pthread_join(workers[i], NULL));
    printf("counter=%u expected=400000\n", counter);
    check(pthread_mutex_destroy(&lock));
    return counter == 400000 ? 0 : 1;
}
