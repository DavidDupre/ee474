#pragma once


class Complex {
    public:

    float real;
    float imag;

    /**
     * Construct a complex number with 0, 0.
     */
    Complex();

    /**
     * Construct a complex number
     *  r: real part
     *  i: imaginary part
     */
    Complex(float r, float i);

    Complex operator+(const Complex &other);

    Complex operator-(const Complex &other);

    Complex operator*(const Complex &other);

    /**
     * Return e^this
     *
     * It's called "expc" to avoid a naming conflict with avr's math.h
     */
    Complex expc();

    /**
     * Return the squared magnitude
     */
    float mag2();
};

/******************************************************************************
 * name: fft
 *
 * inputs:
 *  samples: array of samples. Output is stored in the first n/2 samples.
 *  tmp:     temporary storage of length n/2
 *  n:       number of samples. Must be a power of 2.
 *
 * outputs: void
 *
 * author: David Dupre
 *****************************************************************************/
void fft(Complex *samples, Complex *tmp, int n);

/******************************************************************************
 * name: fftFrequency
 *
 * inputs:
 *  fftResult:  the output of fft (array of length n/2)
 *  sampleFreq: the sample frequency
 *  n:          number of samples. Must be a power of 2.
 *
 * outputs: the frequency
 *
 * author: David Dupre
 *****************************************************************************/
float fftFrequency(Complex *fftResult, float sampleFreq, int n);
