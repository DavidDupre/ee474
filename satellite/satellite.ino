#include "predefinedMacros.h"
#include "sharedVariables.h"
#include "thrusterSubsystem.h"
#include "powerSubsystem.h"
#include "consoleDisplay.h"
#include "satelliteComs.h"
#include "vehicleComms.h"
#include "warningAlarm.h"
#include "solarPanel.h"
#include "schedule.h"
#include "tft.h"

#include <AUnit.h>  // Test framework


unsigned int thrusterCommand;
unsigned short fuelLevel;
SolarPanelState solarPanelState;
bool solarPanelDeploy;
bool solarPanelRetract;
unsigned short powerConsumption;
unsigned short powerGeneration;
bool batteryLow;
bool fuelLow;
volatile unsigned int batteryLevelPtr[BATTERY_LEVEL_BUFFER_LENGTH];
volatile unsigned int batteryTempPtr[BATTERY_TEMP_BUFFER_LENGTH];
bool driveMotorSpeedInc;
bool driveMotorSpeedDec;
char vehicleCommand;
char vehicleResponse;
bool batteryTempHigh;
unsigned int batteryTemp;

ThrusterSubsystemData thrusterSubsystemData = {
    &thrusterCommand,
    &fuelLevel
};

PowerSubsystemData powerSubsystemData = {
    &solarPanelState,
    &solarPanelDeploy,
    &solarPanelRetract,
    batteryLevelPtr,
    batteryTempPtr,
    &powerConsumption,
    &powerGeneration,
    &batteryTempHigh,
};

ConsoleDisplayData consoleDisplayData = {
    &solarPanelState,
    batteryLevelPtr,
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
    batteryLevelPtr,
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
    batteryLevelPtr,
    &fuelLevel,
    &batteryTempHigh
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



void setup() {
    thrusterCommand = 0;
    fuelLevel = 100;
    solarPanelState = SOLAR_PANEL_RETRACTED;
    memset((unsigned int *) batteryLevelPtr, 0, BATTERY_LEVEL_BUFFER_LENGTH);
    memset((unsigned int *) batteryTempPtr, 0, BATTERY_LEVEL_BUFFER_LENGTH);
    solarPanelDeploy = false;
    solarPanelRetract = false;
    powerConsumption = 0;
    powerGeneration = 0;
    batteryLow = false;
    fuelLow = false;
    batteryTempHigh = false;
    vehicleCommand = '\0';
    vehicleResponse = '\0';
    // batteryTemp = some initial value

    Serial.begin(250000);
    Serial1.begin(9600);
    tft.begin(tft.readID());
    tft.setRotation(1);
    tft.fillScreen(BLACK);

    scheduleInit();
    consoleDisplayInit();
    solarPanelControlInit();
    powerSubsystemInit();

    // initialize the task queue
#ifndef RUN_TESTS
    taskQueueInsert(&powerSubsystemTCB);
    taskQueueInsert(&thrusterSubsystemTCB);
    taskQueueInsert(&vehicleCommsTCB);
    taskQueueInsert(&satelliteComsTCB);
    taskQueueInsert(&warningAlarmTCB);
    taskQueueInsert(&consoleDisplayTCB);
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
