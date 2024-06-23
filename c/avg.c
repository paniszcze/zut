#include <stdio.h>

int main(void) {
    int num, sum = 0, count = 0;

    while (scanf("%d", &num) == 1) {
        sum += num;
        count++;
    }

    if (count != 0) printf("%f\n", (float)sum / count);

    return 0;
}
