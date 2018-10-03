#include <stdio.h>

#include "flash.h"

const unsigned int CYCLES_PER_MSEC = 480000;

void delay (unsigned int delay_ms) {
	for (volatile unsigned int i = 0; i < CYCLES_PER_MSEC * delay_ms; i++);
}

void flash(unsigned int delay_ms, char letter) {
	printf("\r%c", letter);
	fflush(stdout);
	delay(delay_ms);
	printf("\r ");
	fflush(stdout);
	delay(delay_ms);
}
