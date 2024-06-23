#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define LIST_SIZE  5
#define DESC_SIZE  64
#define INPUT_SIZE 64 

typedef struct reminder_t {
    int h;
    int m;
    char desc[DESC_SIZE];
} reminder_t;

typedef struct list_t {
    int count;
    reminder_t items[LIST_SIZE];
} list_t;

typedef struct state_t {
    list_t *list;
    bool is_done;
} state_t;

void *watch(void *arg);
bool is_empty(reminder_t reminder);
int find_empty_slot(list_t list);
void add_reminder(list_t *list, int index, int h, int m, const char *desc);
void delete_reminder(list_t *list, int index);
void print_reminder(reminder_t reminder, bool is_alert);
void print_list(list_t list);
bool is_valid_time(int h, int m);
bool get_desc(char *input, char *desc);

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int main(void) {
    list_t reminders = { 0 };
    state_t state = { &reminders, false };
    
    pthread_t watcher;
    int ret = pthread_create(&watcher, NULL, watch, &state);
    if (ret) {
        fprintf(stderr, "pthread_create failed with %d\n", ret);
        return EXIT_FAILURE;
    }

    while (true) {
        printf("> ");

        char line[INPUT_SIZE];
        if (!fgets(line, sizeof(line), stdin)) {
            if (errno)
                perror("fgets");
            break;
        }

        size_t len = strlen(line);
        if (len == 0) {
            fprintf(stderr, "Unexpected empty string returned by fgets\n");
            break;
        }

        if (line[len - 1] != '\n') {
            do {
                if (!fgets(line, sizeof(line), stdin)) {
                    perror("fgets");
                    break;
                }
            } while (line[strlen(line) - 1] != '\n');
            fprintf(stderr, "Couldn't add reminder: input too long\n");
            continue;
        }
        line[len - 1] = 0;

        int h, m;
        char desc[DESC_SIZE] = { 0 };
        bool has_desc = get_desc(line, desc);
        if (sscanf(line, "%d:%d", &h, &m) == 2) {
            if (!is_valid_time(h, m)) {
                printf("Couldn't add reminder: invalid time\n");
                continue;
            }
            pthread_mutex_lock(&lock);
            int slot = find_empty_slot(reminders);
            if (slot != -1) {
                if (has_desc) {
                    add_reminder(&reminders, slot, h, m, desc);
                    printf("Added reminder \"%s\" at %d:%02d\n", desc, h, m);
                } else {
                    printf("Couldn't add reminder: no description provided\n");
                }
            } else {
                printf("Couldn't add reminder \"%s\" at %d:%02d"
                        ": no free slots on the list\n", desc, h, m);
            }
            pthread_mutex_unlock(&lock);
        }

        if (strcmp("list", line) == 0) {
            pthread_mutex_lock(&lock);
            print_list(reminders);
            pthread_mutex_unlock(&lock);
        }

        if (strcmp("exit", line) == 0) {
            break;
        }
    }

    pthread_mutex_lock(&lock);
    state.is_done = true;
    pthread_mutex_unlock(&lock);

    ret = pthread_join(watcher, NULL);
    if (ret) {
        fprintf(stderr, "pthread_join failed with %d\n", ret);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

void *watch(void *args) {
    state_t *state = args;
    bool has_deleted = false;

    while (true) {
        sleep(1);

        pthread_mutex_lock(&lock);
        if (state->is_done) {
            pthread_mutex_unlock(&lock);
            break;
        }
        pthread_mutex_unlock(&lock);

        struct tm tm;
        time_t t = time(NULL);
        localtime_r(&t, &tm);
        has_deleted = false;

        pthread_mutex_lock(&lock);
        if (state->list->count != 0) {
            for (int i = 0; i < LIST_SIZE; i++) {
                reminder_t r = state->list->items[i];
                if (r.h == tm.tm_hour && r.m == tm.tm_min) {
                    if (!has_deleted) printf("\n");
                    print_reminder(r, true);
                    delete_reminder(state->list, i);
                    has_deleted = true;
                }
            }
        }
        if (has_deleted) printf("> ");
        fflush(stdout);
        pthread_mutex_unlock(&lock);
    }

    return NULL;
}

bool is_empty(reminder_t reminder) {
    return strcmp(reminder.desc, "") == 0;
}

int find_empty_slot(list_t list) {
    int res = -1;
    for (int i = 0; i < LIST_SIZE; i++) {
        if (is_empty(list.items[i])) {
            res = i;
            break;
        }
    }
    return res;
}

void add_reminder(list_t *list, int index, int h, int m, const char *desc) {
    reminder_t *r = &list->items[index];
    r->h = h;
    r->m = m;
    snprintf(r->desc, sizeof(r->desc),"%s", desc);
    list->count++;
}

void delete_reminder(list_t *list, int index) {
    list->items[index] = (reminder_t) {
        .h = 0,
        .m = 0,
        .desc = ""
    };
    list->count--;
}

void print_reminder(reminder_t reminder, bool is_alert) {
    printf(is_alert ? "\033[1;31m%2d:%02d %s\033[m\n" : "%2d:%02d %s\n",
            reminder.h, reminder.m, reminder.desc);
}

void print_list(list_t list) {
    for (int i = 0; i < LIST_SIZE; i++) {
        if (!is_empty(list.items[i])) {
            print_reminder(list.items[i], false);
        }
    }
}

bool is_valid_time(int h, int m) {
    return h >= 0 && h < 24 && m >= 0 && m < 60;
}

bool get_desc(char *input, char *desc) {
    char *p = input;
    while (isdigit(*p) || *p == ':' || isspace(*p)) p++;
    return *strcpy(desc, p) != '\0';
}

