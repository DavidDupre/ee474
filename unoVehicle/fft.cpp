#include "fft.h"
#include <math.h>


Complex::Complex() {
    real = 0;
    imag = 0;
}

Complex::Complex(float r, float i) {
    real = r;
    imag = i;
}

Complex Complex::operator+(const Complex &other) {
    return Complex(real + other.real, imag + other.imag);
}

Complex Complex::operator-(const Complex &other) {
    return Complex(real - other.real, imag - other.imag);
}

Complex Complex::operator*(const Complex &other) {
    float r = real * other.real - imag * other.imag;
    float i = real * other.imag + imag * other.real;
    return Complex(r, i);
}

Complex Complex::expc() {
    float ex = exp(real);
    float r = ex * cos(imag);
    float i = ex * sin(imag);
    return Complex(r, i);
}

float Complex::mag2() {
    return real * real + imag * imag;
}


// separate even/odd elements to lower/upped halves fo the array
void separate(Complex *samples, Complex *tmp, int n) {
    // copy odd elements to tmp
    for (int i = 0; i < n / 2; i++) {
        tmp[i] = samples[i * 2 + 1];
    }

    // copy even elements to lower half of samples
    for (int i = 0; i < n / 2; i++) {
        samples[i] = samples[i * 2];
    }

    // copy odd elements to upper half of samples
    for (int i = 0; i < n / 2; i++) {
        samples[i + n / 2] = tmp[i];
    }
}

void fft(Complex *samples, Complex *tmp, int n) {
    if (n < 2) {
        // end of recursion
        return;
    }

    // send all even samples to lower half and all odd samples to upper half
    separate(samples, tmp, n);

    fft(samples, tmp, n / 2);         // recurse even samples
    fft(samples + n / 2, tmp, n / 2); // recurse odd samples

    // combine results of recursions
    for (int k = 0; k < n / 2; k++) {
        Complex e = samples[k];         // even
        Complex o = samples[k + n / 2]; // odd
        Complex w = Complex(0, -2 * M_PI * k / n).expc();
        samples[k] = e + w * o;
        samples[k + n / 2] = e - w * o;
    }
}

float fftFrequency(Complex *fftResult, float sampleFreq, int n) {
    // get the index of the max result
    int maxIndex = 0;
    float maxMag2 = 0;
    for (int i = 0; i < n / 2; i++) {
        float thisMag2 = fftResult[i].mag2();
        if (thisMag2 > maxMag2) {
            maxMag2 = thisMag2;
            maxIndex = i;
        }
    }

    // determine frequency from the index
    return sampleFreq * maxIndex / n;
}