#include <stdio.h>
#include <stdlib.h>

const unsigned int CYCLES_PER_SEC = 480000000;

int main()
{
    while (1) {
        printf("\rA B C D");
		for (volatile unsigned int i = 0; i < CYCLES_PER_SEC; i++);
        printf("\r       ");
		for (volatile unsigned int i = 0; i < CYCLES_PER_SEC; i++);
    }
    return 0;
}