#pragma once

#include <stdint.h>

// the maximum sample value of input to fft
#define FFT_SAMPLE_MAX 32

int16_t fft(int16_t x[256], int16_t y[256]);
