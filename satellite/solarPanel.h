#ifndef _SOLAR_PANEL_H_
#define _SOLAR_PANEL_H_

#define PIN_SOLAR_PANEL_STOPPED 21
#define PIN_SOLAR_PANEL_OUTPUT  44


typedef enum {
    SOLAR_PANEL_DEPLOYED = 0,
    SOLAR_PANEL_DEPLOYING,
    SOLAR_PANEL_RETRACTING,
    SOLAR_PANEL_RETRACTED
} SolarPanelState;

typedef struct {
    SolarPanelState *solarPanelState;
    bool *solarPanelDeploy;
    bool *solarPanelRetract;
    bool *driveMotorSpeedInc;
    bool *driveMotorSpeedDec;
} SolarPanelControlData;


/******************************************************************************
 * name: solarPanelControlInit
 *
 * inputs: void
 *
 * outputs: void
 *
 * description:
 *  solarPanelControlInit initializes the pins and interrupts used by the
 *  solar panel and initializes the console keypad.
 *
 * author: David Dupre
 *****************************************************************************/
void solarPanelControlInit();

/******************************************************************************
 * name: solarPanelControl
 *
 * inputs: solarPanelData
 *
 * outputs: void
 *
 * description:
 *  solarPanelControl updates the solar panel motor speed using
 *  driveMotorSpeedInc and driveMotorSpeedDec. It begins deploying or
 *  retracting the solar panel based on solarPanelDeploy and solarPanelRetract
 *  commands.
 *
 * psuedocode:
 *  # update speed
 *  if driveMotorSpeedInc:
 *      speed += 5
 *  if driveMotorSpeedDec:
 *      speed -= 5
 *
 *  # process commands
 *  if solarPanelDeploy
 *          and solarPanelState is not deployed or deploying:
 *      solarPanelState = deploying
 *      add consoleKeypad to task queue
 *      solarPanelSpeed = maxSpeed / 2
 *  else if solarPanelRetract
 *          and solarPanelState is not retracted or retracting:
 *      solarPanelState = retracting
 *      add consoleKeypad to task queue
 *      solarPanelSpeed = maxSpeed / 2
 *  solarPanelDeploy = false
 *  solarPanelRetract = false
 *
 *  # output PWM
 *  if deploying:
 *      dutyCycle = dutyCycleStopped + speed
 *  else if retracting:
 *      dutyCycle = dutyCycleStopped - speed
 *  else:
 *      dutyCycle = dutyCycleStopped
 *  set pin to output PWM at dutyCycle
 *
 * author: David Dupre
 *****************************************************************************/
void solarPanelControl(void *solarPanelData);

#endif /* _SOLAR_PANEL_H_ */
