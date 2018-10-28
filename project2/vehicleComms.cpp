#include <stdint.h>
#include <Arduino.h>
#include "vehicleComms.h"
#include "schedule.h"

void vehicleComms(void *vehicleCommsData) {

    // Don't run if it has been less than a major cycle since last execution
    static unsigned long lastRunTime;
    if (globalTimeBase() - lastRunTime < MAJOR_CYCLE_DURATION_MS) {
        return;
    }
    lastRunTime = globalTimeBase();

    // Cast from void to correct type
    VehicleCommsData *data = (VehicleCommsData *) vehicleCommsData;

    char comm = *data->command;
    
    // comm = 'F';
    // Print command to Serial1 for the Uno to pick up
    Serial1.println(comm);

    // If the command is \0 there is no command and do nothing
    if (Serial1.available()) {
        *data->response = Serial1.read();
        *data->command = '\0';
    }
        // Serial.println(response);
    /* TODO: Not sure if this is the proper way to show a response
    * Spec had a longer looking response but it also specifies the
    * type as a char
    */
}