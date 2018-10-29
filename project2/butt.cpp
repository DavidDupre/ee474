#include <stdint.h>
#include "schedule.h"
#include "tft.h"
#include "butt.h"


void butt(void* buttData) {
    digitalWrite(13, HIGH);
    TSPoint p = ts.getPoint();
    digitalWrite(13, LOW);


    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!
    Serial.print("Pressure: ");
    Serial.println(p.z);
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
        Serial.print("X: ");
        Serial.println(p.x);
        Serial.print("Y: ");
        Serial.println(p.y);
    }
}