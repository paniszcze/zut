#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

void clear_scr();
void print_dashbrd();

int main(int argc, char **argv)
{
    int opt = -1, ret;

    while (true) {
        clear_scr();
        print_dashbrd();

        if (scanf("%d", &opt) != 1 || opt == 0) break;  // exit on 0 and non-integer values

        ret = fork();
        if (ret == 0) {
            switch (opt) {
            case 1:
                ret = execlp("nudoku", "nudoku", NULL);
                break;
            case 2:
                ret = execlp("greed", "greed", NULL);
                break;
            case 3:
                ret = execlp("moon-buggy", "moon-buggy", NULL);
                break;
            case 4:
                ret = execlp("ninvaders", "ninvaders", NULL);
                break;
            case 5:
                ret = execlp("nsnake", "nsnake", NULL);
                break;
            default:
                return 0;
            }

            if (ret) {
                perror("execvp: ");
                return -1;
            }
        }

        wait(NULL);
    }

    clear_scr();
    return 0;
}

void clear_scr() {
    printf("\x1b[2J\x1b[0;0H");
}

void print_dashbrd() {
    printf("  ___   __   __  __ ____ ___ ____  __  ____ ____ _____ _  _\n"
           " / __) /__\\ (  \\/  ( ___/ __(_  _)/__\\(_  _(_  _(  _  ( \\( )\n"
           "( (_-./(__)\\ )    ( )__)\\__ \\ )( /(__)\\ )(  _)(_ )(_)( )  (\n"
           " \\___(__)(__(_/\\/\\_(____(___/(__(__)(__(__)(____(_____(_)\\_)\n\n"
           " Choose a game:\n\n"
           " 1) nudoku\n" 
           " 2) greed\n"
           " 3) moon-buggy\n"
           " 4) ninvaders\n"
           " 5) nsnake\n\n"
           " type '0' to exit\n\n" 
           " SELECT > ");
}

