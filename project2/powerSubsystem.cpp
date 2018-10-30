#include "powerSubsystem.h"
#include <stdint.h>
#include "schedule.h"


unsigned short capAt100(unsigned short batteryLevel);


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
 * init
 *  create 16 element circular linked list full of null data
 *  attach interrupt to battery pin
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
 * interrupt:
 * delay 600 delayMicroseconds
 * update buffer to reflect most recent measurements:
 *  head = head.next
 *  head = new data (head will alawys be most recent data)
 * 
 * 
 * 
 * deprecated:
 * if solar panel deployed
 *  increment battery level by powerGeneration, deduct powerConsumption
 * else
 *  decrement by 3 times powerConsumption
 *
 * author: Nick Orlov
*****************************************************************************/

struct Measurement measurements[16];

void powerSubsystemInit() {
    for(int i = 0; i < 15; i++) {
        //measurements[i].data = NULL;
        measurements[i].next = &measurements[i+1];
    }
    //measurements[15].data = null;
    measurements[15].next = &measurements[0];
    // There are elements so far so it doesn't matter
    batteryHead = &measurements[0];
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
        if(*data->batteryLevel > HIGH_BATTERY) {
            *data->solarPanelState = !*data->solarPanelState;
        } else if(timesCalled % 2 == 0) {
            // Incrementing logic for the battery level from the solar panel
            *data->powerGeneration += 2;
        } else if (*data->batteryLevel <= BATTERY_LEVEL_MID) {
            *data->powerGeneration += 1;
        }
    } else {
        // Checking to see if the battery level is low, and if solar panel needs to be deployed
        if(*data->batteryLevel <= BATTERY_LEVEL_LOW) {
            *data->solarPanelState = !*data->solarPanelState;
        }
    }

    // analogRead(A13);
    // delayMicroseconds(600);



    



    // The following portion is deprecated from Assignment 2.
    // if(*data->solarPanelState) {
    //     // Updating the battery level for the case of solar panel deployed
    //     *data->batteryLevel = capAt100(*data->batteryLevel - *data->powerConsumption + *data->powerGeneration);
    // } else {
    //     // Updating the battery level for solar panel not deployed
    //     *data->batteryLevel = *data->batteryLevel -
    //     SOLAR_PANEL_NOT_DEPLOYED_AMPLIFIER * *data->powerConsumption;
    // }
    // // Incrementing times function has been called
    // timesCalled+= 1;
}

unsigned short capAt100(unsigned short batteryLevel) {
    if(batteryLevel > 100) {
        return 100;
    } else {
        return batteryLevel;
    }
}

