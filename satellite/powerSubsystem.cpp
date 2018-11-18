#include "powerSubsystem.h"
#include <stdint.h>
#include "schedule.h"
#include "sharedVariables.h"
#include "binarySatelliteComs.h"
#include <Arduino.h>
#include <limits.h>
#include "solarPanel.h"
#include "udools.h"

TLM_PACKET {
    uint32_t batteryLevel;
    uint16_t consumption;
    uint16_t generation;
    uint8_t panelState;
} PowerTlmPacket;


TCB powerSubsystemTCB;

PowerSubsystemData powerSubsystemData = {
    &solarPanelState,
    &solarPanelDeploy,
    &solarPanelRetract,
    batteryLevelPtr,
    &powerConsumption,
    &powerGeneration
};

static PowerTlmPacket tlmPacket;

unsigned int normBattery(unsigned int input);
// Flags
// volatile bool readyToMeasure;
volatile unsigned long batteryInitializationTime;

/******************************************************************************
 * name: powerSubsystem
 *
 * inputs:
 * powerSubsystemData (void*): must be of Type powerSubsystemData*.
 * powerSubsystemData holds pointers to the following variables:
 *
 * solarPanelState: bool representing whether solar panel is deployed.
 * batteryLevel: unsigned short representing the percentage level of the battery level
 *               initally set to 100.
 * powerConsumption: unsigned short representing the power consumption initally set to 0.
 * powerGeneration: unsigned short representing the power generation.
 *
 * outputs: void
 *
 * description:
 *  powerSubsystem manages the satellite's power subsystem. The primary goal in managing the power
 *  subsystem is to ensure that the satellite has sufficient energy to continue operation. The power
 *  subsystem monitors the power consumption and power generation in order to properly deploy the solar
 *  panel to the most appropriate position.
 *
 *
 *
 * pseudocode:
 *
 * init
 *  set initalization time to 0
 *  attach interrupt to EXTERNAL_MEASUREMENT_EVENT_PIN
 *
 * interrupt:
 *  set initalization to current time from globalTimeBase
 *
 *
 *
 *  measure and update buffer:
 *  for every measurement in indices 0-14 (first 15 measurements)
 *  move them to the next index (indices 1-15)
 *  add new measurement to index 0 via analogRead of EXTERNAL_MEASUREMENT_EVENT_PIN
 *
 *
 * powerSubsystem:
 *
 * If we are passed the time that we have stored for the global time globalTimeBase
 * AKA. we have delayed 600 microseconds, then measure and update buffer
 *
 * setPowerConsumption to increasing
 * set timesCalled to zero
 * if powerConsumption is increasing
 *  go up 2 on even, down 1 on odd
 * else
 *  go down 2 on even, up 1 on odd
 *
 * check to see if powerConsumption should be switched
 * if powerConsumption is increasing and > 10
 *  switch to decreasing
 *
 * if powerConsumption is decreasing and < 5
 *  switch to increasing
 *
 * if solar panel is deployed and >95%
 *  retract solar panel
 * else if less than or equal to 50
 *  increase by 2 on even 1 on odd
 * if more than 50
 *  increase by 2 on even
 *
 * author: Nick Orlov
*****************************************************************************/

void powerSubsystemInit() {
    tcbInit(
        &powerSubsystemTCB,
        &powerSubsystemData,
        powerSubsystem,
        TASKID_POWER,
        1
    );

    // setting the battery initialization to unsigned long max value
    // in order to make sure task never measures as it checks to see
    // if the mission elapsed time is greater than it
    batteryInitializationTime = 0;

    // Attaching interrupt
    attachInterrupt(digitalPinToInterrupt(MEAUSURE_INTERRUPT_PIN),
    measurementExternalInterruptISR, RISING);

    // register telemerty
    bcRegisterTlmSender(TLMID_POWER, sizeof(tlmPacket), &tlmPacket);
}

void measurementExternalInterruptISR() {
    // set bool for measurement - dont need if set initial batteryInitializationTime
    // to UNSIGNED_LONG_MAX
    // bool readyToMeasure = true;
    // set integer time
    batteryInitializationTime = globalTimeBase();
}

