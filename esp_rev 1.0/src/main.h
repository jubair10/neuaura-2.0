#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>
#include "Display.h"

// Sensor configuration
#define DHTPIN 5
#define DHTTYPE DHT11

#define GAS_SENSOR 18
#define FIRE_SENSOR 15
#define SOUND_SENSOR 35

#define BUZZER 32
#define RELAY_PIN 16

DHT dht(DHTPIN, DHTTYPE);

// Credentials
const char *ssid = "Samir";
const char *password = "samir9050";
const String serverUrl = "http://192.168.1.5:5000/esp-data";

int prevTimeHttp = millis();
int intervalHttp = 4000;

void initializeWiFi()
{
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print('.');
        delay(500);
    }
    Serial.println("\nConnected to WiFi");
}


// Sensor pin initialization
void initializeSensors()
{
    pinMode(GAS_SENSOR, INPUT);
    pinMode(FIRE_SENSOR, INPUT);

    pinMode(RELAY_PIN, OUTPUT);
    digitalWrite(RELAY_PIN, LOW);

    dht.begin();
}


void showDetails()
