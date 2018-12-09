#include "imageCapture.h"
#include "schedule.h"
#include "comsTransmit.h"
#include "fft.h"
#include <string.h>
#include <Arduino.h>
#include "comsReceive.h"
#include "sharedVariables.h"


// number of bits used to represent an analogRead result
#define IMAGE_CAPTURE_RAW_SIZE  10

// maximum milli-volts for an analogRead
#define IMAGE_CAPTURE_MAX_MVOLTS 5000.0

// Telemetry IDs unique to the entire satellite
// Keep this in sync with COSMOS
#define TLMID_IMAGE 6
#define TLMID_IMAGE_READY 7

// command IDs for commands over Serial
// must be unique for entire vehicle
// keep in sync with COSMOS
#define CMDID_IMAGE_CAPTURE 0
#define CMDID_IMAGE_SEND 1

// us per sample for 256 Hz sampling
#define SAMPLE_PERIOD 3906


TLM_PACKET {
    uint16_t frequency;
} ImagePacket;

// convert a raw analogRead measurement to +-2.5V
float imageCaptureRawToVolts(unsigned short raw);

TCB imageCaptureTCB;

ImageCaptureData imageCaptureData;

/*
 * This is a private, truly circular buffer for incoming data collected by the
 * ISR.
 */
volatile unsigned short imageDataRawest[IMAGE_CAPTURE_RAW_BUFFER_LENGTH];
volatile unsigned int imageDataRawestIndex;

static ImagePacket imagePacket;

bool capture;
bool send;

void imageCaptureInit() {
    tcbInit(
        &imageCaptureTCB,
        &imageCaptureData,
        imageCapture,
        TASKID_IMAGE,
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

    comsTxRegisterSender(BUS_SATELLITE, TLMID_IMAGE, sizeof(imagePacket),
            &imagePacket);
    comsTxRegisterSender(BUS_SATELLITE, TLMID_IMAGE_READY, 0, 0);
}

float imageCaptureRawToVolts(unsigned short raw) {
    int rawScale = 1 << IMAGE_CAPTURE_RAW_SIZE;
    int offsetted = ((int) raw) - rawScale / 2;
    return offsetted * IMAGE_CAPTURE_MAX_MVOLTS / rawScale;
}

void imageCapture(void *imageCaptureData) {
    ImageCaptureData *data = (ImageCaptureData *) imageCaptureData;

    int n = IMAGE_CAPTURE_RAW_BUFFER_LENGTH;

    if (command == 'S') {
        for (int i = 0; i < n; i++) {
            imageDataRawest[imageDataRawestIndex] = analogRead(PIN_IMAGE_CAPTURE);
            imageDataRawestIndex = (imageDataRawestIndex + 1)
                    % n;
            delayMicroseconds(SAMPLE_PERIOD);
        }

        // copy the latest readings into a buffer while the timer is disabled
        for (int i = 0; i < n; i++) {
            unsigned int rawIdx = (imageDataRawestIndex + i) % n;
            data->imageDataRaw[i] = imageDataRawest[rawIdx];
        }

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

        // notify the satellite that its image capture data is ready
        comsTxSend(TLMID_IMAGE_READY);

        command = '\0';
    }

    // send telemetry
    if (command == 'I') {
        imagePacket.frequency = data->imageData[0];
        comsTxSend(TLMID_IMAGE);

        command = '\0';
    }
}