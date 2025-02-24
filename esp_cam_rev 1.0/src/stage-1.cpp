// /*********
//  * Base code by: Rui Santos
//  * Modified by: Jubair
//  * - Added mode switching and MJPEG streaming for improved responsiveness.
//  * - Integrated motion-triggered capture/upload and streaming endpoints.
//  *********/

// #include "Arduino.h"
// #include "WiFi.h"
// #include "esp_camera.h"
// #include "soc/soc.h"          // Disable brownout problems
// #include "soc/rtc_cntl_reg.h" // Disable brownout problems
// #include "driver/rtc_io.h"
// #include <LittleFS.h>
// #include <FS.h>
// #include <Firebase_ESP_Client.h>
// #include <HTTPClient.h>
// #include <WebServer.h>

// // Flask server endpoint (update as needed)
// const char* serverEndpoint = "http://192.168.1.5:5000/upload-url";

// // WiFi credentials
// const char* ssid = "YOUR_WIFI_SSID";
// const char* password = "YOUR_WIFI_PASSWORD";

// // Firebase project settings (update these with your credentials)
// #define FIREBASE_HOST "your-project.firebaseio.com"
// #define FIREBASE_AUTH "YOUR_FIREBASE_AUTH_TOKEN"
// #include <addons/TokenHelper.h>
// #define API_KEY "YOUR_FIREBASE_API_KEY"
// #define USER_EMAIL "your_email@example.com"
// #define USER_PASSWORD "your_password"
// #define STORAGE_BUCKET_ID "your-bucket.appspot.com"

// // File paths for LittleFS and remote storage
// #define FILE_PHOTO_PATH "/photo.jpg"

// // NTP settings for time sync
// const char* ntpServer = "pool.ntp.org";
// const long gmtOffset_sec = 3600 * 6; // Adjust (e.g., GMT+6)
// const int daylightOffset_sec = 0;

// // OV2640 camera module pins (AI Thinker model)
// #define PWDN_GPIO_NUM    32
// #define RESET_GPIO_NUM   -1
// #define XCLK_GPIO_NUM     0
// #define SIOD_GPIO_NUM    26
// #define SIOC_GPIO_NUM    27
// #define Y9_GPIO_NUM      35
// #define Y8_GPIO_NUM      34
// #define Y7_GPIO_NUM      39
// #define Y6_GPIO_NUM      36
// #define Y5_GPIO_NUM      21
// #define Y4_GPIO_NUM      19
// #define Y3_GPIO_NUM      18
// #define Y2_GPIO_NUM       5
// #define VSYNC_GPIO_NUM   25
// #define HREF_GPIO_NUM    23
// #define PCLK_GPIO_NUM    22

// // PIR sensor pin (adjust as needed)
// const int pirPin = 12;

// // Debounce interval (ms) for motion capture
// const unsigned long motionInterval = 6000;
// unsigned long previousMotionMillis = 0;

// // Define camera operating modes
// enum CameraMode {
//   MOTION,
//   STREAM
// };

// volatile CameraMode currentMode = MOTION; // Default mode

// // Create a WebServer instance on port 80
// WebServer server(80);

// // Firebase objects
// FirebaseData fbdo;
// FirebaseAuth auth;
// FirebaseConfig configF;

// // Forward declarations
// void capturePhotoSaveLittleFS(const char* path);
// void initWiFi();
// void initLittleFS();
// void initCamera();
// void sendURLWithTimestamp(const String &url, const String &timestamp);
// bool readMotion();
// void fcsUploadCallback(FCS_UploadStatusInfo info);

// // ----- WebServer Endpoints -----

// // Change mode: POST /mode with "mode" parameter ("motion" or "stream")
// void handleModeChange() {
//   if (server.hasArg("mode")) {
//     String modeArg = server.arg("mode");
//     if (modeArg == "stream") {
//       currentMode = STREAM;
//       Serial.println("Switched to STREAM mode.");
//     } else {
//       currentMode = MOTION;
//       Serial.println("Switched to MOTION mode.");
//     }
//     server.send(200, "text/plain", "Mode set to " + modeArg);
//   } else {
//     server.send(400, "text/plain", "Missing 'mode' parameter");
//   }
// }

// // Get current mode: GET /mode returns current mode string
// void handleGetMode() {
//   String modeStr = (currentMode == MOTION) ? "motion" : "stream";
//   server.send(200, "text/plain", "Current mode: " + modeStr);
// }

