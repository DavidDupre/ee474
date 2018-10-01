#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const unsigned int CYCLES_PER_SEC = 480000000;

int main()
{
    int j = 0;
    while (1) {
        printf("\rA");
        for (volatile unsigned int i = 0; i < CYCLES_PER_SEC; i++);
        printf("\rB");
        for (volatile unsigned int i = 0; i < CYCLES_PER_SEC * 2; i++);
        printf("\rC");
        for (volatile unsigned int i = 0; i < CYCLES_PER_SEC; i++);
        printf("\rD");
        for (volatile unsigned int i = 0; i < CYCLES_PER_SEC * 2; i++);
    }
    return 0;
}
