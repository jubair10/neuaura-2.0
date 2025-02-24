#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <AsyncTCP.h>          // For ESP32
#include <ESPAsyncWebServer.h> // For ESP8266/ESP32
#include <AsyncHTTPRequest_Generic.h> // Async HTTP Client

#define LDR 10
#define PIR_1 11
#define PIR_2 12
#define BUZZER 13
#define THRESHOLD_LDR 300

#define ENDPOINT "www.sth.com"
#define SSID "Samir"
#define PASSWORD "samir9050"

void initializeWiFi()
{
    WiFi.begin(SSID, PASSWORD);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(500);
    }
    Serial.println("\nConnected to WiFi");
}

void initializeSensors()
{
    pinMode(LDR, INPUT);
    pinMode(PIR_1, INPUT);
    pinMode(PIR_2, INPUT);
    pinMode(BUZZER, OUTPUT);
}

bool readPIR(int pin)
{
    bool in = digitalRead(pin);

    return in;
}

bool readLDR(int pin){
    int ldrValue = analogRead(LDR);

    return ldrValue <= THRESHOLD_LDR;
}

void alert(){
    digitalWrite(BUZZER, HIGH);
    delay(500);
    digitalWrite(BUZZER, LOW);
}

void sendData(String payload){

    if(WiFi.status() == WL_CONNECTED){
        HTTPClient client;

        client.begin(ENDPOINT);
        client.addHeader("Content-Type", "application/json");

        int response = client.POST(payload);
        Serial.println("Response Code: " + String(response));
        client.end();
    }
    else Serial.println("NetWork Issue: " + String(WiFi.status()));
}

void sendDataAsync(String payload) {
    if (WiFi.status() == WL_CONNECTED) {
        AsyncHTTPRequest request;

        if (request.open("POST", ENDPOINT)) {
            request.setReqHeader("Content-Type", "application/json");

            // Callback for handling the response
            request.onReadyStateChange([](void* optParm, AsyncHTTPRequest* req, int readyState) {
                if (readyState == 4) { // Request completed
                    Serial.print("Response Code: ");
                    Serial.println(req->responseHTTPcode());
                    Serial.print("Response Body: ");
                    Serial.println(req->responseText());
                }
            });

            // Send payload
            request.send(payload);
        } else {
            Serial.println("Failed to open request");
        }
    } else {
        Serial.println("Network Issue: " + String(WiFi.status()));
    }
}