// // Force a single capture and return JPEG image: GET /capture
// void handleCapture() {
//   camera_fb_t * fb = esp_camera_fb_get();
//   if (!fb) {
//     Serial.println("Camera capture failed");
//     server.send(500, "text/plain", "Camera capture failed");
//     return;
//   }
//   server.sendHeader("Content-Type", "image/jpeg");
//   server.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
//   esp_camera_fb_return(fb);
// }

// // Stream MJPEG video: GET /stream
// void handleStream() {
//   WiFiClient client = server.client();
//   String response = "HTTP/1.1 200 OK\r\n";
//   response += "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n";
//   server.sendContent(response);

//   Serial.println("Client connected to stream");
//   while (client.connected() && currentMode == STREAM) {
//     camera_fb_t * fb = esp_camera_fb_get();
//     if (!fb) {
//       Serial.println("Camera capture failed in stream");
//       continue;
//     }
//     String header = "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: " + String(fb->len) + "\r\n\r\n";
//     server.sendContent(header);
//     client.write(fb->buf, fb->len);
//     server.sendContent("\r\n");
//     esp_camera_fb_return(fb);
//     delay(100); // Adjust delay for desired frame rate
//     // Break if client disconnects
//     if (!client.connected()) {
//       break;
//     }
//   }
//   Serial.println("Client disconnected from stream");
// }

// // ----- Setup Function -----
// void setup() {
//   Serial.begin(115200);
//   delay(1000);

//   // Initialize WiFi
//   initWiFi();

//   // Initialize LittleFS (and format)
//   initLittleFS();

//   // Disable brownout detector
//   WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

//   // Initialize camera
//   initCamera();

//   // Initialize Firebase
//   configF.api_key = API_KEY;
//   auth.user.email = USER_EMAIL;
//   auth.user.password = USER_PASSWORD;
//   configF.token_status_callback = tokenStatusCallback;
//   Firebase.begin(&configF, &auth);
//   Firebase.reconnectWiFi(true);

//   // Sync time with NTP server
//   configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

//   // Set PIR sensor as input
//   pinMode(pirPin, INPUT);

//   // Set up HTTP server endpoints
//   server.on("/mode", HTTP_POST, handleModeChange);
//   server.on("/mode", HTTP_GET, handleGetMode);
//   server.on("/capture", HTTP_GET, handleCapture);
//   server.on("/stream", HTTP_GET, handleStream);
//   server.begin();
//   Serial.println("HTTP server started on port 80");
//   Serial.println("Setup complete. Default mode: MOTION");
// }

// // ----- Main Loop -----
// void loop() {
//   server.handleClient(); // Handle incoming HTTP requests

//   // If in MOTION mode, check PIR sensor
//   if (currentMode == MOTION) {
//     bool motionDetected = readMotion();
//     unsigned long currentMillis = millis();
//     if (motionDetected && (currentMillis - previousMotionMillis > motionInterval)) {
//       previousMotionMillis = currentMillis;
//       Serial.println("Motion detected! Capturing image...");

//       // Get current time for timestamping
//       time_t now = time(nullptr);

//       // Capture photo and save to LittleFS
//       String fileName = FILE_PHOTO_PATH;
//       capturePhotoSaveLittleFS(fileName.c_str());

//       // Create remote file path with timestamp (e.g., photos/1630000000.jpg)
//       String remoteFilePath = "photos/" + String(now) + ".jpg";

//       // Upload photo to Firebase if ready
//       if (Firebase.ready()) {
//         Serial.print("Uploading picture to Firebase... ");
//         if (Firebase.Storage.upload(&fbdo, STORAGE_BUCKET_ID, fileName.c_str(), mem_storage_type_flash, remoteFilePath, "image/jpeg", fcsUploadCallback)) {
//           Serial.println("Upload success!");
//           Serial.print("Download URL: ");
//           Serial.println(fbdo.downloadURL().c_str());
//           // Notify Flask server with URL and timestamp
//           sendURLWithTimestamp(fbdo.downloadURL(), String(now));
//         } else {
//           Serial.println("Upload failed:");
//           Serial.println(fbdo.errorReason());
//         }
//       } else {
//         Serial.println("Firebase not ready. Skipping upload.");
//       }
//     }
//   }

//   delay(10); // Short delay to avoid hogging CPU
// }

// // ----- Function Implementations -----

// // Capture photo and save to LittleFS
// void capturePhotoSaveLittleFS(const char* path) {
//   // Discard a few initial frames for better quality
//   camera_fb_t* fb = NULL;
//   for (int i = 0; i < 4; i++) {
//     fb = esp_camera_fb_get();
//     if (fb) {
//       esp_camera_fb_return(fb);
//     }
//     delay(100);
//   }

