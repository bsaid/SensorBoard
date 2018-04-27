#include <Arduino.h>

HardwareSerial Serial1(2);

void setup() {
    Serial.begin(115200);
    Serial1.begin(115200);
    Serial.write("Started...\n");
}

void loop() {
    if(Serial1.available()) {
        Serial.write(Serial1.read());
    }
    if(Serial.available()) {
        Serial1.write(Serial.read());
    }
    //Serial.write('a');
    //delay(100);
}
