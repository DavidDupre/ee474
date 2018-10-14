#include "powerSubsystem.h"
#include <stdint.h>


    /******************************************************************************
     * name: powerSubsystem
     * 
     * inputs:
     *  powerSubsystemData (void*): must be of Type powerSubsystemData*.
     *  powerSubsystemData holds pointers to the following variables:
     * 
     * solarPanelState
     * batteryLevel: unsigned short representing the percentage level of the battery level
     *               initally set to 100.
     * powerConsumption: unsigned short representing the power consumption per 1/3 of a major cycle. 
     *                   initally set to 0.
     * powerGeneration: unsigned short representing the power generation per 1/3 of a major cycle.
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
     * 
     * 
     * author: Nick Orlov
    *****************************************************************************/ 
void powerSubsystem(void* powerSubsystemData) {
    // Casting pointer to proper data type
    PowerSubsystemData* data = (PowerSubsystemData*) powerSubsystemData;
    unsigned short powerConsumption = *data->powerConsumption;
    bool solarPanelState = *data->solarPanelState;
    unsigned short batteryLevel = *data->batteryLevel;
    

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

    // Checking if the solar panel should be up for power generation
    if(solarPanelState) {
        if(batteryLevel > 95) {
            solarPanelState = !solarPanelState;
        } else  {
            if (timesCalled % 2 == 0) {
                batteryLevel += 2;
            } else {
                if(batteryLevel <= 50) {
                    batteryLevel += 1;
                }
            }
        }
    } else {
        if(batteryLevel <= 10) {
            solarPanelState = !solarPanelState;
        }
    }

    if(solarPanelState) {

    }



    timesCalled+= 1;
}

