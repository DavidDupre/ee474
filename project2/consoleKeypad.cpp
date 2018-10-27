#include "consoleKeypad.h"
#include "tft.h"
#include <Elegoo_GFX.h>

Elegoo_GFX_Button buttonInc = Elegoo_GFX_Button();
Elegoo_GFX_Button buttonDec = Elegoo_GFX_Button();

void consoleKeypadInit() {
    buttonInc.initButton(&tft, 90, 200, 50, 50, 2, RED, WHITE, "+", 4);
    buttonDec.initButton(&tft, 30, 200, 50, 50, 2, RED, WHITE, "-", 4);
}

void consoleKeypad(void *consoleKeypadData) {
    ConsoleKeypadData *data = (ConsoleKeypadData *) consoleKeypadData;
    buttonInc.drawButton();
    buttonDec.drawButton();
    *data->driveMotorSpeedInc = buttonInc.isPressed();
    *data->driveMotorSpeedDec = buttonDec.isPressed();
    Serial.print("pressed? ");
    Serial.println(buttonInc.isPressed());
}
