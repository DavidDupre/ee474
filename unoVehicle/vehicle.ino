#include "comsReceive.h"
#include "comsTransmit.h"
#include "imageCapture.h"
#include "schedule.h"

uint8_t numCmdErrors;

unsigned short imageDataRaw[IMAGE_CAPTURE_RAW_BUFFER_LENGTH];
unsigned int imageData[IMAGE_CAPTURE_FREQ_BUFFER_LENGTH];

void setup() {
    Serial.begin(9600);

    comsTxInit(); // must be called first
    comsRxInit(); // must be called before other inits but after comsTxInit
    imageCaptureInit();

    imageCaptureTCB.priority = 1;
    taskQueueInsert(&imageCaptureTCB);

    comsRxTCB.priority = 2;
    taskQueueInsert(&comsRxTCB);

    comsTxTCB.priority = 2;
    taskQueueInsert(&comsTxTCB);
}

void loop() {
    schedule();
}