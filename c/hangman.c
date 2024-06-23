#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define FRAME_WIDTH  9
#define FRAME_HEIGHT 5
#define FRAME_COUNT  7
#define MAX_ERR      FRAME_COUNT - 1
#define MAX_LEN      32

#define ARRAY_SIZE(x) (sizeof(x) / sizeof(*(x)))

char hangman[] = " .----.   .----.   .----.   .----.   .----.   .----.   .----.  "
                 "      |   O    |   O    |   O    |   O    |   O    |   O    |  "
                 "      |        |    \\   |   |\\   |  /|\\   |  /|\\   |  /|\\   |  "
                 "      |        |        |        |        |  /     |  / \\   |  "
                 "     / \\      / \\      / \\      / \\      / \\      / \\      / \\ ";

char *cities[] = {
    "Warszawa",
    "Szczecin",
    "Sosnowiec",
    "Gdynia",
    "Zakopane"
};

void draw_hangman(int mistakes);

int main(void) {
    // INITIALIZATION
    char guess[MAX_LEN] = { 0 };
    char input[MAX_LEN] = { 0 };
    srand(time(NULL));
    int city = rand() % ARRAY_SIZE(cities);
    char *solution = cities[city];
    for (int i = 0; i < strlen(solution); i++) guess[i] = '_';

    // GAME STATE
    int mistakes = 0;
    bool has_guessed_letter = false;
    bool has_won = false; 

    // GAME LOOP
    while (!has_won && mistakes < MAX_ERR) {
        printf("\033[J");                                   // clear screen downwards
        draw_hangman(mistakes);
        printf("%s\n", guess);
        printf("> ");
        fgets(input, sizeof(input), stdin);
        input[strlen(input) - 1] = '\0';                    // loose new line for strcmp
        printf("\033[%dA", FRAME_HEIGHT + 2);               // move cursor up by frame and
                                                            // guess + input line (2)
        has_guessed_letter = false;
        if (strlen(input) == 1) {                           // single letter guess
            for (int i = 0; i < strlen(solution); i++) {
                if (solution[i] == input[0]) {
                    guess[i] = input[0];
                    has_guessed_letter = true;
                } 
            }
            has_won = strcmp(guess, solution) == 0;
        } else {                                            // word guess
            has_won = strcmp(input, solution) == 0;
        }

        if (!has_won && !has_guessed_letter) mistakes++;
    }

    // FINAL MESSAGE
    draw_hangman(mistakes);
    if (has_won) {
        printf("%s\n", solution);
        printf("Congratulations!\n");
    } else {
        printf("Game over.\n");
    }

    return EXIT_SUCCESS;
}

void draw_hangman(int mistakes) {
    for (int row = 0; row < FRAME_HEIGHT; row++) {
        for (int col = 0; col < FRAME_WIDTH; col++) {
            printf("%c", hangman[row * FRAME_COUNT * FRAME_WIDTH +  // row index
                                 mistakes * FRAME_WIDTH + col]);    // col index
        }
        printf("\n");
    }
}

