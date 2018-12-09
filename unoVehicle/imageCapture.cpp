#include "imageCapture.h"
#include "comsTransmit.h"
#include "fft.h"
#include <string.h>
#include <Arduino.h>
#include "comsReceive.h"


// Telemetry IDs unique to the entire satellite
// Keep this in sync with COSMOS
#define TLMID_IMAGE 6
#define TLMID_IMAGE_READY 7

// command IDs for commands over Serial
// must be unique for entire vehicle
#define CMDID_IMAGE 10

// us per sample for 256 Hz sampling
#define SAMPLE_PERIOD 3906

// the max value from an analog read
#define ANALOG_READ_MAX 1024


TLM_PACKET {
    uint16_t frequency;
} ImagePacket;

// convert a raw analogRead measurement to -31 to 32
int imageCaptureConvert(unsigned short raw);

bool handleCommand(char opcode, uint8_t *data);

// This is kind of a lot of memory.
int16_t fft_real[IMAGE_CAPTURE_RAW_BUFFER_LENGTH];
int16_t fft_imag[IMAGE_CAPTURE_RAW_BUFFER_LENGTH];

static ImagePacket imagePacket;

char command;

void imageCaptureInit() {
    // initialize the analog pin
    pinMode(PIN_IMAGE_CAPTURE, INPUT);
    comsRxRegisterCallback(CMDID_IMAGE, handleCommand);
}

int imageCaptureConvert(unsigned short raw) {
    return ((int) raw / (ANALOG_READ_MAX / (FFT_SAMPLE_MAX * 2))) - FFT_SAMPLE_MAX;
}

void imageCapture() {
    Serial.println(F("\nin imageCapture"));

    if (command == 'S') {
        Serial.println(F("\ngot S"));

        // copy the samples into the real part of the fft input
        for (uint16_t i = 0; i < IMAGE_CAPTURE_RAW_BUFFER_LENGTH; i++) {
            // Serial.print(F("interation "));
            // Serial.println(i);
            short rawSample = analogRead(PIN_IMAGE_CAPTURE);
            fft_real[i] = imageCaptureConvert(rawSample);
            fft_imag[i] = 0;
            delayMicroseconds(SAMPLE_PERIOD);
        }

        // perform the fft
        imagePacket.frequency = fft(fft_real, fft_imag);
        Serial.print("\ngot freq: ");
        Serial.println(imagePacket.frequency);

        // notify the satellite that its image capture data is ready
        sendTelemetryPacket(BUS_SATELLITE, TLMID_IMAGE_READY, 0, 0);

        command = '\0';
    }

    // send telemetry
    if (command == 'I') {
        sendTelemetryPacket(BUS_SATELLITE, TLMID_IMAGE, (uint8_t *) &imagePacket, sizeof(imagePacket));

        command = '\0';
    }
}

bool handleCommand(char opcode, uint8_t *data) {
    switch (opcode) {
        case 'S': // pass through
        case 'I':
            command = opcode;
            break;
        default:
            return false;
    }
    return true;
}
