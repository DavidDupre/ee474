#pragma once

#include "schedule.h"

#define IMAGE_CAPTURE_FREQ_BUFFER_LENGTH 16
#define IMAGE_CAPTURE_RAW_BUFFER_LENGTH  256 // must be a power of 2

#define PIN_IMAGE_CAPTURE A11

// The sample frequency. Keep this in sync with the timer interrupt.
#define IMAGE_CAPTURE_FREQ_HZ 256


typedef struct {
    unsigned int *imageData;
    unsigned short *imageDataRaw;
} ImageCaptureData;


extern TCB imageCaptureTCB;

/******************************************************************************
 * name: imageCaptureInit
 *
 * inputs: void
 *
 * outputs: void
 *
 * description:
 *  - initializes TCB data and local variables
 *  - configures PIN_IMAGE_CAPTURE to output mode
 *  - starts a 256 Hz timer interrupt on timer 0
 *
 * author: David Dupre
 *****************************************************************************/
void imageCaptureInit();

/******************************************************************************
 * name: imageCapture
 *
 * inputs: imageCaptureData
 *
 * outputs: void
 *
 * description:
 *  This performs a FFT on the last 256 samples collected by the image capture
 *  timer interrupt service routine. The samples used in the FFT are copied
 *  into imageDataRaw sorted from most recent to least recent sample, starting
 *  from index 0. The last 16 calculated frequencies are stored in imageData
 *  from most recent to least recent. Each call to imageCapture will add one
 *  entry to imageData.
 *
 * author: David Dupre
 *****************************************************************************/
void imageCapture(void *imageCaptureData);