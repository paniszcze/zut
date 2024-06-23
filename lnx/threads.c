#include <ctype.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define DEFAULT_THREADS 4
#define DEFAULT_SIZE    32
#define DEFAULT_SCALE   2
#define DISPLAY_COLS    8
#define NUM_BASE        10

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct Data {
    int  * data;
    size_t size;
    size_t count;
} Data;

int get_rand_int(size_t range) {
    return rand() % range;
}

static int compare_ints(const void *a, const void *b) {
    int num1 = *(const int *)a;
    int num2 = *(const int *)b;
    return (num1 > num2) - (num1 < num2);
}

bool is_duplicate(int num, Data *d) {
    if (d->count == 0) return false;

    bool has_found = false;
    size_t count = 0;

    while (count < d->count && !has_found) {
        has_found = !(bool)compare_ints(&d->data[count], &num);
        count += 1;
    }

    return has_found;
}

void sort_data(Data *d) {
    qsort(d->data, d->count, sizeof(d->data[0]), compare_ints);
}

int get_col_width(int num) {
    int ret = 1;
    if (num < 0) ret++;

    while (num >= NUM_BASE) {
        num /= NUM_BASE;
        ret += 1;
    }

    return ret;
}

void print_data(Data *d) {
    int idx_w = get_col_width(d->size);
    int col_w = get_col_width(d->size * DEFAULT_SCALE);

    for (int i = 0; i < d->size; i++) {
        printf("[%*d] %*d ", idx_w, i, col_w, d->data[i]);
        if (i != d->size - 1 && (i + 1) % DISPLAY_COLS == 0) printf("\n");
    }
    printf("\n");
}

static void *thread_start(void *arg) {
    Data *d = arg;
    size_t range = d->size * DEFAULT_SCALE;

    while (true) {
        int num = get_rand_int(range);
        usleep(10); // simulate a time-consuming calculation
                    // to demonstrate multithreading

        pthread_mutex_lock(&lock);
        if (d->count >= d->size) {
            pthread_mutex_unlock(&lock);
            break;
        }
        if (!is_duplicate(num, d)) {
            d->data[d->count] = num;
            d->count += 1;
            printf("[LOG] %lu: saved %d\n", pthread_self(), num);
            sort_data(d);
        }
        pthread_mutex_unlock(&lock);
    }

    return EXIT_SUCCESS;
}

int main(int argc, char *argv[]) {
    int ret, opt, num_threads, size;
    
    num_threads = DEFAULT_THREADS;
    size = DEFAULT_SIZE;
    srand(time(NULL));

    while ((opt = getopt(argc, argv, "t:s:")) != -1) {
        switch (opt) {
        case 't':
            if ((num_threads = atoi(optarg)) <= 0) {
                fprintf(stderr, "Invalid number of threads. Exiting\n");
                exit(EXIT_FAILURE);
            }
            break;
        case 's':
            if ((size = atoi(optarg)) <= 0) {
                fprintf(stderr, "Invalid size. Exiting\n");
                exit(EXIT_FAILURE);
            }
            break;
        default: // '?'
            fprintf(stderr, "Usage: %s [-t threads] [-s size]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    Data *data = calloc(1, sizeof(Data));
    if (data == NULL) {
        perror("caloc");
        exit(EXIT_FAILURE);
    }
    if ((data->data = malloc(size * sizeof(int))) == NULL) {
        perror("caloc");
        exit(EXIT_FAILURE);
    }
    data->size = size;
    memset(data->data, -1, data->size * sizeof(int));

    pthread_t *ts = calloc(num_threads, sizeof(pthread_t));
    if (ts == NULL) {
        fprintf(stderr, "calloc");
        exit(EXIT_FAILURE);
    }

    for (int tnum = 0; tnum < num_threads; tnum++) {
        ret = pthread_create(&ts[tnum], NULL, &thread_start, data);
        if (ret != 0) {
            fprintf(stderr, "pthread_create\n");
            exit(EXIT_FAILURE);
        }
        printf("[LOG] %lu: created\n", ts[tnum]);
    }

    for (int tnum = 0; tnum < num_threads; tnum++) {
        ret = pthread_join(ts[tnum], NULL);
        if (ret != 0) {
            fprintf(stderr, "pthread_join\n");
            exit(EXIT_FAILURE);
        }
        printf("[LOG] %lu: joined\n", ts[tnum]);
    }
    pthread_mutex_destroy(&lock);

    printf("\nDATA:\n");
    print_data(data);

    free(ts);
    free(data->data);
    free(data);
}

