#include <stdio.h>
#include <stdlib.h>

int get_value();
void print_hline(int len);
void print_rect(int height, int width);

int main() {
    int a, b;
    if ((a = get_value()) < 1 || (b = get_value()) < 1) {   // 0 and negative values are invalidated
        printf("Invalid input.\n");
        return EXIT_FAILURE;
    }
    print_rect(a, b);
    return EXIT_SUCCESS;
}

int get_value() {
    int ret;
    return (scanf("%d", &ret) != 1) ? -1 : ret;
}

void print_hline(int len) {
    for (int i = 0; i < len; i++) {
        printf("*");
    }
    printf("\n");
}

void print_rect(int height, int width) {
    // draw upper edge
    print_hline(width);
    // draw vertical line(s) if needed
    for (int i = 0; i < height - 2; i++) {                  // -2 to accomodate the upper and lower edge
        if (width == 1) {
            printf("*\n");
        } else {
            printf("*%*c", width - 1, '*');                 // -1 to accomodate the left edge in the padding
            printf("\n");
        }
    }
    // draw lower edge if needed
    if (height != 1) print_hline(width);
}

