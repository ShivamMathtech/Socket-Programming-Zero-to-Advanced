#include <pthread.h>
#include <stdio.h>
#include <string.h>
/* Step: join establishes completion before main reads the worker's result. */
static void *worker(void *argument) { *(int *)argument = 99; return NULL; }
int main(void) {
    int value = 7;
    pthread_t thread;
    int code = pthread_create(&thread, NULL, worker, &value);
    if (code) { fprintf(stderr, "create: %s\n", strerror(code)); return 1; }
    code = pthread_join(thread, NULL);
    if (code) { fprintf(stderr, "join: %s\n", strerror(code)); return 1; }
    printf("shared value=%d\n", value); return 0;
}
