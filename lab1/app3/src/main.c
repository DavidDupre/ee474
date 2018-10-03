#include <stdio.h>
#include <stdlib.h>

#define CYCLES_PER_MSEC 480000


const char letters[] = {'A', 'B', 'C', 'D'};
const unsigned int delays_ms[] = {1000, 2000, 1000, 2000};

int main() {
    while (1) {
        for (int j = 0; j < 4; j++) {
            printf("\r%c", letters[j]);
            for (volatile unsigned int i = 0; i < CYCLES_PER_MSEC * delays_ms[j]; i++);
            printf("\r ");
            for (volatile unsigned int i = 0; i < CYCLES_PER_MSEC * delays_ms[j]; i++);
        }
    }
    return 0;
}