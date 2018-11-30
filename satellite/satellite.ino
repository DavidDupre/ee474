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
#include "imageCapture.h"
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
volatile float distanceBufferPtr[TRANSPORT_DISTANCE_BUFFER_LENGTH];
volatile unsigned int batteryTempPtr[BATTERY_TEMP_BUFFER_LENGTH];
bool driveMotorSpeedInc;
bool driveMotorSpeedDec;
char vehicleCommand;
char vehicleResponse;
uint8_t numCmdErrors;
bool batteryTempHigh;

bool temperatureAlarmAcked;

unsigned short imageDataRaw[IMAGE_CAPTURE_RAW_BUFFER_LENGTH];
unsigned int imageData[IMAGE_CAPTURE_FREQ_BUFFER_LENGTH];

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
    imageCaptureInit();
    powerSubsystemInit();
    satelliteComsInit();
    solarPanelControlInit();
    thrusterSubsystemInit();
    transportDistanceInit();
    vehicleCommsInit();
    warningAlarmInit();

    powerSubsystemTCB.priority = 1;
    taskQueueInsert(&powerSubsystemTCB);

    thrusterSubsystemTCB.priority = 1;
    taskQueueInsert(&thrusterSubsystemTCB);

    imageCaptureTCB.priority = 1;
    taskQueueInsert(&imageCaptureTCB);

    comsRxTCB.priority = 2;
    taskQueueInsert(&comsRxTCB);

    vehicleCommsTCB.priority = 4;
    taskQueueInsert(&vehicleCommsTCB);

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

    sei(); // enable interrupts
}

void loop() {
    schedule();
    return;
}
