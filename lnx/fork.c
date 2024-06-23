#include <pthread.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define DEFAULT_PROCS 3
#define BUFF_SIZE     256

int pipefd[2];

void *reader(void *arg) {
    char buf[BUFF_SIZE];
    while ((read(pipefd[0], &buf, sizeof(buf))) > 0) {
        printf("%s\n", buf);
    }
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    int opt, procs_num = DEFAULT_PROCS;

    while ((opt = getopt(argc, argv, "n:")) != -1) {
        switch (opt) {
        case 'n':
            if ((procs_num = atoi(optarg)) != 0) {
                break;
            }
            fprintf(stderr, "%s: invalid input: %s\n", argv[0], optarg);
        default: // '?'
            fprintf(stderr, "Usage: %s [-n <procs_num>]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);

    }
    pthread_t tid;
    pthread_create(&tid, NULL, reader, NULL);
    pid_t cpid, wpid;
    char msg[BUFF_SIZE];

    for (size_t i = 0; i < procs_num; i++) {
        cpid = fork();
        if (cpid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (cpid == 0) {
            snprintf(msg, BUFF_SIZE, "[C] %d was forked from %d", getpid(), getppid());
            if (write(pipefd[1], msg, BUFF_SIZE) < 0) {
                perror("write: child");
                exit(EXIT_FAILURE);
            }
        } else {
            snprintf(msg, BUFF_SIZE, "[P] %d has created %d", getpid(), cpid);
            if (write(pipefd[1], msg, BUFF_SIZE) < 0) {
                perror("write: parent");
                exit(EXIT_FAILURE);
            }
        }
    }

    while ((wpid = wait(NULL)) > 0) {
        snprintf(msg, BUFF_SIZE, "[C] %d terminated", wpid);
        if (write(pipefd[1], msg, BUFF_SIZE) < 0) {
            perror("write: wait");
            exit(EXIT_FAILURE);
        }
    };

    close(pipefd[0]);
    close(pipefd[1]);
}

