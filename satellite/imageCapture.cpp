#include "imageCapture.h"
#include "schedule.h"
#include "sharedVariables.h"
#include "fft.h"
#include <string.h>
#include <Arduino.h>


// number of bits used to represent an analogRead result
#define IMAGE_CAPTURE_RAW_SIZE  10

// maximum milli-volts for an analogRead
#define IMAGE_CAPTURE_MAX_MVOLTS 5000.0


TCB imageCaptureTCB;

ImageCaptureData imageCaptureData;

const char* const taskName = "Image Capture";

/*
 * This is a private, truly circular buffer for incoming data collected by the
 * ISR.
 */
volatile unsigned short imageDataRawest[IMAGE_CAPTURE_RAW_BUFFER_LENGTH];
volatile unsigned int imageDataRawestIndex;

void imageCaptureTimerInit();
void imageCaptureTimerEnable();
void imageCaptureTimerDisable();

// convert a raw analogRead measurement to +-2.5V
float imageCaptureRawToVolts(unsigned short raw);

void imageCaptureInit() {
    tcbInit(
        &imageCaptureTCB,
        &imageCaptureData,
        imageCapture,
        taskName,
        1
    );

    // zero out the arrays
    memset(imageDataRaw, 0, sizeof(imageDataRaw[0])
            * IMAGE_CAPTURE_RAW_BUFFER_LENGTH);
    memset(imageData, 0, sizeof(imageData[0])
            * IMAGE_CAPTURE_FREQ_BUFFER_LENGTH);

    // assign the data's arrays to the global arrays
    imageCaptureData.imageDataRaw = imageDataRaw;
    imageCaptureData.imageData = imageData;

    // initialize the analog pin
    pinMode(PIN_IMAGE_CAPTURE, INPUT);

    // initialize the ISR's buffer
    for (int i = 0; i < IMAGE_CAPTURE_RAW_BUFFER_LENGTH; i++) {
        imageDataRawest[i] = 0;
    }
    imageDataRawestIndex = 0;

    // initialize the timer0 interrupt at 7.5 kHz
    imageCaptureTimerInit();
}

void imageCaptureTimerInit() {
    // init timer registers
    TCCR5A = 0;
    TCCR5B = 0;
    TCNT5 = 0;

    // set compare match register for 256Hz increments
    /*
     * This uses 256 Hz so that the sample buffer is full of new data every
     * time the task is run. This rate means that the task can detect
     * frequencies up to 128 Hz (because of Nyquist theory).
     *
     * OCRn = clockSpeed / prescaler / frequency
     *      = 16 MHz / 8 / 256
     *      = 7813
     */
    OCR5A = 7813;

    // turn on CTC mode
    TCCR5B |= (1 << WGM52);

    // set CS51 bit for 8 prescaler
    TCCR5B |= (1 << CS51);

    // enable timer compare interrupt
    imageCaptureTimerEnable();
}

void imageCaptureTimerEnable() {
    TIMSK5 |= (1 << OCIE5A);
}

void imageCaptureTimerDisable() {
    TIMSK5 &= ~(1 << OCIE5A);
}

float imageCaptureRawToVolts(unsigned short raw) {
    int rawScale = 1 << IMAGE_CAPTURE_RAW_SIZE;
    int offsetted = ((int) raw) - rawScale / 2;
    return offsetted * IMAGE_CAPTURE_MAX_MVOLTS / rawScale;
}

void imageCapture(void *imageCaptureData) {
    ImageCaptureData *data = (ImageCaptureData *) imageCaptureData;

    int n = IMAGE_CAPTURE_RAW_BUFFER_LENGTH;

    // copy the latest readings into a buffer while the timer is disabled
    imageCaptureTimerDisable();
    for (int i = 0; i < n; i++) {
        unsigned int rawIdx = (imageDataRawestIndex + i) % n;
        data->imageDataRaw[i] = imageDataRawest[rawIdx];
    }
    imageCaptureTimerEnable();

    // This is kind of a lot of memory.
    // It could be easily optimized by removing the imageDataRaw buffer
    Complex samples[n];
    Complex tmp[n / 2];

    // copy the samples into the real part of the fft input
    for (int i = 0; i < n; i++) {
        samples[i].real = imageCaptureRawToVolts(data->imageDataRaw[i]);
    }

    // perform the fft
    fft(samples, tmp, n);
    float frequency = fftFrequency(samples, IMAGE_CAPTURE_FREQ_HZ, n);

    /*
     * Add the frequency to the output buffer.
     * This casts back from float to an int, but this is OK for the frequency
     * range we're expecting (35 Hz - 3.75 kHz).
     */
    // TODO use shared addToBuffer function
    for (int i = IMAGE_CAPTURE_FREQ_BUFFER_LENGTH - 1; i > 0; i--) {
        data->imageData[i] = data->imageData[i - 1];
    }
    data->imageData[0] = frequency;
}

// timer5 interrupt service routine
ISR(TIMER5_COMPA_vect){
    // put the next reading into the next spot of the circular raw buffer
    imageDataRawest[imageDataRawestIndex] = analogRead(PIN_IMAGE_CAPTURE);
    imageDataRawestIndex = (imageDataRawestIndex + 1)
            % IMAGE_CAPTURE_RAW_BUFFER_LENGTH;
}
