#include "powerSubsystem.h"
#include "sharedVariables.h"
#include <AUnit.h>  // Test framework
#include <stdint.h>

/* The following testing was done on the MEGA after running the simulation for a "while" ~powerGeneration=100:

    Start the satellite and ensure solar panel deploys after decreases below BATTERY_LEVEL_LOW
    Ensure the proper power generation incrementation would happen
    Ensure the power generation rate of change would flip when appropriate
    Ensure the battery level never decrements below 0
    Ensure the battery level never increases above 100
*/

// Testing to make sure solar panel deploys upon battery falling below BATTERY_LEVEL_LOW
test(solarPanelOperation) {
    bool solarPanelState = false;
    volatile unsigned int batteryLevelPtr[BATTERY_LEVEL_BUFFER_LENGTH]
        = { BATTERY_LEVEL_LOW - 1 };
    unsigned short powerConsumption = 6;
    unsigned short powerGeneration = 0;
    bool solarPanelDeploy = false;
    bool solarPanelRetract = false;

    PowerSubsystemData data = {
        &solarPanelState,
        &solarPanelDeploy,
        &solarPanelRetract,
        batteryLevelPtr,
        &powerConsumption,
        &powerGeneration
    };

    powerSubsystem((void *) &data);
    assertTrue(solarPanelState == true);
}