#include <main.h>


void setup()
{
  Serial.begin(115200);
  initializeDisplay();

  initializeWiFi();
  initializeSensors();
}

void loop()
{

  // Read sensors
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  bool fire_detected = !(digitalRead(FIRE_SENSOR));
  bool gas_detected = !(digitalRead(GAS_SENSOR));
  int sound_freq = analogRead(SOUND_SENSOR);


  
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.println("%");

  Serial.print("Fire Detected: ");
  Serial.println(fire_detected);
  

  Serial.print("Gas Detected: ");
  Serial.println(gas_detected);

  Serial.print("Frequency: ");
  Serial.println(sound_freq);
  Serial.println();


  if (isnan(temperature) || isnan(humidity))
  {
    Serial.println("Failed to read from DHT sensor! Using fallback values.");
    temperature = -1; // Fallback value
    humidity = -1;    // Fallback value
  }

  // ********************************
  // Data based actions

  // if(fire_detected || gas_detected) soundBuzzer(1000);

  String __f_text = "";
  if (fire_detected)
  {
    __f_text = "Fire detected!";
    digitalWrite(RELAY_PIN, HIGH);
    delay(800);
    digitalWrite(RELAY_PIN, LOW);
  }
  String __g_text = "";
  if (gas_detected)
    __g_text = "Gas detected";
  // ********************************

  // want to do asyncronously
  String disp[] = {
      __f_text,
      __g_text,
      "Temp: " + String(temperature) + "°C",
      "Humid: " + String(humidity) + "%",
      "Frequency: " + String(sound_freq) + " Hz"};

  const int numTexts = sizeof(disp) / sizeof(disp[0]);
  // scrollAllText(disp, numTexts, 100);

  int currentTimeHttp = millis();

  if (currentTimeHttp - prevTimeHttp >= intervalHttp)
  {
    prevTimeHttp = currentTimeHttp;
    String payload = "{\"fire_detected\": " + String(fire_detected) +
                     ", \"gas_detected\": " + String(gas_detected) +
                     ", \"temperature\": " + String(temperature) +
                     ", \"humidity\": " + String(humidity) +
                     ", \"sound_freq\": " + String(sound_freq) + "}";

    if (WiFi.status() == WL_CONNECTED)
    {
      HTTPClient http;

      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");
      int httpResponseCode = http.POST(payload);

      if (httpResponseCode > 0)
        Serial.println("Response Code: " + String(httpResponseCode));
      else
        Serial.println("Error in sending data. Response Code: " + String(httpResponseCode));

      http.end();
    }
    else
      Serial.println("WiFi not connected");
  }
  delay(800);
}