//   // Capture a new photo
//   fb = esp_camera_fb_get();
//   if (!fb) {
//     Serial.println("Camera capture failed! Restarting...");
//     delay(1000);
//     ESP.restart();
//   }

//   File file = LittleFS.open(path, FILE_WRITE);
//   if (!file) {
//     Serial.println("Failed to open file for writing");
//     esp_camera_fb_return(fb);
//     return;
//   }

//   file.write(fb->buf, fb->len);
//   Serial.printf("Photo saved to %s (%d bytes)\n", path, fb->len);
//   file.close();
//   esp_camera_fb_return(fb);
// }

// // Initialize WiFi connection
// void initWiFi() {
//   WiFi.begin(ssid, password);
//   Serial.println("Connecting to WiFi...");
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(1000);
//     Serial.print(".");
//   }
//   Serial.println("\nWiFi connected!");
//   Serial.print("IP address: ");
//   Serial.println(WiFi.localIP());
// }

// // Initialize LittleFS and format if needed
// void initLittleFS() {
//   if (!LittleFS.begin(true)) {
//     Serial.println("Error mounting LittleFS");
//     ESP.restart();
//   } else {
//     delay(500);
//     Serial.println("LittleFS mounted successfully.");
//     Serial.println("Formatting LittleFS...");
//     LittleFS.format();  // Warning: erases all files in LittleFS
//     Serial.println("LittleFS formatted successfully!");
//   }
// }

// // Initialize camera with AI Thinker settings
// void initCamera() {
//   camera_config_t config;
//   config.ledc_channel = LEDC_CHANNEL_0;
//   config.ledc_timer   = LEDC_TIMER_0;
//   config.pin_d0     = Y2_GPIO_NUM;
//   config.pin_d1     = Y3_GPIO_NUM;
//   config.pin_d2     = Y4_GPIO_NUM;
//   config.pin_d3     = Y5_GPIO_NUM;
//   config.pin_d4     = Y6_GPIO_NUM;
//   config.pin_d5     = Y7_GPIO_NUM;
//   config.pin_d6     = Y8_GPIO_NUM;
//   config.pin_d7     = Y9_GPIO_NUM;
//   config.pin_xclk   = XCLK_GPIO_NUM;
//   config.pin_pclk   = PCLK_GPIO_NUM;
//   config.pin_vsync  = VSYNC_GPIO_NUM;
//   config.pin_href   = HREF_GPIO_NUM;
//   config.pin_sccb_sda = SIOD_GPIO_NUM;
//   config.pin_sccb_scl = SIOC_GPIO_NUM;
//   config.pin_pwdn   = PWDN_GPIO_NUM;
//   config.pin_reset  = RESET_GPIO_NUM;
//   config.xclk_freq_hz = 20000000;
//   config.pixel_format = PIXFORMAT_JPEG;
//   config.grab_mode = CAMERA_GRAB_LATEST;

//   if (psramFound()) {
//     config.frame_size = FRAMESIZE_UXGA;
//     config.jpeg_quality = 6;
//     config.fb_count = 1;
//   } else {
//     config.frame_size = FRAMESIZE_SVGA;
//     config.jpeg_quality = 12;
//     config.fb_count = 1;
//   }

//   esp_err_t err = esp_camera_init(&config);
//   if (err != ESP_OK) {
//     Serial.printf("Camera init failed with error 0x%x\n", err);
//     ESP.restart();
//   } else {
//     Serial.println("Camera initialized successfully.");
//   }
// }

// // Send URL and timestamp to Flask server via HTTP POST
// void sendURLWithTimestamp(const String &url, const String &timestamp) {
//   HTTPClient http;
//   http.begin(serverEndpoint);
//   http.addHeader("Content-Type", "application/json");

//   String payload = "{\"url\": \"" + url + "\", \"timestamp\": \"" + timestamp + "\"}";
//   int httpResponseCode = http.POST(payload);

//   if (httpResponseCode > 0) {
//     Serial.println("Flask POST Success:");
//     Serial.println(httpResponseCode);
//     Serial.println(http.getString());
//   } else {
//     Serial.print("Flask POST failed, error: ");
//     Serial.println(http.errorToString(httpResponseCode).c_str());
//   }
//   http.end();
// }

// // Read motion from PIR sensor (HIGH = motion detected)
// bool readMotion() {
//   return digitalRead(pirPin) == HIGH;
// }

