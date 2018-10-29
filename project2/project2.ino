#include "predefinedMacros.h"
#include "thrusterSubsystem.h"
#include "powerSubsystem.h"
#include "consoleDisplay.h"
#include "satelliteComs.h"
#include "vehicleComms.h"
#include "warningAlarm.h"
#include "consoleKeypad.h"
#include "schedule.h"
#include "tft.h"

#include <AUnit.h>  // Test framework


unsigned int thrusterCommand;
unsigned short fuelLevel;
bool solarPanelState;
unsigned short batteryLevel;
unsigned short powerConsumption;
unsigned short powerGeneration;
bool batteryLow;
bool fuelLow;
bool driveMotorSpeedInc;
bool driveMotorSpeedDec;
char vehicleCommand;
char vehicleResponse;

ThrusterSubsystemData thrusterSubsystemData = {
    &thrusterCommand,
    &fuelLevel
};

PowerSubsystemData powerSubsystemData = {
    &solarPanelState,
    &batteryLevel,
    &powerConsumption,
    &powerGeneration
};

ConsoleDisplayData consoleDisplayData = {
    &solarPanelState,
    &batteryLevel,
    &fuelLevel,
    &powerConsumption,
    &powerGeneration,
    &batteryLow,
    &fuelLow
};

SatelliteComsData satelliteComsData = {
    &fuelLow,
    &batteryLow,
    &solarPanelState,
    &batteryLevel,
    &fuelLevel,
    &powerConsumption,
    &powerGeneration,
    &thrusterCommand,
    &vehicleResponse
};

VehicleCommsData vehicleCommsData = {
    &vehicleCommand,
    &vehicleResponse
};

WarningAlarmData warningAlarmData = {
    &batteryLow,
    &fuelLow,
    &batteryLevel,
    &fuelLevel
};

ConsoleKeypadData consoleKeypadData = {
    &driveMotorSpeedInc,
    &driveMotorSpeedDec
};

TCB powerSubsystemTCB = {
    &powerSubsystemData,
    powerSubsystem,
    "Power Subsystem",
    NULL, NULL
};

TCB thrusterSubsystemTCB = {
    &thrusterSubsystemData,
    thrusterSubsystem,
    "Thruster Subsystem",
    NULL, NULL
};

TCB consoleDisplayTCB = {
    &consoleDisplayData,
    consoleDisplay,
    "Console Display",
    NULL, NULL
};

TCB satelliteComsTCB = {
    &satelliteComsData,
    satelliteComs,
    "Satellite Communications",
    NULL, NULL
};

TCB vehicleCommsTCB = {
    &vehicleCommsData,
    vehicleComms,
    "Vehicle Communications",
    NULL, NULL
};

TCB warningAlarmTCB = {
    &warningAlarmData,
    warningAlarm,
    "Warning/Alarm",
    NULL, NULL
};

TCB consoleKeypadTCB = {
    &consoleKeypadData,
    consoleKeypad,
    "Console Keypad",
    NULL, NULL
};

void setup() {
    thrusterCommand = 0;
    fuelLevel = 100;
    fuelLow = false;
    batteryLow = false;
    solarPanelState = false;
    batteryLevel = 100;
    powerConsumption = 0;
    powerGeneration = 0;

    Serial.begin(9600);
    Serial1.begin(9600);
    tft.begin(tft.readID());
    tft.setRotation(1);
    tft.fillScreen(BLACK);

    consoleKeypadInit();

    // initialize the task queue
#ifndef RUN_TESTS
    taskQueueInsert(&powerSubsystemTCB);
    taskQueueInsert(&thrusterSubsystemTCB);
    taskQueueInsert(&consoleDisplayTCB);
    taskQueueInsert(&satelliteComsTCB);
    taskQueueInsert(&warningAlarmTCB);
    taskQueueInsert(&consoleKeypadTCB);
    taskQueueInsert(&vehicleCommsTCB);
#endif
}

void loop() {
#ifdef RUN_TESTS
    aunit::TestRunner::run();
#else
    schedule();
#endif  /* RUN_TESTS */
    return;
}
