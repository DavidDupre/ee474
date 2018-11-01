#ifndef _CONSOLE_KEYPAD_H_
#define _CONSOLE_KEYPAD_H_


#define PIN_DRIVE_MOTOR_SPEED_INC 50
#define PIN_DRIVE_MOTOR_SPEED_DEC 51


typedef struct {
    bool *driveMotorSpeedInc;
    bool *driveMotorSpeedDec;
} ConsoleKeypadData;


void consoleKeypadInit();

void consoleKeypad(void *consoleKeypadData);

#endif  /* _CONSOLE_KEYPAD_H_ */