// // Firebase Storage upload callback
// void fcsUploadCallback(FCS_UploadStatusInfo info) {
//   if (info.status == firebase_fcs_upload_status_init) {
//     Serial.printf("Uploading file %s (%d bytes) to %s\n", info.localFileName.c_str(), info.fileSize, info.remoteFileName.c_str());
//   } else if (info.status == firebase_fcs_upload_status_upload) {
//     Serial.printf("Upload progress: %d%%, elapsed time: %d ms\n", (int)info.progress, info.elapsedTime);
//   } else if (info.status == firebase_fcs_upload_status_complete) {
//     Serial.println("Upload completed successfully.");
//     FileMetaInfo meta = fbdo.metaData();
//     Serial.printf("Name: %s\nBucket: %s\nContent Type: %s\nSize: %d bytes\nDownload URL: %s\n",
//                   meta.name.c_str(), meta.bucket.c_str(), meta.contentType.c_str(), meta.size, fbdo.downloadURL().c_str());
//   } else if (info.status == firebase_fcs_upload_status_error) {
//     Serial.printf("Upload failed: %s\n", info.errorMsg.c_str());
//   }
// }

/*********
 * Base code by:  Rui Santos
  Complete instructions at: https://RandomNerdTutorials.com/esp32-cam-save-picture-firebase-storage/

  Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

  Based on the example provided by the ESP Firebase Client Library
*********/
/********
 * Modifier: Jubair
 * Used base code and modified:
 * ****************************
 * Interact with flask api
 * Filename modified to unique.
 * ****************************
 ********/

#include "Arduino.h"
#include "WiFi.h"
#include "esp_camera.h"
#include "soc/soc.h"          // Disable brownout problems
#include "soc/rtc_cntl_reg.h" // Disable brownout problems
#include "driver/rtc_io.h"
#include <LittleFS.h>
#include <FS.h>
#include <Firebase_ESP_Client.h>
#include <ArduinoWebsockets.h>
#include <addons/TokenHelper.h>
#include <WebServer.h>
#include <ArduinoJson.h>

const char *serverEndpoint = "http://192.168.0.101:5000/upload-url";
const char *modeEndpoint = "http://192.168.0.101:5000/esp/mode"; // Endpoint to get/set mode

// WiFi credentials
const char *ssid = "Jubayir";
const char *password = "jubair123";

#define FIREBASE_HOST "project-aurixeon-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "MsZUOz2Eybd6FhA68tJ7EHp5TLOBeAe4XTKhN5C2"

// Insert Firebase project API Key
#define API_KEY "AIzaSyATLck7bu6T8VKEamSiBe1WQogeb2lf1mI"

// Insert Authorized Email and Corresponding Password
#define USER_EMAIL "al.nahiyan40@gmail.com"
#define USER_PASSWORD "12345678"

// Insert Firebase storage bucket ID e.g bucket-name.appspot.com
#define STORAGE_BUCKET_ID "project-aurixeon.appspot.com"

// Photo File Name to save in LittleFS
#define FILE_PHOTO_PATH "/photo.jpg"
#define BUCKET_PHOTO "/images/photo.jpg"

// NTP Server details
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600 * 6; // Adjust for your timezone, e.g., 3600 for GMT+1
const int daylightOffset_sec = 0;    // Adjust for daylight saving time

// OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
#define PWDN_GPIO_NUM 32
#define RESET_GPIO_NUM -1
#define XCLK_GPIO_NUM 0
#define SIOD_GPIO_NUM 26
#define SIOC_GPIO_NUM 27
#define Y9_GPIO_NUM 35
#define Y8_GPIO_NUM 34
#define Y7_GPIO_NUM 39
#define Y6_GPIO_NUM 36
#define Y5_GPIO_NUM 21
#define Y4_GPIO_NUM 19
#define Y3_GPIO_NUM 18
#define Y2_GPIO_NUM 5
#define VSYNC_GPIO_NUM 25
#define HREF_GPIO_NUM 23
#define PCLK_GPIO_NUM 22

// Define Firebase Data objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig configF;

// ----- PIR Sensor -----
const int pirPin = 13;                     // adjust as needed
const unsigned long motionInterval = 4000; // minimum interval between captures
unsigned long previousMotionMillis = 0;
// Mode polling configuration
unsigned long previousPollMillis = 0;
const unsigned long pollInterval = 5000; // Poll every 5 seconds
// ----- Modes -----

#define MOTION "motion"
#define STREAM "stream"

