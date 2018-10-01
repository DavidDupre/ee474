#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const unsigned int CYCLES_PER_SEC = 480000000;

int main()
{
    while (1) {
        for (volatile unsigned int i = 0; i < CYCLES_PER_SEC; i++);
        printf("A B C D\n");
    }
    return 0;
}
