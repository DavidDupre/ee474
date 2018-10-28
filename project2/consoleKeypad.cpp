#include "consoleKeypad.h"
#include "tft.h"
#include <Elegoo_GFX.h>
#include <TouchScreen.h>

// pins for the shield
#define YP A1
#define XM A2
#define YM 7
#define XP 6

#define MINPRESSURE 10
#define MAXPRESSURE 1000

Elegoo_GFX_Button buttonInc = Elegoo_GFX_Button();
Elegoo_GFX_Button buttonDec = Elegoo_GFX_Button();
TouchScreen touchScreen = TouchScreen(XP, YP, XM, YM, 300);

void consoleKeypadInit() {
    buttonInc.initButton(&tft, 90, 200, 50, 50, 2, RED, WHITE, "+", 4);
    buttonDec.initButton(&tft, 30, 200, 50, 50, 2, RED, WHITE, "-", 4);

    // draw the buttons once
    // this only works if the screen is never cleared again
    buttonInc.drawButton();
    buttonDec.drawButton();
}

void consoleKeypad(void *consoleKeypadData) {
    ConsoleKeypadData *data = (ConsoleKeypadData *) consoleKeypadData;

    // update the buttons' pressed state based on the touchscreen
    TSPoint point = touchScreen.getPoint();
    if (point.z > MINPRESSURE && point.z < MAXPRESSURE) {
        buttonInc.press(buttonInc.contains(point.x, point.y));
        buttonDec.press(buttonDec.contains(point.x, point.y));
    }

    // update the speed commands based on the buttons' pressed state
    *data->driveMotorSpeedInc = buttonInc.isPressed();
    *data->driveMotorSpeedDec = buttonDec.isPressed();
}