// batteryLevelPtr points to a pointer which points to an array of the 16
// most recent battery level measurements
void measureBattery() {
    // Taking the most recent measurement from the external event interrupt pin
    unsigned int analogBatteryLvl = analogRead(EXTERNAL_MEASUREMENT_EVENT_PIN);
    unsigned int newBatterLvl = norm<unsigned int>(analogBatteryLvl, ANALOG_MIN, ANALOG_MAX, BATTERY_MIN, BATTERY_MAX);
    addToBuffer(newBatterLvl, batteryLevelPtr, BATTERY_LEVEL_BUFFER_LENGTH);
}

void powerSubsystem(void* powerSubsystemData) {
    // return early if less than 5 seconds have passed
    static unsigned long lastRunTime;
    if (globalTimeBase() - lastRunTime < MAJOR_CYCLE_DURATION_MS) {
        return;
    }
    lastRunTime = globalTimeBase();

    // This if condition will be satisfied if it is greater by 1 which would measurement
    // it is 1 millisecond (1000 microseconds) more which is more than 600 microseconds
    // and would not measure if the interrupt hasn't happen because that is the max value
    // of the mission elapsed time (around 50 days)
    if(globalTimeBase() >= batteryInitializationTime + MEASURE_DELAY_MS) {
        measureBattery();
    }

    // Casting pointer to proper data type
    PowerSubsystemData* data = (PowerSubsystemData*) powerSubsystemData;

    // Initially the Power Consumption is increasing until it reaches
    // the upper limit of POWER_CONSUMPTION_UPPER
    static bool isIncreasing = true;

    // Keeping track of the number of times the function has been called
    static int timesCalled = 0;

    // Updates the power consumption accordingly
    if(isIncreasing) {
        // If on an even call: increment by 2, odd call: decrement by 1
        if(timesCalled % 2 == 0) {
            *data->powerConsumption +=2;
        } else {
            *data->powerConsumption -=1;
        }
    } else if (timesCalled % 2 == 0) {
        // If on an even call: decrement by 2, odd call: increment by 1
        *data->powerConsumption -=2;
    } else {
        *data->powerConsumption +=1;
    }

    // Check to see if power consumption should reverse its rate of change
    if(isIncreasing && *data->powerConsumption > POWER_CONSUMPTION_UPPER ||
     !isIncreasing && *data->powerConsumption < POWER_CONSUMPTION_LOWER) {
        isIncreasing = !isIncreasing;
    }

    // Checking if the solar panel should be up for power generation
    if(*data->solarPanelState != SOLAR_PANEL_RETRACTED) {
        // If the battery level is above the threshold, retract the solar panel
        if(data->batteryLevelPtr[0] > BATTERY_LEVEL_HIGH) {
            *data->solarPanelRetract = true;
            taskQueueInsert(&solarPanelControlTCB);
        } else if(timesCalled % 2 == 0) {
            // Incrementing logic for the battery level from the solar panel
            *data->powerGeneration += 2;
        } else if (data->batteryLevelPtr[0] <= BATTERY_LEVEL_MID) {
            *data->powerGeneration += 1;
        }
    } else {
        // Checking to see if the battery level is low, and if solar panel needs to be deployed
        if(data->batteryLevelPtr[0] <= BATTERY_LEVEL_LOW) {
            // Deploy solar panel
            *data->solarPanelDeploy = true;
            taskQueueInsert(&solarPanelControlTCB);
        }
    }

    // send telemetry
    tlmPacket.batteryLevel = data->batteryLevelPtr[0];
    tlmPacket.consumption = *data->powerConsumption;
    tlmPacket.generation = *data->powerGeneration;
    tlmPacket.panelState = *data->solarPanelState;
    bcSend(TLMID_POWER);
}

unsigned int normBattery(unsigned int input) {
    unsigned int output = (((BATTERY_MAX - BATTERY_MIN)*
        (input - ANALOG_MIN))/(ANALOG_MAX - ANALOG_MIN) + BATTERY_MIN);
    return output;
}