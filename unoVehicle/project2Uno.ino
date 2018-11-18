void setup() {
    Serial.begin(9600);
}

void loop() {
    char comm;
    if (Serial.available()) {
        comm = Serial.read();
        // Do not respond if command is null
        if(comm != '\0') {
            Serial.print(comm);
        }
    }
}