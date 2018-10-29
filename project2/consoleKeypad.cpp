#include "consoleKeypad.h"
#include "tft.h"
#include <Elegoo_GFX.h>
#include <TouchScreen.h>
#include <Arduino.h>

#define MINPRESSURE 10
#define MAXPRESSURE 1000

Elegoo_GFX_Button buttonInc = Elegoo_GFX_Button();
Elegoo_GFX_Button buttonDec = Elegoo_GFX_Button();

void consoleKeypadInit() {
    buttonInc.initButton(&tft, 90, 200, 50, 50, 2, RED, WHITE, "+", 4);
    buttonDec.initButton(&tft, 30, 200, 50, 50, 2, RED, WHITE, "-", 4);

    // draw the buttons once
    // this only works if the screen is never cleared again
    buttonInc.drawButton();
    buttonDec.drawButton();

    pinMode(13, OUTPUT);
}

void consoleKeypad(void *consoleKeypadData) {
    ConsoleKeypadData *data = (ConsoleKeypadData *) consoleKeypadData;

    pinMode(XM, INPUT);
    pinMode(YP, INPUT);

    digitalWrite(13, HIGH);
    TSPoint point = touchScreen.getPoint();
    digitalWrite(13, LOW);

    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);

    Serial.print(" ");
    Serial.print(point.z);

    // update the buttons' pressed state based on the touchscreen
    if (point.z > MINPRESSURE && point.z < MAXPRESSURE) {
        buttonInc.press(buttonInc.contains(point.x, point.y));
        buttonDec.press(buttonDec.contains(point.x, point.y));
    }

    // update the speed commands based on the buttons' pressed state
    *data->driveMotorSpeedInc = buttonInc.isPressed();
    *data->driveMotorSpeedDec = buttonDec.isPressed();
}
