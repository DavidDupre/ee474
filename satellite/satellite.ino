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
#include "transportDistance.h"
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
    &solarPanelDeploy,
    &solarPanelRetract,
    batteryLevelPtr,
    &powerConsumption,
    &powerGeneration
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
    &vehicleCommand,
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
    &fuelLevel
};

// static storage for TCBs inserted in setup
TCB tcbs[8];

void setup() {
    thrusterCommand = 0;
    fuelLevel = 100;
    solarPanelState = SOLAR_PANEL_RETRACTED;
    memset((unsigned int *) batteryLevelPtr, 0, BATTERY_LEVEL_BUFFER_LENGTH);
    solarPanelDeploy = false;
    solarPanelRetract = false;
    powerConsumption = 0;
    powerGeneration = 0;
    batteryLow = false;
    fuelLow = false;
    vehicleCommand = '\0';
    vehicleResponse = '\0';

    Serial.begin(250000);
    Serial1.begin(9600);
    tft.begin(tft.readID());
    tft.setRotation(1);
    tft.fillScreen(BLACK);

    scheduleInit();
    consoleDisplayInit();
    solarPanelControlInit();
    powerSubsystemInit();
    vehicleCommsInit();
    transportDistanceInit();

    // insert power
    tcbInit(
        &tcbs[0],
        &powerSubsystemData,
        powerSubsystem,
        "Power Subsystem",  // TODO this will probably fall out of scope...
        1
    );
    taskQueueInsert(&tcbs[0]);

    // insert thruster
    tcbInit(
        &tcbs[1],
        &thrusterSubsystemData,
        thrusterSubsystem,
        "Thruster Subsystem",
        1
    );
    taskQueueInsert(&tcbs[1]);

    // insert vehicle comms
    tcbInit(
        &tcbs[2],
        &vehicleCommsData,
        vehicleComms,
        "Vehicle Communications",
        4
    );
    taskQueueInsert(&tcbs[2]);

    // insert satellite comms
    tcbInit(
        &tcbs[3],
        &satelliteComsData,
        satelliteComs,
        "Satellite Communications",
        4
    );
    taskQueueInsert(&tcbs[3]);

    // insert warning/alarm
    tcbInit(
        &tcbs[4],
        &warningAlarmData,
        warningAlarm,
        "Warning/Alarm",
        4
    );
    taskQueueInsert(&tcbs[4]);

    // insert console display
    tcbInit(
        &tcbs[5],
        &consoleDisplayData,
        consoleDisplay,
        "Console Display",
        4
    );
    taskQueueInsert(&tcbs[5]);
}

void loop() {
    schedule();
    return;
}
