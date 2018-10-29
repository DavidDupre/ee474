#include "consoleKeypad.h"
#include <Arduino.h>

void consoleKeypadInit() {
    pinMode(PIN_DRIVE_MOTOR_SPEED_INC, INPUT);
    pinMode(PIN_DRIVE_MOTOR_SPEED_DEC, INPUT);
}

void consoleKeypad(void *consoleKeypadData) {
    ConsoleKeypadData *data = (ConsoleKeypadData *) consoleKeypadData;

    // update the speed commands based on the buttons' pressed state
    *data->driveMotorSpeedInc = digitalRead(PIN_DRIVE_MOTOR_SPEED_INC) == HIGH;
    *data->driveMotorSpeedDec = digitalRead(PIN_DRIVE_MOTOR_SPEED_DEC) == HIGH;
}
