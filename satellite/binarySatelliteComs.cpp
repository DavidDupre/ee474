#include "binarySatelliteComs.h"
#include "schedule.h"
#include "sharedVariables.h"
#include <Arduino.h>

#define MAX_COMMAND_HANDLERS 10
#define MAX_TLM_SENDERS      10
#define TLM_SYNC_PATTERN     0xFC
#define SERIAL_TIMEOUT_MS    100


typedef struct {
    TaskId taskId;
    cmd_handler_fn handle;
} CommandHandler;

typedef struct {
    TlmId tlmId;
    uint8_t length;
    uint8_t *data;
    bool ready;
} TelemetrySender;

TLM_PACKET {
    uint8_t numErrors;
    uint8_t numCmdHandlers;
    uint8_t numTlmSenders;
} MetaPacket;


void sendTelemetryPacket(TlmId tlmId, uint8_t *data, uint8_t size);

// reads an incoming command from Serial and dispatches it to a command handler
void processCommand(BCData *data);

// returns true if it found the sync byte
bool sync();


TCB bcTCB;
BCData bcData = {
    &numTlmErrors
};

CommandHandler commandHandlers[MAX_COMMAND_HANDLERS];
uint8_t numCommandHandlers = 0;

TelemetrySender tlmSenders[MAX_TLM_SENDERS];
uint8_t numTlmSenders = 0;

MetaPacket metaPacket;


void bcInit() {
    tcbInit(
        &bcTCB,
        &bcData,
        binarySatelliteComs,
        TASKID_BINCOMS,
        1
    );

    Serial.setTimeout(SERIAL_TIMEOUT_MS);
    numTlmErrors = 0;
    memset(commandHandlers, 0, sizeof(commandHandlers));
    memset(tlmSenders, 0, sizeof(tlmSenders));

    bcRegisterTlmSender(TLMID_META, sizeof(metaPacket), &metaPacket);
}

void binarySatelliteComs(void *bcData) {
    BCData *data = (BCData *) bcData;

    // send telemetry packets
    for (uint8_t i = 0; i < numTlmSenders; i++) {
        if (tlmSenders[i].ready) {
            sendTelemetryPacket(tlmSenders[i].tlmId, tlmSenders[i].data,
                    tlmSenders[i].length);
            tlmSenders[i].ready = false;
        }
    }

    // process commands
    while (sync()) {
        processCommand(data);
    }

    // update meta packet
    metaPacket.numErrors = *data->numErrors;
    metaPacket.numTlmSenders = numTlmSenders;
    metaPacket.numCmdHandlers = numCommandHandlers;
    bcSend(TLMID_META);
}

void bcRegisterCmdHandler(TaskId taskId, cmd_handler_fn handler) {
    if (numCommandHandlers >= MAX_COMMAND_HANDLERS) {
        Serial.println(F("ERROR! Too many command handlers!"));
        return;
    }
    commandHandlers[numCommandHandlers++] = {
        taskId,
        handler
    };
}

void bcRegisterTlmSender(TlmId tlmId, uint8_t length, void *data) {
    if (numTlmSenders >= MAX_TLM_SENDERS) {
        Serial.println(F("ERROR! Too many telementry senders!"));
        return;
    }
    tlmSenders[numTlmSenders++] = {
        tlmId,
        length,
        (uint8_t *) data,
        false
    };
}

void bcSend(TlmId tlmId) {
    for (uint8_t i = 0; i < numTlmSenders; i++) {
        if (tlmSenders[i].tlmId == tlmId) {
            tlmSenders[i].ready = true;
            break;
        }
    }
}

void sendTelemetryPacket(TlmId tlmId, uint8_t *data, uint8_t size) {
    // write the header (sync byte, full length, and ID)
    Serial.write(TLM_SYNC_PATTERN);
    Serial.write(size + 3); // add 3 for these header bytes
    Serial.write(tlmId);

    // write the data
    Serial.write(data, size);
}

bool sync() {
    while (Serial.available()) {
        if (Serial.read() == TLM_SYNC_PATTERN) {
            return true;
        }
    }
    return false;
}

void processCommand(BCData *bcData) {
    // read the header
    uint8_t length;
    uint8_t taskId;
    uint8_t opcode;
    Serial.readBytes(&length, 1);
    Serial.readBytes(&taskId, 1);
    Serial.readBytes(&opcode, 1);

    // read the body of the command
    uint8_t cmdData[256];
    Serial.readBytes(cmdData, length);

    // dispatch to different entities
    bool handled = false;
    for (uint8_t i = 0; i < numCommandHandlers; i++) {
        CommandHandler *h = &commandHandlers[i];
        if (taskId == h->taskId) {
            handled = h->handle(opcode, cmdData);
            break;
        }
    }

    // report unhandled commands
    if (!handled) {
        bcData->numErrors++;
    }
}
