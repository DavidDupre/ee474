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
    
    Serial1.println(comm);

    char response;

    comm = 'F';
    while (!Serial.available() && comm != '\0') {
        response = Serial1.read();
    }

    *data->response = response;
    *data->command = '\0';
    Serial.println(response);

}