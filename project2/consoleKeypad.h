#ifndef _CONSOLE_KEYPAD_H_
#define _CONSOLE_KEYPAD_H_


#define PIN_DRIVE_MOTOR_SPEED_INC 50
#define PIN_DRIVE_MOTOR_SPEED_DEC 51


typedef struct {
    bool *driveMotorSpeedInc;
    bool *driveMotorSpeedDec;
} ConsoleKeypadData;


/******************************************************************************
 * name: consoleKeypadInit
 *
 * inputs: void
 *
 * outputs: void
 *
 * description:
 *  consoleKeypadInit initializes the pins used by the console keypad.
 *
 * author: David Dupre
 *****************************************************************************/
void consoleKeypadInit();

/******************************************************************************
 * name: consoleKeypad
 *
 * inputs: consoleKeypadData
 *
 * outputs: void
 *
 * description:
 *  consoleKeypad queries the state of the solar panel motor speed buttons and
 *  sets driveMotorSpeedInc and driveMotorSpeedDec to indicate which buttons
 *  are currently pressed. Both may be pressed at once.
 *
 *  If less than two seconds have passes since the last time this function ran
 *  fully, the function will return immediately.
 *
 * psuedocode:
 *  driveMotorSpeedInc = is increase speed button high?
 *  driveMotorSpeedDec = is decrease speed button high?
 *
 * author: David Dupre
 *****************************************************************************/
void consoleKeypad(void *consoleKeypadData);

#endif  /* _CONSOLE_KEYPAD_H_ */
