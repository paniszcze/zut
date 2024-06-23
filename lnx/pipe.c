#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {
    int pipefd[2], pid;
    char buf;

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        close(pipefd[1]);
        while (read(pipefd[0], &buf, 1) > 0) {
            write(STDOUT_FILENO, &buf, 1);
        }
        close(pipefd[0]);
    } else {
        close(pipefd[0]);
        char *hello = "hello world\n";
        write(pipefd[1], hello, strlen(hello));
        close(pipefd[1]);
        wait(NULL);
    }

    return EXIT_SUCCESS;
}

