#include "flash.h"
#include <stdio.h>

#define CYCLES_PER_MSEC 480000


void delay(const unsigned int* delay_ms) {
    for (volatile unsigned int i = 0; i < CYCLES_PER_MSEC * *delay_ms; i++);
    return;
}

void flash(const unsigned int* delay_ms, char letter) {
    printf("\r%c", letter);
    fflush(stdout);
    delay(delay_ms);
    printf("\r ");
    fflush(stdout);
    delay(delay_ms);
    return;
}
