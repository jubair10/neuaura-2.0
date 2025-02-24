// /*********
//  * Base code by:  Rui Santos
//   Complete instructions at: https://RandomNerdTutorials.com/esp32-cam-save-picture-firebase-storage/

//   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files.
//   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

//   Based on the example provided by the ESP Firebase Client Library
// *********/
// /******** 
//  * Modifier: Jubair
//  * Used base code and modified:
//  * ****************************
//  * Interact with flask api
//  * Filename modified to unique.
//  * ****************************
// ********/
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

// const char *serverEndpoint = "http://192.168.1.5:5000/upload-url";

// // WiFi credentials
// const char *ssid = "Samir";
// const char *password = "samir9050";

// #define FIREBASE_HOST "project-aurixeon-default-rtdb.firebaseio.com"
// #define FIREBASE_AUTH "MsZUOz2Eybd6FhA68tJ7EHp5TLOBeAe4XTKhN5C2"
// #include <addons/TokenHelper.h>

// // Insert Firebase project API Key
// #define API_KEY "AIzaSyATLck7bu6T8VKEamSiBe1WQogeb2lf1mI"

// // Insert Authorized Email and Corresponding Password
// #define USER_EMAIL "al.nahiyan40@gmail.com"
// #define USER_PASSWORD "12345678"

// // Insert Firebase storage bucket ID e.g bucket-name.appspot.com
// #define STORAGE_BUCKET_ID "project-aurixeon.appspot.com"

// // Photo File Name to save in LittleFS
// #define FILE_PHOTO_PATH "/photo.jpg"
// #define BUCKET_PHOTO "/images/photo.jpg"

// // NTP Server details
// const char *ntpServer = "pool.ntp.org";
// const long gmtOffset_sec = 3600 * 6; // Adjust for your timezone, e.g., 3600 for GMT+1
// const int daylightOffset_sec = 0;    // Adjust for daylight saving time


// // OV2640 camera module pins (CAMERA_MODEL_AI_THINKER)
// #define PWDN_GPIO_NUM 32
// #define RESET_GPIO_NUM -1
// #define XCLK_GPIO_NUM 0
// #define SIOD_GPIO_NUM 26
// #define SIOC_GPIO_NUM 27
// #define Y9_GPIO_NUM 35
// #define Y8_GPIO_NUM 34
// #define Y7_GPIO_NUM 39
// #define Y6_GPIO_NUM 36
// #define Y5_GPIO_NUM 21
// #define Y4_GPIO_NUM 19
// #define Y3_GPIO_NUM 18
// #define Y2_GPIO_NUM 5
// #define VSYNC_GPIO_NUM 25
// #define HREF_GPIO_NUM 23
// #define PCLK_GPIO_NUM 22

// boolean takeNewPhoto = true;

// // Define Firebase Data objects
// FirebaseData fbdo;
// FirebaseAuth auth;
// FirebaseConfig configF;


// bool taskCompleted = false;
// int pirPin = 12;

// void fcsUploadCallback(FCS_UploadStatusInfo info);


// void capturePhotoSaveLittleFS(const char *path)
// {
//   // Dispose first pictures because of bad quality
//   camera_fb_t *fb = NULL;
//   // Skip first 3 frames (increase/decrease number as needed).
//   for (int i = 0; i < 4; i++)
//   {
//     fb = esp_camera_fb_get();
//     esp_camera_fb_return(fb);
//     fb = NULL;
//   }

//   // Take a new photo
//   fb = NULL;
//   fb = esp_camera_fb_get();
//   if (!fb)
//   {
//     Serial.println("Camera capture failed");
//     delay(1000);
//     ESP.restart();
//   }

//   // Photo file name
//   Serial.printf("Picture file name: %s\n", path);
//   File file = LittleFS.open(path, FILE_WRITE);

//   // Insert the data in the photo file
//   if (!file)
//   {
//     Serial.println("Failed to open file in writing mode");
//   }
//   else
//   {
//     file.write(fb->buf, fb->len); // payload (image), payload length
//     Serial.print("The picture has been saved in ");
//     Serial.print(path);
//     Serial.print(" - Size: ");
//     Serial.print(fb->len);
//     Serial.println(" bytes");
//   }
//   // Close the file
//   file.close();
//   esp_camera_fb_return(fb);
// }

// void initWiFi()
// {
//   WiFi.begin(ssid, password);
//   while (WiFi.status() != WL_CONNECTED)
//   {
//     delay(1000);
//     Serial.println("Connecting to WiFi...");
//   }
// }

// void initLittleFS()
// {
//   if (!LittleFS.begin(true))
//   {
//     Serial.println("An Error has occurred while mounting LittleFS");
//     ESP.restart();
//   }
//   else
//   {
//     delay(500);
//     Serial.println("LittleFS mounted successfully");
//     Serial.println("Formatting LittleFS...");
//     LittleFS.format(); // This erases all data in LittleFS
//     Serial.println("LittleFS formatted successfully!");
//   }
// }

// void initCamera()
// {
//   // OV2640 camera module
//   camera_config_t config;
//   config.ledc_channel = LEDC_CHANNEL_0;
//   config.ledc_timer = LEDC_TIMER_0;
//   config.pin_d0 = Y2_GPIO_NUM;
//   config.pin_d1 = Y3_GPIO_NUM;
//   config.pin_d2 = Y4_GPIO_NUM;
//   config.pin_d3 = Y5_GPIO_NUM;
//   config.pin_d4 = Y6_GPIO_NUM;
//   config.pin_d5 = Y7_GPIO_NUM;
//   config.pin_d6 = Y8_GPIO_NUM;
//   config.pin_d7 = Y9_GPIO_NUM;
//   config.pin_xclk = XCLK_GPIO_NUM;
//   config.pin_pclk = PCLK_GPIO_NUM;
//   config.pin_vsync = VSYNC_GPIO_NUM;
//   config.pin_href = HREF_GPIO_NUM;
//   config.pin_sccb_sda = SIOD_GPIO_NUM;
//   config.pin_sccb_scl = SIOC_GPIO_NUM;
//   config.pin_pwdn = PWDN_GPIO_NUM;
//   config.pin_reset = RESET_GPIO_NUM;
//   config.xclk_freq_hz = 20000000;
//   config.pixel_format = PIXFORMAT_JPEG;
//   config.grab_mode = CAMERA_GRAB_LATEST;

