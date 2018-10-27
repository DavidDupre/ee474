#ifndef _CONSOLE_KEYPAD_H_
#define _CONSOLE_KEYPAD_H_


typedef struct {
    bool *driveMotorSpeedInc;
    bool *driveMotorSpeedDec;
} ConsoleKeypadData;


void consoleKeypadInit();

void consoleKeypad(void *consoleKeypadData);

#endif  /* _CONSOLE_KEYPAD_H_ */
