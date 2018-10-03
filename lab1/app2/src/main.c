#include <stdio.h>
#include <stdlib.h>

const unsigned int CYCLES_PER_SEC = 480000000;

const char letters[] = {'A', 'B', 'C', 'D'};

int main()
{
    while (1) {
		for (int j = 0; j < 4; j++) {
			printf("\r%c", letters[j]);
			for (volatile unsigned int i = 0; i < CYCLES_PER_SEC; i++);
			printf("\r ");
			for (volatile unsigned int i = 0; i < CYCLES_PER_SEC; i++);
		}
    }
    return 0;
}