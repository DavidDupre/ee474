#include "comsReceive.h"
#include "imageCapture.h"

void setup() {
    Serial.begin(9600);

    comsRxInit(); // must be called first
    imageCaptureInit();
}

void loop() {
    comsRxUpdate();
    imageCapture();
    delay(500);
}