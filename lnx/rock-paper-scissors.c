#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_ROUNDS 30
#define MAX_WINS   10

bool should_end = false;

typedef enum {
    RPS_R = 0,  // rock
    RPS_P,      // paper
    RPS_S,      // scissors
    RPS_CNT,
} RPS;

const char *rps_labels[RPS_CNT] = {
    [RPS_R] = "rock",
    [RPS_P] = "paper",
    [RPS_S] = "scissors",
};

RPS choose_rps(void) {
    return rand() % RPS_CNT;
}

int compare_rps(RPS a, RPS b) {
    if (a == b) {
        return 0;
    } else if ((a == RPS_R && b == RPS_P) ||
               (a == RPS_P && b == RPS_S) ||
               (a == RPS_S && b == RPS_R)) {
        return -1;
    } else {
        return 1;
    }
}

char get_sign(RPS a, RPS b) {
    int res = compare_rps(a, b);
    switch (res) {
    case 0:
        return '=';
    case 1:
        return '>';
    case -1:
        return '<';
    default:
        return '?';
    }
}

void print_rps(RPS rps) {
    printf("%s", rps_labels[rps]);
}

const char *get_rps_label(RPS rps) {
    return rps_labels[rps];
}

typedef enum {
    PL_A = 0,      // player A
    PL_B,          // player B
    PL_CNT,
    PL_NONE = -1,
} PL;

typedef struct Player {
    PL              id;
    pthread_t       tid;
    pthread_cond_t  cond;
    pthread_mutex_t mut;
    sem_t           sem;
    RPS             rps;
    bool            has_chosen;
    size_t          wins;
} Player;

PL get_other(PL p) {
    if (p == PL_A || p == PL_B) {
        return ~p & 1;
    }
    return PL_NONE;
}

PL get_winner_id(Player *a, Player *b) {
    int w = compare_rps(a->rps, b->rps);
    switch (w) {
    case 1:
        return PL_A;
    case -1:
        return PL_B;
    default:
        return PL_NONE;
    }
}

PL get_final_id(Player *a, Player *b) {
    if (a->wins >= MAX_WINS) {
        return PL_A;
    } else if (b->wins >= MAX_WINS) {
        return PL_B;
    }
    return PL_NONE;
}

bool check_for_end(Player *a, Player *b, size_t rounds) {
    if (a->wins >= MAX_WINS || b->wins >= MAX_WINS || rounds >= MAX_ROUNDS) {
        return true;
    }
    return false;
}

void *player(void *arg) {
    Player *p = arg;

    while (true) {
        sem_wait(&p->sem);
        if (should_end)
            break;
        pthread_mutex_lock(&p->mut);
        p->rps = choose_rps();
        p->has_chosen = true;
        printf("P%d chose %s\n", p->id, get_rps_label(p->rps));
        pthread_cond_signal(&p->cond);
        pthread_mutex_unlock(&p->mut);
    }

    return EXIT_SUCCESS;
}

int main(void) {
    srand(time(NULL));

    Player ps[PL_CNT] = { 0 };

    for (size_t p = 0; p < PL_CNT; p++) {
        ps[p].id = p;
        if (pthread_create(&ps[p].tid, NULL, player, &ps[p]) != 0) {
            perror("pthread_create");
            exit(EXIT_FAILURE);
        }
        if (pthread_mutex_init(&ps[p].mut, NULL) != 0) {
            perror("pthread_mutex_init");
            exit(EXIT_FAILURE);
        }
        if (pthread_cond_init(&ps[p].cond, NULL) != 0) {
            perror("pthread_mutex_cond");
            exit(EXIT_FAILURE);
        }
        if (sem_init(&ps[p].sem, 0, 0) != 0) {
            perror("sem_init");
            exit(EXIT_FAILURE);
        }
    }

    PL first  = PL_A;  // player A begins the 1st round
    PL second = PL_B;
    size_t round = 1;

    while (!should_end) {
        printf("------- ROUND %lu -------\n", round);

        sem_post(&ps[first].sem);

        pthread_mutex_lock(&ps[first].mut);
        while (!ps[first].has_chosen)
            pthread_cond_wait(&ps[first].cond, &ps[first].mut);
        pthread_mutex_unlock(&ps[first].mut);

        sem_post(&ps[second].sem);

        pthread_mutex_lock(&ps[second].mut);
        while (!ps[second].has_chosen) {
            pthread_cond_wait(&ps[second].cond, &ps[second].mut);
        }
        pthread_mutex_unlock(&ps[second].mut);

        if (ps[first].has_chosen && ps[second].has_chosen) {
            printf("%s %c %s\n",
                    get_rps_label(ps[PL_A].rps),
                    get_sign(ps[PL_A].rps, ps[PL_B].rps),
                    get_rps_label(ps[PL_B].rps));

            PL winner = get_winner_id(&ps[PL_A], &ps[PL_B]);
            if (winner != -1) {
                ps[winner].wins += 1;
                if (winner == first) {
                    first = get_other(winner);
                    second = winner;
                }
            }

            ps[first].has_chosen = false;
            ps[second].has_chosen = false;
        }

        if ((should_end = check_for_end(&ps[PL_A], &ps[PL_B], round))) {
            sem_post(&ps[first].sem);
            sem_post(&ps[second].sem);
            break;
        }

        round += 1;
    }

    PL final_winner = get_final_id(&ps[PL_A], &ps[PL_B]);
    size_t draws = round - ps[PL_A].wins - ps[PL_B].wins;
    printf("------- RESULTS -------\n");
    printf("P%d:%lu vs P%d:%lu (%lu draw%s)\n",
            ps[PL_A].id, ps[PL_A].wins,
            ps[PL_B].id, ps[PL_B].wins,
            draws, draws == 1 ? "" : "s");
    if (final_winner == PL_NONE) {
        printf("No winner after %lu rounds\n", round);
    } else {
        printf("The winner is player %d!\n", final_winner);
    }


    for (size_t p = 0; p < PL_CNT; p++) {
        if (pthread_join(ps[p].tid, NULL) != 0) {
            perror("pthread_join");
            exit(EXIT_FAILURE);
        }
        if (pthread_mutex_destroy(&ps[p].mut) != 0) {
            perror("pthread_mutex_destroy");
            exit(EXIT_FAILURE);
        }
        if (pthread_cond_destroy(&ps[p].cond) != 0) {
            perror("pthread_cond_destroy");
            exit(EXIT_FAILURE);
        }
        if (sem_destroy(&ps[p].sem) != 0) {
            perror("sem_destroy");
            exit(EXIT_FAILURE);
        }
    }
}

