#include <stdint.h>
#include <Arduino.h>
#include "vehicleComms.h"
#include "schedule.h"

/******************************************************************************
 * name : vehicleComms
 *
 * inputs: vehicleCommsData (void*)
 * vehicleCommsData holds the pointers to the following data:
 * command: char representing the command to be sent to the mining vehicle
 * response: char representing the response from the cart corresponding to the command
 *
 * outputs: void
 *
 * description:
 *
 * vehicleComms sends commands to the minning vehicle (uno) over serial and 
 * and stores its response
 *
 *
 * psuedocode:
 *
 * make sure it is time to run
 * cast data
 * print the command over serial 1 if there is one
 * if serial is available, read the response into its variable
 *
 *  author: Philip White
*****************************************************************************/
void vehicleComms(void *vehicleCommsData) {

    // Don't run if it has been less than a major cycle since last execution
    static unsigned long lastRunTime;
    if (globalTimeBase() - lastRunTime < MAJOR_CYCLE_DURATION_MS) {
        return;
    }
    lastRunTime = globalTimeBase();

    // Cast from void to correct type
    VehicleCommsData *data = (VehicleCommsData *) vehicleCommsData;

    char comm = *data->vehicleCommand;
    
    // Print command to Serial1 for the Uno to pick up
    // Do not print if there is no command
    if (comm != '\0') {
        Serial1.println(comm);
        *data->vehicleCommand = '\0';
    }

    // If the command is \0 there is no command and do nothing
    if (Serial1.available()) {
        *data->vehicleResponse = Serial1.read();
    }

    /* TODO: Not sure if this is the proper way to show a response
    * Spec had a longer looking response but it also specifies the
    * type as a char
    */
}