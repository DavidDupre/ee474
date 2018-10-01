#include <stdio.h>
#include <stdlib.h>

const unsigned long CYCLES_PER_MSEC = 480000;

void delay(unsigned long ms) {
    for (volatile unsigned long i = 0; i < CYCLES_PER_MSEC * ms; i++);
}

int main()
{
    while (1) {
        printf("\rA");
        delay(1000);
        printf("\rB");
        delay(2000);
        printf("\rC");
        delay(1000);
        printf("\rD");
        delay(2000);
    }
    return 0;
}