String currentMode = MOTION; // default mode
// Server configuration
WiFiServer server(80);

// ----- Function Prototypes -----
void capturePhotoSaveLittleFS(const char *path);
void streamMJPEG(WiFiClient &client);
void initWiFi();
void initLittleFS();
void initCamera();
void sendURLWithTimestamp(const String &url, const String &timestamp);
bool readMotion();
void fcsUploadCallback(FCS_UploadStatusInfo info);
void pollServerMode();

void setup()
{
    Serial.begin(115200);
    initWiFi();
    initLittleFS();
    // Turn-off the 'brownout detector'
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    initCamera();

    // Firebase initialization
    configF.api_key = API_KEY;
    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;
    configF.token_status_callback = tokenStatusCallback;
    Firebase.begin(&configF, &auth);
    Firebase.reconnectNetwork(true);

    // Sync time with NTP server
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    pinMode(pirPin, INPUT);

    server.begin();
    Serial.println("HTTP server started on port 80");
    Serial.print("ESP32-CAM IP: ");
    Serial.println(WiFi.localIP());
}

void loop()
{
    unsigned long currentMillis = millis();
    if (currentMillis - previousPollMillis >= pollInterval)
    {
        pollServerMode();
        previousPollMillis = currentMillis;
    }

    if (currentMode == MOTION)
    {

        bool motionDetected = readMotion();
        unsigned long currentMillis = millis();

        if (motionDetected && (currentMillis - previousMotionMillis > motionInterval))
        {

            previousMotionMillis = currentMillis;
            Serial.println("Motion detected! Capturing image...");

            time_t now = time(nullptr);
            String fileName = "/img_cam.jpg";
            String remoteFilePath = "photos/" + String(now) + ".jpg";
            capturePhotoSaveLittleFS(fileName.c_str());

            if (Firebase.ready())
            {
                Serial.print("Uploading picture... ");
                if (Firebase.Storage.upload(&fbdo, STORAGE_BUCKET_ID /* Firebase Storage bucket id */, fileName.c_str() /* path to local file */, mem_storage_type_flash /* memory storage type, mem_storage_type_flash and mem_storage_type_sd */, remoteFilePath /* path of remote file stored in the bucket */, "image/jpeg" /* mime type */, fcsUploadCallback))
                {
                    Serial.printf("\nDownload URL: %s\n", fbdo.downloadURL().c_str());
                    sendURLWithTimestamp(fbdo.downloadURL(), String(now));
                }
                else
                {
                    Serial.println(fbdo.errorReason());
                }
            }
        }
    }
    else if (currentMode == STREAM)
    {
        WiFiClient client = server.available();
        if(client) streamMJPEG(client);
        // client.stop();
    }

    delay(50);
}

void streamMJPEG(WiFiClient &client)
{
    // Start MJPEG stream
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: multipart/x-mixed-replace; boundary=frame");
    client.println();

    while (client.connected() && currentMode == STREAM)
    {
        camera_fb_t *fb = esp_camera_fb_get(); // Get a frame from the camera
        if (!fb)
        {
            Serial.println("Camera capture failed");
            return;
        }

        // Send the MJPEG frame to the client
        client.printf("--frame\r\nContent-Type: image/jpeg\r\n\r\n");
        client.write(fb->buf, fb->len);
        client.println("\r\n");
        client.flush();

        esp_camera_fb_return(fb); // Return the frame buffer to the camera driver

        unsigned long currentMillis = millis();
        if (currentMillis - previousPollMillis >= pollInterval)
        {
            pollServerMode();
            previousPollMillis = currentMillis;
        }
        Serial.print(".-");
        delay(10); // Adjust the delay for the frame rate
    }
    Serial.print(".");
}

// Poll the Flask server for the current mode
void pollServerMode()
{
    HTTPClient http;
    http.begin(modeEndpoint); // Connect to the mode endpoint
    int httpResponseCode = http.GET();

    if (httpResponseCode > 0)
    {
        String payload = http.getString();
        DynamicJsonDocument doc(256);
        DeserializationError error = deserializeJson(doc, payload);
        if (!error)
        {
            currentMode = doc["mode"].as<String>();
            Serial.println("Polled mode: " + currentMode);
        }
        else
        {
            Serial.println("Failed to parse JSON from mode endpoint");
        }
    }
    else
    {
        Serial.print("Mode polling failed, error: ");
        Serial.println(http.errorToString(httpResponseCode));
    }
    http.end();
}

