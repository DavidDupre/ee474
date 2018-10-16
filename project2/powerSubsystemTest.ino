#include "powerSubsystem.h"
#include <stdint.h>

/* The following testing was done on the MEGA after running the simulation for a "while" ~powerGeneration=100:

    Start the satellite and ensure solar panel deploys after decreases below BATTERY_LEVEL_LOW
    Ensure the battery level never decrements below 0
    Ensure the battery level never increases above 100
*/

// Testing to make sure solar panel deploys upon battery falling below BATTERY_LEVEL_LOW
test(solarPanelOperation) {
    bool solarPanelState = false;
    unsigned short batteryLevel = BATTERY_LEVEL_LOW - 1;
    unsigned short powerConsumption = 6;
    unsigned short powerGeneration = 0;

    PowerSubsystemData data = {
        &solarPanelState,
        &batteryLevel,
        &powerConsumption,
        &powerGeneration
    };

    powerSubsystem((void *) &data);
    assertTrue(solarPanelState == true);
}