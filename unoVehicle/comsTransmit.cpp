#include "comsTransmit.h"
#include <Arduino.h>

#define TLM_SYNC_PATTERN 0xFC


void comsTxSend(uint8_t tlmId, void *data, uint8_t size) {
    // write the header (sync byte, full length, and ID)
    Serial.write(TLM_SYNC_PATTERN);
    Serial.write(size + 3); // add 3 for these header bytes
    Serial.write(tlmId);

    // write the data
    Serial.write((uint8_t *) data, size);
}
