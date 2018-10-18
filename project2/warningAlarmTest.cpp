#include "warningAlarm.h"
#include "schedule.h"
#include <stdint.h>
#include <AUnit.h>  // Test framework

/*
 * Test consoleDisplay
 */
test(warningAlarm) {
    unsigned short fuelLevel = 100;
    unsigned short batteryLevel = 100;
    bool batteryLow = false;
    bool fuelLow = false;

    // build the input data struct
    WarningAlarmData data = {
        &batteryLow,
        &fuelLow,
        &batteryLevel,
        &fuelLevel
    };

    // send the new command
    setGlobalTimeBase(0);
    warningAlarm((void *) &data);
    assertFalse((bool) fuelLow);    // Fuel maxed
    assertFalse((bool) batteryLow); // Battery maxed

    // Next cycle
    setGlobalTimeBase(5000);

    fuelLevel = 10;
    batteryLevel = 10;

    // call the function again to update the warnings
    warningAlarm((void *) &data);

    // Check that it updated the low statuses
    assertTrue((bool) fuelLow);
    assertTrue((bool) batteryLow);

    // Next cycle
    setGlobalTimeBase(10000);

    fuelLevel = 40;
    batteryLevel = 40;

    // call the function again to update the warnings
    warningAlarm((void *) &data);

    assertFalse((bool) fuelLow);    // Fuel medium
    assertFalse((bool) batteryLow); // Battery medium

    // Next cycle
    setGlobalTimeBase(15000);

    fuelLevel = 1;
    batteryLevel = 1;

    // call the function again to update the warnings
    warningAlarm((void *) &data);

    assertTrue((bool) fuelLow);
    assertTrue((bool) batteryLow);

    // Next cycle
    setGlobalTimeBase(20000);

    fuelLevel = 80;
    batteryLevel = 80;

    // call the function again to update the warnings
    warningAlarm((void *) &data);

    assertFalse((bool) fuelLow);    // Fuel high
    assertFalse((bool) batteryLow); // Battery high
}
