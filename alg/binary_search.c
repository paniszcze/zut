#include <stdio.h>

int bsearch(int *tab, int first, int last, int num);

int main(void) {
    int tab[] = { 0, 2, 6, 8, 9, 10, 13 };
    int size = sizeof(tab) / sizeof(tab[0]);

    int num = 6;
    int i = bsearch(tab, 0, size - 1, num);

    if (i < 0) {
        printf("Number %d not found\n", num);
    } else {
        printf("Number %d is located at index %d\n", num, i);
    }

    return 0;
}

int bsearch(int *tab, int first, int last, int num) {
    int index = -1;
    int middle = first + (last - first) / 2;

    if (last >= first) {
        if (tab[middle] == num) {
            return middle;
        } else if (tab[middle] > num) {
            index = bsearch(tab, first, middle - 1, num);
        } else {
            index = bsearch(tab, middle + 1, last, num);
        }
    }

    return index;
}

