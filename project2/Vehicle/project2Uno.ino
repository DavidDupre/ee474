
String commandFormat(String comm) {
    return "A<" + comm + ">";
}

void setup() {
    Serial.begin(9600);
}

void loop() {
    char comm;
    if (Serial.available()) {
        comm = Serial.read();
        if(comm != '\0') {
        Serial.println(comm);
        }
    }    
    // TODO: Figure out what the timeline of the UNO needs to be
}

// if(comm == 'F') {
//         Serial.println(commandFormat("Forward"));
//     } else if(comm == 'B') {
//         Serial.println(commandFormat("Back"));
//     } else if(comm == 'L') {
//         Serial.println(commandFormat("Left"));
//     } else if(comm == 'R') {
//         Serial.println(commandFormat("Right"));
//     } else if(comm == 'D') {
//         Serial.println(commandFormat("Drill down - Start"));
//     } else if(comm == 'H') {
//         Serial.println(commandFormat("Drill up - Stop"));
//     }  else {
//         Serial.println(commandFormat("ERROR"));
//     }
// }