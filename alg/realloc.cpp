#include <iostream>

static const int COUNT = 10;
static const int NEW_COUNT = 20;

int *_realloc(int *ptr, int cnt, int newCnt);

int main(void) {
    int *tab = new int[COUNT];
    for (int i = 0; i < COUNT; i++) tab[i] = i;

    int *newTab = _realloc(tab, COUNT, NEW_COUNT);

    for (int i = 0; i < NEW_COUNT; i++) std::cout << newTab[i] << " ";
    std::cout << std::endl;

    delete[] newTab;

    return 0;
}

int *_realloc(int *ptr, int cnt, int newCnt)
{
    int *newTab = new int[newCnt];
    for (int i = 0; i < cnt; i++) newTab[i] = ptr[i];

    delete[] ptr;
    return newTab;
}

