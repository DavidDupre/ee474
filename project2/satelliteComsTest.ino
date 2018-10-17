#include "satelliteComs.h"
#include "thrusterSubsystem.h"
#include <stdint.h>

// Testing last bit is a 0: Updating Thruster Command
test(thrusterUpdate) {
    bool fuelLow = false;
    bool batteryLow = false;
    bool solarPanelState = false;
    unsigned short batteryLevel = 90;
    unsigned short fuelLevel = 90;
    unsigned short powerConsumption = 10;
    unsigned short powerGeneration = 8;
    unsigned int thrusterCommand = 0b0000001001000100;

    SatelliteComsData data = {
        &fuelLow,
        &batteryLow,
        &solarPanelState,
        &batteryLevel,
        &fuelLevel,
        &powerConsumption,
        &powerGeneration,
        &thrusterCommand
    };

    // Setting seed for psuedo random to 1, through testing, the next number produced will be 846
    // Because this number is even, thruster command should be set to it
    srand(2);
    satelliteComs((void *) &data);
    assertEqual(thrusterCommand, (unsigned int) 846);
}

// Testing last bit is a 1: Setting Thruster Command to Invalid
test(thrusterInvalid) {
    bool fuelLow = false;
    bool batteryLow = false;
    bool solarPanelState = false;
    unsigned short batteryLevel = 90;
    unsigned short fuelLevel = 90;
    unsigned short powerConsumption = 10;
    unsigned short powerGeneration = 8;
    unsigned int thrusterCommand = 0b0000001001000101;

    SatelliteComsData data = {
        &fuelLow,
        &batteryLow,
        &solarPanelState,
        &batteryLevel,
        &fuelLevel,
        &powerConsumption,
        &powerGeneration,
        &thrusterCommand
    };

    // will force a 16807 to return from the next rand()
    // Because the number is odd, thruster command should be set to none
    srand(1);
    satelliteComs((void *) &data);
    assertEqual(thrusterCommand, THRUSTER_CMD_NONE);
}