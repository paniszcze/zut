#include <stdio.h>
#include <stdbool.h>

#define array_size(x) sizeof(x) / (sizeof(*(x)))

void bubble_sort(int *array, int size);
void print_array(int *array, int size);

int main(void) {
    int nums[] = { 9, 7, 3, 2, 5, 1, 8, 6, 8, 0 };

    printf("[ 0] ");
    print_array(nums, array_size(nums));

    bubble_sort(nums, array_size(nums));

    return 0;
}

void bubble_sort(int *array, int size) {
    bool has_swapped = false;
    int count = 0;
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - i - 1; j++) {
            if (array[j] > array[j + 1]) {
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
                has_swapped = true;
                count++;
                printf("[%2d] ", count);
                print_array(array, size);
            }
        }
        if (!has_swapped) return;
    }
}

void print_array(int *array, int size) {
    for (int i = 0; i < size; i++) {
        printf("%d ", array[i]);
    }
    printf("\n");
}

