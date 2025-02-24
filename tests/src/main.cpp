// // #include <main.h>
// #include "Display.h"


// void setup()
// {
//     Serial.begin(115200);
//     initializeDisplay();
//     pinMode(17, INPUT);

// }

// void loop()
// {
//     // want to do asyncronously
//     // String disp[] = {
//     //     "Fire: ignited",
//     //     "Gas leaked",
//     //     "Temp: 25°C",
//     //     "Humid: 80%",
//     //     "Frequency: 300 Hz"};

//     // const int numTexts = sizeof(disp) / sizeof(disp[0]);
//     // scrollAllText(disp, numTexts, 100);


//     delay(800);
// }

#include <Arduino.h>

// Define the analog input pin for the sound sensor
const int soundSensorPin = 33;  // Can use any available ADC pin on ESP32

// Variables for storing sensor readings
int soundLevel = 0;
int threshold = 500;    // Sensitivity threshold, adjust as needed

void setup() {
  Serial.begin(115200);   // Start serial communication
//   pinMode(soundSensorPin, INPUT);  // Set the sound sensor pin as input
  printf("Setup done\n");
}

void loop() {
  // Get the current sound level
  soundLevel = analogReadRaw(soundSensorPin);

  // Print the sound level and threshold
  Serial.print("Sound Level: ");
  Serial.println(soundLevel);
  Serial.print("Threshold: ");
  Serial.println(threshold);  // Display the threshold value
  Serial.println("\n\n");

  delay(150);  // Delay between readings to make it stable
}
