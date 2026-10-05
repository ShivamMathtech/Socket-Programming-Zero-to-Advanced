#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
/* Step: fork copies the process address space; changing a private variable is not IPC. */
int main(void) {
    int value = 7;
    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }
    if (pid == 0) { value = 99; printf("child value=%d\n", value); fflush(stdout); _exit(0); }
    int status; pid_t result;
    do { result = waitpid(pid, &status, 0); } while (result < 0 && errno == EINTR);
    if (result < 0) { perror("waitpid"); return 1; }
    printf("parent value=%d\n", value);
    return WIFEXITED(status) && WEXITSTATUS(status) == 0 ? 0 : 1;
}
