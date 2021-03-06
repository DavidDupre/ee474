#include "imageCapture.h"
#include "comsTransmit.h"
#include "fft.h"
#include <string.h>
#include <Arduino.h>
#include "comsReceive.h"


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
#define CMDID_IMAGE 10

// us per sample for 7500 Hz sampling
#define SAMPLE_PERIOD 133


TLM_PACKET {
    uint16_t frequency;
} ImagePacket;

// convert a raw analogRead measurement to +-2.5V
float imageCaptureRawToVolts(unsigned short raw);

// handle a command from the satellite
// returns true iff handled
bool handleCommand(char opcode, uint8_t *data);


// This is kind of a lot of memory.
Complex samples[IMAGE_CAPTURE_RAW_BUFFER_LENGTH];
Complex tmp[IMAGE_CAPTURE_RAW_BUFFER_LENGTH / 2];

static ImagePacket imagePacket;

char command;

void imageCaptureInit() {
    // initialize the analog pin
    pinMode(PIN_IMAGE_CAPTURE, INPUT);

    comsRxRegisterCallback(CMDID_IMAGE, handleCommand);
}

float imageCaptureRawToVolts(unsigned short raw) {
    int rawScale = 1 << IMAGE_CAPTURE_RAW_SIZE;
    int offsetted = ((int) raw) - rawScale / 2;
    return offsetted * IMAGE_CAPTURE_MAX_MVOLTS / rawScale;
}

void imageCapture() {
    if (command == 'S') {
        int n = IMAGE_CAPTURE_RAW_BUFFER_LENGTH;

        // copy the samples into the real part of the fft input
        for (int i = 0; i < n; i++) {
            long startTime = micros();
            int rawSample = analogRead(PIN_IMAGE_CAPTURE);
            samples[i].real = imageCaptureRawToVolts(rawSample);
            samples[i].imag = 0;
            long timePassed = micros() - startTime;
            if (timePassed < SAMPLE_PERIOD) {
                delayMicroseconds(SAMPLE_PERIOD - timePassed);
            }
        }

        // perform the fft
        fft(samples, tmp, n);
        imagePacket.frequency = fftFrequency(samples, IMAGE_CAPTURE_FREQ_HZ, n);

        // notify the satellite that its image capture data is ready
        comsTxSend(TLMID_IMAGE_READY, 0, 0);
    } else if (command == 'I') {
        // send telemetry
        comsTxSend(TLMID_IMAGE, &imagePacket, sizeof(imagePacket));
    }

    command = '\0';
}

bool handleCommand(char opcode, uint8_t *data) {
    if (opcode == 'S' || opcode == 'I') {
        command = opcode;
        return true;
    }
    return false;
}
