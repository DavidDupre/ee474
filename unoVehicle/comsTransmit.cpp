#include "comsTransmit.h"
#include <Arduino.h>

#define TLM_SYNC_PATTERN 0xFC


void sendTelemetryPacket(serial_bus *bus, uint8_t tlmId, uint8_t *data,
        uint8_t size) {
    // write the header (sync byte, full length, and ID)
    bus->write(TLM_SYNC_PATTERN);
    bus->write(size + 3); // add 3 for these header bytes
    bus->write(tlmId);

    // write the data
    if (size > 0) {
        bus->write(data, size);
    }
}