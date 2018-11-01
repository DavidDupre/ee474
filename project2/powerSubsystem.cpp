#include "powerSubsystem.h"
#include <stdint.h>
#include "schedule.h"
#include "sharedVariables.h"
#include <Arduino.h>


unsigned int normBattery(unsigned int input);


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
 *  attach interrupt to EXTERNAL_MEASUREMENT_EVENT_PIN
 *
 * interrupt:
 *  delay 600 microseconds
 *  make measurement and update buffer:
 *  for every measurement in indices 0-14 (first 15 measurements)
 *  move them to the next index (indices 1-15)
 *  add new measurement to index 0 via analogRead of EXTERNAL_MEASUREMENT_EVENT_PIN
 *
 * powerSubsystem:
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

// Measure battery on timer interrupt
ISR(TIMER5_COMPA_vect) {
    measurementExternalInterruptISR();
}

// batteryLevelPtr points to a pointer which points to an array of the 16
// most recent battery level measurements
void measurementExternalInterruptISR() {
    // Waiting 600 microseconds for the battery level to stab
    delayMicroseconds(600);

    // Moving up the first 15 measurements, overwriting the 16th measurement
    for(int i = BATTERY_LEVEL_BUFFER_LENGTH - 1; i >= 0; i--) {
        batteryLevelPtr[i] = batteryLevelPtr[i+1];
    }

    // Taking the most recent measurement from the external event interrupt pin
    unsigned int analogBatteryLvl = analogRead(EXTERNAL_MEASUREMENT_EVENT_PIN);
    batteryLevelPtr[0] = normBattery(analogBatteryLvl);
}

void powerSubsystem(void* powerSubsystemData) {
    // return early if less than 5 seconds have passed
    static unsigned long lastRunTime;
    if (globalTimeBase() - lastRunTime < MAJOR_CYCLE_DURATION_MS) {
        return;
    }
    lastRunTime = globalTimeBase();

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
    if(*data->solarPanelState) {
        // If the battery level is above the threshold, retract the solar panel
        if(data->batteryLevelPtr[0] > HIGH_BATTERY) {
            *data->solarPanelState = !*data->solarPanelState;
        } else if(timesCalled % 2 == 0) {
            // Incrementing logic for the battery level from the solar panel
            *data->powerGeneration += 2;
        } else if (data->batteryLevelPtr[0] <= BATTERY_LEVEL_MID) {
            *data->powerGeneration += 1;
        }
    } else {
        // Checking to see if the battery level is low, and if solar panel needs to be deployed
        if(data->batteryLevelPtr[0] <= BATTERY_LEVEL_LOW) {
            *data->solarPanelState = !*data->solarPanelState;
        }
    }

    // The following portion is deprecated from Assignment 2.

    // if(*data->solarPanelState) {
    //     // Updating the battery level for the case of solar panel deployed
    //     (*data->batteryLevelPtr)[0] = capAt100((*data->batteryLevelPtr)[0] - *data->powerConsumption + *data->powerGeneration);
    // } else {
    //     // Updating the battery level for solar panel not deployed
    //     (*data->batteryLevelPtr)[0] = (*data->batteryLevelPtr)[0] -
    //     SOLAR_PANEL_NOT_DEPLOYED_AMPLIFIER * *data->powerConsumption;
    // }
    // // Incrementing times function has been called
    // timesCalled+= 1;
}

unsigned int normBattery(unsigned int input) {
    unsigned int output = (((BATTERY_MAX - BATTERY_MIN)*
        (input - ANALOG_MIN))/(ANALOG_MAX - ANALOG_MIN) + BATTERY_MIN);
    return output;
}