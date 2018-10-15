#include "powerSubsystem.h"
#include <stdint.h>

#define HIGH_BATTERY 95
#define BATTERY_LEVEL_MID 50
#define BATTERY_LEVEL_LOW 10
#define POWER_CONSUMPTION_UPPER 10
#define POWER_CONSUMPTION_LOWER 5

/******************************************************************************
 * name: powerSubsystem
 * 
 * inputs:
 * powerSubsystemData (void*): must be of Type powerSubsystemData*.
 * powerSubsystemData holds pointers to the following variables:
 * 
 * solarPanelDeployed: bool representing whether solar panel is deployed.
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
 * setPowerConsumption to increasing
 * set timesCalled to zero
 * if powerConsumption is increasing
 *  go up 2 on even, down 1 on odd
 * else 
 *  go down 2 on even, up 1 on odd
 * 
 * check to see if powerConsumption should be switched
 * if powerConsumption is increasing and > 10
 * switch to decreasing
 * if powerConsumption is decreasing and < 5
 * switch to increasing
 * 
 * if solar panel is deployed and >95%
 * retract solar panel
 * otherwise if less than or equal to 50
 * increase by 2 on even 1 on odd
 * if more than 50
 * increase by 2 on even
 * 
 * if solar panel deployed 
 *  increment battery level by powerGeneration, deduct powerConsumption
 * else 
 * decrement by 3 times powerConsumption
 * 
 * author: Nick Orlov
*****************************************************************************/ 
void powerSubsystem(void* powerSubsystemData) {
    // Casting pointer to proper data type
    PowerSubsystemData* data = (PowerSubsystemData*) powerSubsystemData;
    unsigned short powerConsumption = *data->powerConsumption;
    bool solarPanelDeployed = *data->solarPanelState;
    unsigned short batteryLevel = *data->batteryLevel;
    unsigned short powerGeneration = *data->powerGeneration;
    

    // Initially the Power Consumption is increasing until it reaches 10
    static bool isIncreasing = true;

    // Keeping track of the number of times the function has been called
    static int timesCalled = 0;

    // Modifying the power consumption
    if(isIncreasing) {
        if(timesCalled % 2 == 0) {
            powerConsumption +=2;
        } else {
            powerConsumption -=1;
        }
    } else {
        if(timesCalled % 2 == 0) {
            powerConsumption -=2;
        } else {
            powerConsumption +=1;
        }
    }

    // Check to see if power consumption should reverse its rate of change
    if(isIncreasing && powerConsumption > POWER_CONSUMPTION_UPPER ||
     !isIncreasing && powerConsumption < POWER_CONSUMPTION_LOWER) {
        isIncreasing = !isIncreasing;
    }

    // Checking if the solar panel should be up for power generation
    if(solarPanelDeployed) {
        if(batteryLevel > HIGH_BATTERY) {
            solarPanelDeployed = !solarPanelDeployed;
        } else  {
            if (timesCalled % 2 == 0) {
                batteryLevel += 2;
            } else {
                if(batteryLevel <= BATTERY_LEVEL_MID) {
                    batteryLevel += 1;
                }
            }
        }
        // Updating the battery level for solar panel deployed
        batteryLevel = batteryLevel - powerConsumption + powerGeneration;
    } else {
        if(batteryLevel <= BATTERY_LEVEL_LOW) {
            solarPanelDeployed = !solarPanelDeployed;
        }
        // Updating the battery level for solar panel not deployed
        batteryLevel = batteryLevel - 3 * powerConsumption;
    }

    // Incrementing times function has been called
    timesCalled+= 1;
}