//   if (psramFound())
//   {
//     config.frame_size = FRAMESIZE_UXGA;
//     config.jpeg_quality = 6;
//     config.fb_count = 1;
//   }
//   else
//   {
//     config.frame_size = FRAMESIZE_SVGA;
//     config.jpeg_quality = 12;
//     config.fb_count = 1;
//   }
//   // Camera init
//   esp_err_t err = esp_camera_init(&config);
//   if (err != ESP_OK)
//   {
//     Serial.printf("Camera init failed with error 0x%x", err);
//     ESP.restart();
//   }
// }

// void sendURLWithTimestamp(const String &url, const String &timestamp)
// {
//   HTTPClient http;
//   http.begin(serverEndpoint);

//   http.addHeader("Content-Type", "application/json");

//   String payload = "{\"url\": \"" + url + "\", \"timestamp\": \"" + timestamp + "\"}";

//   int httpResponseCode = http.POST(payload);

//   if (httpResponseCode > 0)
//   {
//     Serial.println("POST Success:");
//     Serial.println(httpResponseCode);
//     Serial.println(http.getString());
//   }
//   else
//   {
//     Serial.print("POST failed, error: ");
//     Serial.println(http.errorToString(httpResponseCode).c_str());
//   }

//   http.end();
// }

// bool readMotion() {
//   return digitalRead(pirPin) == HIGH;  
// }

// void setup()
// {
//   Serial.begin(115200);
//   initWiFi();
//   initLittleFS();
//   // Turn-off the 'brownout detector'
//   WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
//   initCamera();

//   // Firebase initialization
//   configF.api_key = API_KEY;
//   auth.user.email = USER_EMAIL;
//   auth.user.password = USER_PASSWORD;
//   configF.token_status_callback = tokenStatusCallback; 
//   Firebase.begin(&configF, &auth);
//   Firebase.reconnectWiFi(true);

//   // Sync time with NTP server
//   configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

//   pinMode(pirPin, INPUT);
// }

// unsigned long previousMillis = 0;
// const unsigned long interval = 6000; 

// void loop()
// {
//   bool motionDetected = readMotion();

//   if(motionDetected){
//     Serial.println("Motion detected");

//   }
//   else Serial.println("Motion is not detected");
  

//   unsigned long currentMillis = millis();
//   if (currentMillis - previousMillis >= interval)
//   {
//     previousMillis = currentMillis;

//     time_t now = time(nullptr);
//     String fileName = "/img_cam.jpg";
//     String remoteFilePath = "photos/"+ String(now) +".jpg";
//     capturePhotoSaveLittleFS(fileName.c_str());
//     if (Firebase.ready())
//     {
//       Serial.print("Uploading picture... ");
//       if (Firebase.Storage.upload(&fbdo, STORAGE_BUCKET_ID /* Firebase Storage bucket id */, fileName.c_str() /* path to local file */, mem_storage_type_flash /* memory storage type, mem_storage_type_flash and mem_storage_type_sd */, remoteFilePath /* path of remote file stored in the bucket */, "image/jpeg" /* mime type */, fcsUploadCallback))
//       {
//         Serial.printf("\nDownload URL: %s\n", fbdo.downloadURL().c_str());
//         sendURLWithTimestamp(fbdo.downloadURL(), String(now));
//       }
//       else
//       {
//         Serial.println(fbdo.errorReason());
//       }
//     }
//   }
  
//   delay(1000);
// }

// // The Firebase Storage upload callback function
// void fcsUploadCallback(FCS_UploadStatusInfo info)
// {
//   if (info.status == firebase_fcs_upload_status_init)
//   {
//     Serial.printf("Uploading file %s (%d) to %s\n", info.localFileName.c_str(), info.fileSize, info.remoteFileName.c_str());
//   }
//   else if (info.status == firebase_fcs_upload_status_upload)
//   {
//     Serial.printf("Uploaded %d%s, Elapsed time %d ms\n", (int)info.progress, "%", info.elapsedTime);
//   }
//   else if (info.status == firebase_fcs_upload_status_complete)
//   {
//     Serial.println("Upload completed\n");
//     FileMetaInfo meta = fbdo.metaData();
//     Serial.printf("Name: %s\n", meta.name.c_str());
//     Serial.printf("Bucket: %s\n", meta.bucket.c_str());
//     Serial.printf("contentType: %s\n", meta.contentType.c_str());
//     Serial.printf("Size: %d\n", meta.size);
//     Serial.printf("Generation: %lu\n", meta.generation);
//     Serial.printf("Metageneration: %lu\n", meta.metageneration);
//     Serial.printf("ETag: %s\n", meta.etag.c_str());
//     Serial.printf("CRC32: %s\n", meta.crc32.c_str());
//     Serial.printf("Tokens: %s\n", meta.downloadTokens.c_str());
//     Serial.printf("Download URL: %s\n\n", fbdo.downloadURL().c_str());
//   }
//   else if (info.status == firebase_fcs_upload_status_error)
//   {
//     Serial.printf("Upload failed, %s\n", info.errorMsg.c_str());
//   }
// }
