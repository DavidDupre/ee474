#ifndef _SHARED_VARIABLES_H_
#define _SHARED_VARIABLES_H_

#define BATTERY_LEVEL_BUFFER_LENGTH 16
#define BATTERY_TEMP_BUFFER_LENGTH 16
#define TRANSPORT_DISTANCE_BUFFER_LENGTH 8
#define IMAGE_CAPTURE_FREQ_BUFFER_LENGTH 16

#include "solarPanel.h"
#include <stdint.h>
#include "transportDistance.h"
#include "consoleDisplay.h"


/*
 * These variables must have global scope.
 *
 * They should normally be accessed through data in Task Control Blocks.
 * However, some must also be accessed in Interrupt Service Routines which
 * are not given TCBs.
 */

extern unsigned int thrusterCommand;
extern unsigned short fuelLevel;
extern SolarPanelState solarPanelState;
extern bool solarPanelDeploy;
extern bool solarPanelRetract;
extern volatile unsigned int batteryLevelPtr[BATTERY_LEVEL_BUFFER_LENGTH];
extern volatile unsigned int batteryTempPtr[BATTERY_TEMP_BUFFER_LENGTH];
extern volatile float distanceBufferPtr[TRANSPORT_DISTANCE_BUFFER_LENGTH];
extern unsigned short powerConsumption;
extern unsigned short powerGeneration;
extern bool batteryLow;
extern bool fuelLow;
extern bool driveMotorSpeedInc;
extern bool driveMotorSpeedDec;
extern char vehicleCommand;
extern char vehicleResponse;
extern uint8_t numCmdErrors;
extern bool batteryTempHigh;
extern bool detected;
extern unsigned int pirateProximity;

extern bool temperatureAlarmAcked;

extern unsigned int imageData[IMAGE_CAPTURE_FREQ_BUFFER_LENGTH];

extern ConsoleStatus cStatus;

#endif /* _SHARED_VARIABLES_H_ */