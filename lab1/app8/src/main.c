#include "flash.h"


const char letters[] = {'A', 'B', 'C', 'D'};
const unsigned int delays_ms[] = {1000, 2000, 1000, 2000};

int main() {
    while (1) {
        for (int j = 0; j < 4; j++) {
            flash(delays_ms[j], letters[j]);
        }
    }
    return 0;
}
