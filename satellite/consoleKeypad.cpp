#include "consoleKeypad.h"
#include "schedule.h"
#include "sharedVariables.h"
#include <Arduino.h>


TCB consoleKeypadTCB;

ConsoleKeypadData consoleKeypadData = {
    &driveMotorSpeedInc,
    &driveMotorSpeedDec
};

const char* const taskName = "Console Keypad";

void consoleKeypadInit() {
    tcbInit(
        &consoleKeypadTCB,
        &consoleKeypadData,
        consoleKeypad,
        taskName,
        1
    );

    pinMode(PIN_DRIVE_MOTOR_SPEED_INC, INPUT);
    pinMode(PIN_DRIVE_MOTOR_SPEED_DEC, INPUT);
}

void consoleKeypad(void *consoleKeypadData) {
    // return early if less than 2 seconds have passed
    static unsigned long lastRunTime;
    if (globalTimeBase() - lastRunTime < 2000) {
        return;
    }
    lastRunTime = globalTimeBase();

    ConsoleKeypadData *data = (ConsoleKeypadData *) consoleKeypadData;

    // update the speed commands based on the buttons' pressed state
    *data->driveMotorSpeedInc = digitalRead(PIN_DRIVE_MOTOR_SPEED_INC) == HIGH;
    *data->driveMotorSpeedDec = digitalRead(PIN_DRIVE_MOTOR_SPEED_DEC) == HIGH;
}
