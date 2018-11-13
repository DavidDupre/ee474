#include "imageCapture.h"
#include "schedule.h"
#include "sharedVariables.h"
#include <string.h>
#include <Arduino.h>


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
    memset(imageDataRawest, 0, sizeof(imageDataRawest[0])
            * IMAGE_CAPTURE_RAW_BUFFER_LENGTH);
    imageDataRawestIndex = 0;

    // initialize the timer0 interrupt at 7.5 kHz
    imageCaptureTimerInit();
}

void imageCaptureTimerInit() {
    // init timer registers
    TCCR0A = 0;
    TCCR0B = 0;
    TCNT0 = 0;

    // set compare match register for 256Hz increments
    /*
     * This uses 256 Hz so that the sample buffer is full of new data every
     * time the task is run. This rate means that the task can detect
     * frequencies up to 128 Hz (because of Nyquist theory).
     *
     * OCRn = clockSpeed / prescaler / frequency - 1
     *      = 16 MHz / 1024 / 256 - 1
     *      = 15369 / 256
     *      ~ 60
     */
    OCR0A = 60;

    // turn on CTC mode
    TCCR0A |= (1 << WGM01);

    // set CS02 and CS00 bits for 1024 prescaler
    TCCR0B |= (1 << CS02) | (1 << CS00);

    // enable timer compare interrupt
    imageCaptureTimerEnable();
}

void imageCaptureTimerEnable() {
    TIMSK0 |= (1 << OCIE0A);
}

void imageCaptureTimerDisable() {
    TIMSK0 &= ~(1 << OCIE0A);
}

void imageCapture(void *imageCaptureData) {
    ImageCaptureData *data = (ImageCaptureData *) imageCaptureData;

    // copy the latest readings into a buffer while the timer is disabled
    // TODO what if the timer is disabled during the ISR? Should this use a mutex? What kind?
    imageCaptureTimerDisable();
    for (unsigned int i = 0; i < IMAGE_CAPTURE_RAW_BUFFER_LENGTH; i++) {
        unsigned int rawIdx = (imageDataRawestIndex + i)
                % IMAGE_CAPTURE_RAW_BUFFER_LENGTH;
        data->imageDataRaw[i] = imageDataRawest[rawIdx];
    }
    imageCaptureTimerEnable();

    // TODO do the FFT
}

// timer0 interrupt at 256Hz
ISR(TIMER0_COMPA_vect){
    // Put the next reading into the next spot of the raw buffer
    imageDataRawest[imageDataRawestIndex] = analogRead(PIN_IMAGE_CAPTURE);
    imageDataRawestIndex = (imageDataRawestIndex + 1) % IMAGE_CAPTURE_RAW_BUFFER_LENGTH;
}
