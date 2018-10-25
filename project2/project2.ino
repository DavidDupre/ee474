#include "predefinedMacros.h"
#include "thrusterSubsystem.h"
#include "powerSubsystem.h"
#include "consoleDisplay.h"
#include "satelliteComs.h"
#include "warningAlarm.h"
#include "schedule.h"
#include "tcb.h"
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
    powerSubsystem
};

TCB thrusterSubsystemTCB = {
    &thrusterSubsystemData,
    thrusterSubsystem
};

TCB consoleDisplayTCB = {
    &consoleDisplayData,
    consoleDisplay
};

TCB satelliteComsTCB = {
    &satelliteComsData,
    satelliteComs
};

TCB warningAlarmTCB = {
    &warningAlarmData,
    warningAlarm
};

TCB *taskQueue[] = {
    &powerSubsystemTCB,
    &thrusterSubsystemTCB,
    &satelliteComsTCB,
    &consoleDisplayTCB,
    &warningAlarmTCB,
    // blink LED? Maybe not a task
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
    tft.begin(TFT_IDENTIFIER);
    tft.setRotation(1);
    tft.fillScreen(BLACK);
}

void loop() {
#ifdef RUN_TESTS
    aunit::TestRunner::run();
#else
    schedule(taskQueue, sizeof(taskQueue) / sizeof(TCB*));
#endif  /* RUN_TESTS */
    return;
}
