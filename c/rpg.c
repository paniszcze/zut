#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define MAP_ROWS 5
#define MAP_COLS 12
#define INACTIVE 2  // increase avg number of enemies' inactive rounds; should be >= 0, 
                    // with 0 meaning that an enemy will try to move at each round

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(*(x)))

typedef enum { UP, DOWN, LEFT, RIGHT, DIR_COUNT } dir_t;

const char key[DIR_COUNT] = {
    [UP]    = 'w',
    [DOWN]  = 's',
    [LEFT]  = 'a',
    [RIGHT] = 'd',
};

typedef struct {
    char c;
    int  row;
    int  col;
    bool is_dead;
} ch_t;

dir_t get_dir(char c);
void character_move(ch_t *ch, dir_t dir);
void character_random_move(ch_t *ch);

typedef struct {
    char cells[MAP_ROWS][MAP_COLS];
} frame_t;

void frame_clear(frame_t *frame);
void frame_add(frame_t *frame, ch_t *ch);
void frame_draw(frame_t *frame);

int main(void) {
    srand((unsigned int)time(NULL));
    frame_t frame;
    ch_t player = { '&', 0, 0, false };
    ch_t enemies[] = {
        { 'X', 0, MAP_COLS - 1, false },
        { 'Y', MAP_ROWS - 1, 0, false },
        { 'Z', MAP_ROWS - 1, MAP_COLS - 1, false },
    };

    while (true) {
        frame_clear(&frame);

        bool has_won = true;
        for (int i = 0; i < ARRAY_SIZE(enemies); i++) {
            // move living enemies and check if they die afterwards
            // NOTE: enemies get to move first to make the chase harder
            //       (player can 'miss' an enemy), but it also means
            //       an enemy can run into the player before he/she makes a move
            //       (collision check needed to prevent this behaviour)
            if (!enemies[i].is_dead) {
                character_random_move(&enemies[i]);
                if (enemies[i].row == player.row && enemies[i].col == player.col) {
                    enemies[i].is_dead = true;
                }
            }
            // add remaining enemies to frame
            if (!enemies[i].is_dead) {
                frame_add(&frame, &enemies[i]);
                has_won = false;
            }
        }

        frame_add(&frame, &player);
        frame_draw(&frame);

        if (has_won) break;
        
        char c;
        scanf(" %c", &c);
        character_move(&player, get_dir(c));
    }

    return 0;
}

dir_t get_dir(char c) {
    for (int i = 0; i < DIR_COUNT; i++) {
        if (key[i] == c) return (dir_t)i;
    }
    return (dir_t)c;
}

void character_move(ch_t *ch, dir_t dir) {
    switch (dir) {
        case UP:
            if (ch->row > 0) ch->row--;
            break;
        case DOWN:
            if (ch->row < MAP_ROWS - 1) ch->row++;
            break;
        case LEFT:
            if (ch->col > 0) ch->col--;
            break;
        case RIGHT:
            if (ch->col < MAP_COLS - 1) ch->col++;
            break;
        default:
            break;
    }
}

void character_random_move(ch_t *ch)
{
    int dir = rand() % (DIR_COUNT + INACTIVE);
    character_move(ch, (dir_t)dir);
}

void frame_clear(frame_t *frame) {
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            frame->cells[i][j] = '.';
        }
    }
}

void frame_add(frame_t *frame, ch_t *ch) {
    frame->cells[ch->row][ch->col] = ch->c;
}

void frame_draw(frame_t *frame) {
    printf("\033[2J\033[0;0H");
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            printf("%c", frame->cells[i][j]);
        }
        printf("\n");
    }
}

