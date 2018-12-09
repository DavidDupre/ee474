#include "schedule.h"
#include "comsReceive.h"
#include "comsTransmit.h"
#include <Arduino.h>
#include "sharedVariables.h"

#define MAX_COMMAND_HANDLERS 16
#define CMD_SYNC_PATTERN     0xFC
#define SERIAL_TIMEOUT_MS    100

// Telemetry IDs unique to the entire satellite
#define TLMID_CMD_ACK 5

// special code to represent an unknown command ID
#define CMDID_ERR 255


typedef struct {
    uint8_t cmdId;
    cmd_callback_fn handle;
} CommandHandler;

TLM_PACKET {
    char opcode; // the letter sent
} AckTlmPacket;


// reads an incoming command from Serial and dispatches it to a command handler
void processCommand(serial_bus *bus, CmdData *data);

// returns true if it found the sync byte
bool sync(serial_bus *bus);


CommandHandler commandHandlers[MAX_COMMAND_HANDLERS];
uint8_t numCommandHandlers = 0;

TCB comsRxTCB;
CmdData cmdData = { };

AckTlmPacket ackTlmPacket;

serial_bus *serialBuses[] = {
    BUS_SATELLITE,
};


void comsRxInit() {
    tcbInit(
        &comsRxTCB,
        &cmdData,
        comsRxUpdate,
        TASKID_COMSRX,
        1
    );

    memset(commandHandlers, 0, sizeof(commandHandlers));
    comsTxRegisterSender(BUS_SATELLITE, TLMID_CMD_ACK, sizeof(ackTlmPacket),
        &ackTlmPacket);
}

void comsRxUpdate(void *cmdData) {
    CmdData *data = (CmdData *) cmdData;

    // process commands from each serial bus
    for (uint8_t i = 0; i < sizeof(serialBuses) / sizeof(serial_bus *); i++) {
        serial_bus *bus = serialBuses[i];
        while (sync(bus)) {
            processCommand(bus, data);
        }
    }
}

void comsRxRegisterCallback(uint8_t cmdId, cmd_callback_fn callback) {
    if (numCommandHandlers >= MAX_COMMAND_HANDLERS) {
        Serial.println(F("ERROR! Too many command handlers!"));
        return;
    }
    commandHandlers[numCommandHandlers++] = {
        .cmdId = cmdId,
        .handle = callback
    };
}

bool sync(serial_bus *bus) {
    while (bus->available()) {
        if (bus->read() == CMD_SYNC_PATTERN) {
            return true;
        }
    }
    return false;
}

void processCommand(serial_bus *bus, CmdData *cmdData) {
    // read the header
    uint8_t length;
    uint8_t cmdId;
    bus->readBytes(&length, 1);
    bus->readBytes(&cmdId, 1);

    // read the letter (opcode)
    char opcode;
    bus->readBytes(&opcode, 1);

    // read the body of the command
    uint8_t data[256];
    bus->readBytes(data, length - 4);

    // dispatch to different entities
    bool handled = false;
    for (uint8_t i = 0; i < numCommandHandlers; i++) {
        CommandHandler *h = &commandHandlers[i];
        if (cmdId == h->cmdId) {
            handled = h->handle(data);
            break;
        }
    }

    // update the ack packet and send it
    ackTlmPacket.opcode = opcode;
    comsTxSend(TLMID_CMD_ACK);
}