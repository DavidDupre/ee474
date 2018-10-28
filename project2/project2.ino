#include "predefinedMacros.h"
#include "thrusterSubsystem.h"
#include "powerSubsystem.h"
#include "consoleDisplay.h"
#include "satelliteComs.h"
#include "warningAlarm.h"
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
    &thrusterCommand
};

WarningAlarmData warningAlarmData = {
    &batteryLow,
    &fuelLow,
    &batteryLevel,
    &fuelLevel
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

TCB warningAlarmTCB = {
    &warningAlarmData,
    warningAlarm,
    "Warning/Alarm",
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

    // initialize the task queue
#ifndef RUN_TESTS
    taskQueueInsert(&powerSubsystemTCB);
    taskQueueInsert(&thrusterSubsystemTCB);
    taskQueueInsert(&consoleDisplayTCB);
    taskQueueInsert(&satelliteComsTCB);
    taskQueueInsert(&warningAlarmTCB);
#endif

    Serial.begin(614400);
    tft.begin(TFT_IDENTIFIER);
    tft.setRotation(1);
    tft.fillScreen(BLACK);
}

void loop() {
#ifdef RUN_TESTS
    aunit::TestRunner::run();
#else
    schedule();
#endif  /* RUN_TESTS */
    return;
}
