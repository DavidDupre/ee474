#include "predefinedMacros.h"
#include "sharedVariables.h"
#include "thrusterSubsystem.h"
#include "powerSubsystem.h"
#include "consoleDisplay.h"
#include "consoleKeypad.h"
#include "solarPanel.h"
#include "satelliteComs.h"
#include "comsReceive.h"
#include "comsTransmit.h"
#include "vehicleComms.h"
#include "warningAlarm.h"
#include "solarPanel.h"
#include "schedule.h"
#include "transportDistance.h"
#include "tft.h"
#include "pirateDetectionSubsystem.h"
#include "pirateManagementSubsystem.h"

#include <AUnit.h>  // Test framework

#define CMDID_START 21
#define CMDID_STOP 22


uint16_t thrusterCommand;
unsigned short fuelLevel;
SolarPanelState solarPanelState;
bool solarPanelDeploy;
bool solarPanelRetract;
unsigned short powerConsumption;
unsigned short powerGeneration;
bool batteryLow;
bool fuelLow;
volatile unsigned int batteryLevelPtr[BATTERY_LEVEL_BUFFER_LENGTH];
volatile float distanceBufferPtr[TRANSPORT_DISTANCE_BUFFER_LENGTH];
volatile unsigned int batteryTempPtr[BATTERY_TEMP_BUFFER_LENGTH];
bool driveMotorSpeedInc;
bool driveMotorSpeedDec;
char vehicleCommand;
char vehicleResponse;
uint8_t numCmdErrors;
bool batteryTempHigh;
bool detected;
unsigned int pirateProximity;
ConsoleStatus cStatus;

bool temperatureAlarmAcked;

unsigned int imageData[IMAGE_CAPTURE_FREQ_BUFFER_LENGTH];

void setup() {
    initializeData();

    Serial.begin(250000);
    Serial1.begin(9600);
    tft.begin(tft.readID());
    tft.setRotation(1);
    tft.fillScreen(BLACK);

    comsTxInit(); // must be called first
    comsRxInit(); // must be called before other inits, except comxTxInit
    scheduleInit();
    consoleDisplayInit();
    consoleKeypadInit();
    powerSubsystemInit();
    satelliteComsInit();
    solarPanelControlInit();
    thrusterSubsystemInit();
    transportDistanceInit();
    vehicleCommsInit();
    warningAlarmInit();
    pirateDetectionInit();
    pirateManagementInit();

    setupTaskQueue();

    sei(); // enable interrupts

    comsRxRegisterCallback(CMDID_START, start);
    comsRxRegisterCallback(CMDID_STOP, stop);
}

void loop() {
    schedule();
    return;
}

// Start task execution
// Tasks are running by defualt
void start() {
//    interrupts();
   initializeData();
   setupTaskQueue();
   drawLabels();
}


// Stop most task execution but keep up COSMOS coms
void stop() {
    // noInterrupts();
    clearAll();
    tft.fillScreen(BLACK);
}

void initializeData() {
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
    detected = false;
    pirateProximity = PIRATE_PROXIMITY_INITIAL;
    cStatus = consoleOn;
}

void setupTaskQueue() {
    powerSubsystemTCB.priority = 1;
    taskQueueInsert(&powerSubsystemTCB);

    thrusterSubsystemTCB.priority = 1;
    taskQueueInsert(&thrusterSubsystemTCB);

    comsRxTCB.priority = 2;
    taskQueueInsert(&comsRxTCB);

    vehicleCommsTCB.priority = 4;
    taskQueueInsert(&vehicleCommsTCB);

    pirateDetectionSubsystemTCB.priority = 1;
    taskQueueInsert(&pirateDetectionSubsystemTCB);

#ifdef ENABLE_BINARY_COMS
    comsTxTCB.priority = 5;
    taskQueueInsert(&comsTxTCB);
#else
    satelliteComsTCB.priority = 5;
    taskQueueInsert(&satelliteComsTCB);
#endif /* ENABLE_BINARY_COMS */

    warningAlarmTCB.priority = 4;
    taskQueueInsert(&warningAlarmTCB);

    consoleDisplayTCB.priority = 4;
    taskQueueInsert(&consoleDisplayTCB);
}
