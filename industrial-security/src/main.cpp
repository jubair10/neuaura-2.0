#include "main.h"

void setup(){
    Serial.begin(115200);
    initializeWiFi();
    initializeSensors();
}

void loop(){

    bool pir1 = readPIR(PIR_1);
    bool pir2 = readPIR(PIR_2);
    bool ldr = readLDR(LDR);

    String payload = "{\"motion_det\":" + String(pir1 || pir2) + ",\"tresspass\":" + String(ldr) +  "}";

    if(pir1 || pir2 || ldr){
        alert();
        sendDataAsync(payload);
    }

    delay(1000);
}