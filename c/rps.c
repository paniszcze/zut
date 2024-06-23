#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum {
    ROCK,
    PAPER,
    SCISSORS,
    RPS_COUNT
} rps_t;

const char *rps_labels[RPS_COUNT] = {
    [ROCK] = "rock",
    [PAPER] = "paper",
    [SCISSORS] = "scissors",
};

void display_player(const char *name, rps_t rps);

int main(void) {
    srand(time(NULL));

    rps_t player1 = rand() % RPS_COUNT;
    rps_t player2 = rand() % RPS_COUNT;

    display_player("player 1", player1);
    display_player("player 2", player2);

    printf("result  : ");
    if (player1 == player2) {
        printf("draw\n");
    } else if ((player1 == ROCK && player2 == PAPER) ||
               (player1 == PAPER && player2 == SCISSORS) ||
               (player1 == SCISSORS && player2 == ROCK)) {
        printf("player 2 wins the game\n");
    } else {
        printf("player 1 wins the game\n");
    }

    return EXIT_SUCCESS;
}

void display_player(const char *name, rps_t rps) {
    printf("%s: %s\n", name, rps_labels[rps]);
}

