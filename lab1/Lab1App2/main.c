#include <stdio.h>
#include <stdlib.h>

const unsigned int CYCLES_PER_SEC = 480000000;
const char letters[] = {'A', 'B', 'C', 'D'};

int main()
{
    int j = 0;
    while (1) {
        for (volatile unsigned int i = 0; i < CYCLES_PER_SEC; i++);
        printf("\r%c", letters[j]);
        j = (j + 1) % sizeof(letters);
    }
    return 0;
}
