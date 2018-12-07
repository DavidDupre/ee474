#include <stdint.h>
#include <Arduino.h>
#include "vehicleComms.h"
#include "schedule.h"
#include "sharedVariables.h"
#include "comsReceive.h"
#include "comsTransmit.h"
#include "transportDistance.h"

#define IMAGE_CAPTURE_FREQ_BUFFER_LENGTH 16

// command IDs for commands over Serial
// these must be unique to the entire satellite
// keep this in sync with COSMOS
#define CMDID_EARTH 3
#define CMDID_VEHICLE_RESPONSE 5
#define CMDID_VEHICLE_IMAGE 6

// Telemetry IDs unique to the entire satellite
// Keep this in sync with COSMOS
#define TLMID_VEHICLE 10
#define TLMID_EARTH 8


TLM_PACKET {
    char response;
} VehicleResponsePacket;

TLM_PACKET {
    char command;
} SatellitePacket;


bool handleCommand(uint8_t *data);
bool handleImage(uint8_t *data);
bool handleResponse(uint8_t *data);


TCB vehicleCommsTCB;

VehicleCommsData vehicleCommsData = {
    &vehicleCommand,
    &vehicleResponse
};

static VehicleResponsePacket responseTlmPacket;
static SatellitePacket commandTlmPacket;


void vehicleCommsInit() {
    tcbInit(
        &vehicleCommsTCB,
        &vehicleCommsData,
        vehicleComms,
        TASKID_VEHCOMS,
        1
    );
    comsTxRegisterSender(BUS_GROUND, TLMID_EARTH, sizeof(responseTlmPacket),
            &responseTlmPacket);
    comsTxRegisterSender(BUS_VEHICLE, TLMID_VEHICLE, sizeof(commandTlmPacket),
            &commandTlmPacket);
    comsRxRegisterCallback(CMDID_EARTH, handleCommand);
    comsRxRegisterCallback(CMDID_VEHICLE_RESPONSE, handleResponse);
    comsRxRegisterCallback(CMDID_VEHICLE_IMAGE, handleImage);
}

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

    // Set response if serial is available
    if (*data->vehicleResponse == 'D') {
        taskQueueInsert(&transportDistanceTCB);
    }

    // relay response to binary comms
    // tlmPacket.response = *data->vehicleResponse;
    // comsTxSend(TLMID_EARTH);

    if (*data->vehicleCommand == 'C') {
        taskQueueDelete(&transportDistanceTCB);
    }
}

bool handleCommand(uint8_t *data) {
    vehicleCommand = data[0];
    commandTlmPacket.command = vehicleCommand;
    comsTxSend(TLMID_VEHICLE);
    return true;
}

bool handleResponse(uint8_t *data) {
    vehicleResponse = data[0];
    responseTlmPacket.response = vehicleResponse;
    comsTxSend(TLMID_EARTH);
    return true;
}

bool handleImage(uint8_t *data) {
    for (int i = 0; i < IMAGE_CAPTURE_FREQ_BUFFER_LENGTH; i++) {
        imageData[i] = data[i];
    }
    return true;
}
