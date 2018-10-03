#include <stdio.h>
#include <stdlib.h>

#define CYCLES_PER_MSEC 480000


const char letters[] = {'A', 'B', 'C', 'D'};
const unsigned int delays_ms[] = {1000, 2000, 1000, 2000};

void delay(unsigned int delay_ms) {
    for (volatile unsigned int i = 0; i < CYCLES_PER_MSEC * delay_ms; i++);
    return;
}

void flash(unsigned int delay_ms, char letter) {
    printf("\r%c", letter);
    fflush(stdout);
    delay(delay_ms);
    printf("\r ");
    fflush(stdout);
    delay(delay_ms);
    return;
}

int main() {
    while (1) {
        for (int j = 0; j < 4; j++) {
            flash(delays_ms[j], letters[j]);
        }
    }
    return 0;
}
