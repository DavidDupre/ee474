#include "imageCapture.h"
#include "schedule.h"
#include "comsTransmit.h"
#include "comsReceive.h"
#include "sharedVariables.h"
#include "satelliteComs.h"

#define CMDID_SATELLITE_COMMAND 10

TLM_PACKET {
    char response;
} VehicleResponse;

bool handleCommand(uint8_t *data);

static VehicleResponse tlmPacket;

void satelliteComsInit() {
    comsTxRegisterSender(BUS_SATELLITE, TLMID_RESPONSE, sizeof(tlmPacket),
            &tlmPacket);
    comsRxRegisterCallback(CMDID_SATELLITE_COMMAND, handleCommand);
}

bool handleCommand(uint8_t *data) {
    command = data[0];
    tlmPacket.response = command;
    comsTxSend(TLMID_RESPONSE);
    return true;
}