void capturePhotoSaveLittleFS(const char *path)
{
    // Dispose first pictures because of bad quality
    camera_fb_t *fb = NULL;
    // Skip first 3 frames (increase/decrease number as needed).
    for (int i = 0; i < 4; i++)
    {
        fb = esp_camera_fb_get();
        esp_camera_fb_return(fb);
        fb = NULL;
    }

    // Take a new photo
    fb = NULL;
    fb = esp_camera_fb_get();
    if (!fb)
    {
        Serial.println("Camera capture failed");
        delay(1000);
        ESP.restart();
    }

    // Photo file name
    Serial.printf("Picture file name: %s\n", path);
    File file = LittleFS.open(path, FILE_WRITE);

    // Insert the data in the photo file
    if (!file)
    {
        Serial.println("Failed to open file in writing mode");
    }
    else
    {
        file.write(fb->buf, fb->len); // payload (image), payload length
        Serial.print("The picture has been saved in ");
        Serial.print(path);
        Serial.print(" - Size: ");
        Serial.print(fb->len);
        Serial.println(" bytes");
    }
    // Close the file
    file.close();
    esp_camera_fb_return(fb);
}

void initWiFi()
{
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
}

void initLittleFS()
{
    if (!LittleFS.begin(true))
    {
        Serial.println("An Error has occurred while mounting LittleFS");
        ESP.restart();
    }
    else
    {
        delay(500);
        Serial.println("LittleFS mounted successfully");
        Serial.println("Formatting LittleFS...");
        LittleFS.format(); // This erases all data in LittleFS
        Serial.println("LittleFS formatted successfully!");
    }
}

void initCamera()
{
    // OV2640 camera module
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sccb_sda = SIOD_GPIO_NUM;
    config.pin_sccb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.grab_mode = CAMERA_GRAB_LATEST;

    if (psramFound())
    {
        config.frame_size = FRAMESIZE_UXGA;
        config.jpeg_quality = 6;
        config.fb_count = 1;
    }
    else
    {
        config.frame_size = FRAMESIZE_SVGA;
        config.jpeg_quality = 12;
        config.fb_count = 1;
    }
    // Camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        ESP.restart();
    }
}

void sendURLWithTimestamp(const String &url, const String &timestamp)
{
    HTTPClient http;
    http.begin(serverEndpoint);

    http.addHeader("Content-Type", "application/json");

    String payload = "{\"url\": \"" + url + "\", \"timestamp\": \"" + timestamp + "\"}";

    int httpResponseCode = http.POST(payload);

    if (httpResponseCode > 0)
    {
        Serial.println("POST Success:");
        Serial.println(httpResponseCode);
        Serial.println(http.getString());
    }
    else
    {
        Serial.print("POST failed, error: ");
        Serial.println(http.errorToString(httpResponseCode).c_str());
    }

    http.end();
}

bool readMotion()
{
    return digitalRead(pirPin) == HIGH;
}

// The Firebase Storage upload callback function
void fcsUploadCallback(FCS_UploadStatusInfo info)
{
    if (info.status == firebase_fcs_upload_status_init)
    {
        Serial.printf("Uploading file %s (%d) to %s\n", info.localFileName.c_str(), info.fileSize, info.remoteFileName.c_str());
    }
    else if (info.status == firebase_fcs_upload_status_upload)
    {
        Serial.printf("Uploaded %d%s, Elapsed time %d ms\n", (int)info.progress, "%", info.elapsedTime);
    }
    else if (info.status == firebase_fcs_upload_status_complete)
    {
        Serial.println("Upload completed\n");
        FileMetaInfo meta = fbdo.metaData();
        Serial.printf("Name: %s\n", meta.name.c_str());
        Serial.printf("Bucket: %s\n", meta.bucket.c_str());
        Serial.printf("contentType: %s\n", meta.contentType.c_str());
        Serial.printf("Size: %d\n", meta.size);
        Serial.printf("Generation: %lu\n", meta.generation);
        Serial.printf("Metageneration: %lu\n", meta.metageneration);
        Serial.printf("ETag: %s\n", meta.etag.c_str());
        Serial.printf("CRC32: %s\n", meta.crc32.c_str());
        Serial.printf("Tokens: %s\n", meta.downloadTokens.c_str());
        Serial.printf("Download URL: %s\n\n", fbdo.downloadURL().c_str());
    }
    else if (info.status == firebase_fcs_upload_status_error)
    {
        Serial.printf("Upload failed, %s\n", info.errorMsg.c_str());
    }
}
