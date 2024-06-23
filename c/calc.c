#include <stdio.h>

int main(void) {
    int a, b;
    char op;

    if (scanf("%d", &a) == 1 && scanf(" %c", &op) && scanf("%d", &b)) {
        switch (op) {
            case '+':
                printf("%d + %d = %d\n", a, b, a + b);
                break;
            case '-':
                printf("%d - %d = %d\n", a, b, a - b);
                break;
            case '/':
                if (b == 0) {
                    printf("Division by 0 is bad. Bye\n");
                } else {
                    printf("%d / %d = %f\n", a, b, (float)a / b); 
                }
                break;
            case '*':
                printf("%d * %d = %d\n", a, b, a * b);
                break;
            default:
                printf("Unknown operation. Bye\n");
                break;
        }
    } else {
        printf("Invalid input. Bye\n");
    }

    return 0;
}

