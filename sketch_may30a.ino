#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "base64.h" // Include base64 library
#include "I2S.h"

#define SAMPLE_RATE (16000)
#define PIN_I2S_BCLK 26
#define PIN_I2S_LRC 22
#define PIN_I2S_DIN 34
#define PIN_I2S_DOUT 25

const char* ssid = "HURAA";
const char* password = "12345678";
const char* googleApiKey = "AIzaSyC2o8YuOn_GuA5uWQns8Zt6wIHnUGC2oJw";

// Root certificate for "speech.googleapis.com"
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n"
"MIIFljCCA36gAwIBAgINAgO8U1lrNMcY9QFQZjANBgkqhkiG9w0BAQsFADBHMQsw\n"
"CQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZpY2VzIExMQzEU\n"
"MBIGA1UEAxMLR1RTIFJvb3QgUjEwHhcNMjAwODEzMDAwMDQyWhcNMjcwOTMwMDAw\n"
"MDQyWjBGMQswCQYDVQQGEwJVUzEiMCAGA1UEChMZR29vZ2xlIFRydXN0IFNlcnZp\n"
"Y2VzIExMQzETMBEGA1UEAxMKR1RTIENBIDFDMzCCASIwDQYJKoZIhvcNAQEBBQAD\n"
"ggEPADCCAQoCggEBAPWI3+dijB43+DdCkH9sh9D7ZYIl/ejLa6T/belaI+KZ9hzp\n"
"kgOZE3wJCor6QtZeViSqejOEH9Hpabu5dOxXTGZok3c3VVP+ORBNtzS7XyV3NzsX\n"
"lOo85Z3VvMO0Q+sup0fvsEQRY9i0QYXdQTBIkxu/t/bgRQIh4JZCF8/ZK2VWNAcm\n"
"BA2o/X3KLu/qSHw3TT8An4Pf73WELnlXXPxXbhqW//yMmqaZviXZf5YsBvcRKgKA\n"
"gOtjGDxQSYflispfGStZloEAoPtR28p3CwvJlk/vcEnHXG0g/Zm0tOLKLnf9LdwL\n"
"tmsTDIwZKxeWmLnwi/agJ7u2441Rj72ux5uxiZ0CAwEAAaOCAYAwggF8MA4GA1Ud\n"
"DwEB/wQEAwIBhjAdBgNVHSUEFjAUBggrBgEFBQcDAQYIKwYBBQUHAwIwEgYDVR0T\n"
"AQH/BAgwBgEB/wIBADAdBgNVHQ4EFgQUinR/r4XN7pXNPZzQ4kYU83E1HScwHwYD\n"
"VR0jBBgwFoAU5K8rJnEaK0gnhS9SZizv8IkTcT4waAYIKwYBBQUHAQEEXDBaMCYG\n"
"CCsGAQUFBzABhhpodHRwOi8vb2NzcC5wa2kuZ29vZy9ndHNyMTAwBggrBgEFBQcw\n"
"AoYkaHR0cDovL3BraS5nb29nL3JlcG8vY2VydHMvZ3RzcjEuZGVyMDQGA1UdHwQt\n"
"MCswKaAnoCWGI2h0dHA6Ly9jcmwucGtpLmdvb2cvZ3RzcjEvZ3RzcjEuY3JsMFcG\n"
"A1UdIARQME4wOAYKKwYBBAHWeQIFAzAqMCgGCCsGAQUFBwIBFhxodHRwczovL3Br\n"
"aS5nb29nL3JlcG9zaXRvcnkvMAgGBmeBDAECATAIBgZngQwBAgIwDQYJKoZIhvcN\n"
"AQELBQADggIBAIl9rCBcDDy+mqhXlRu0rvqrpXJxtDaV/d9AEQNMwkYUuxQkq/BQ\n"
"cSLbrcRuf8/xam/IgxvYzolfh2yHuKkMo5uhYpSTld9brmYZCwKWnvy15xBpPnrL\n"
"RklfRuFBsdeYTWU0AIAaP0+fbH9JAIFTQaSSIYKCGvGjRFsqUBITTcFTNvNCCK9U\n"
"+o53UxtkOCcXCb1YyRt8OS1b887U7ZfbFAO/CVMkH8IMBHmYJvJh8VNS/UKMG2Yr\n"
"PxWhu//2m+OBmgEGcYk1KCTd4b3rGS3hSMs9WYNRtHTGnXzGsYZbr8w0xNPM1IER\n"
"lQCh9BIiAfq0g3GvjLeMcySsN1PCAJA/Ef5c7TaUEDu9Ka7ixzpiO2xj2YC/WXGs\n"
"Yye5TBeg2vZzFb8q3o/zpWwygTMD0IZRcZk0upONXbVRWPeyk+gB9lm+cZv9TSjO\n"
"z23HFtz30dZGm6fKa+l3D/2gthsjgx0QGtkJAITgRNOidSOzNIb2ILCkXhAd4FJG\n"
"AJ2xDx8hcFH1mt0G/FX0Kw4zd8NLQsLxdxP8c4CU6x+7Nz/OAipmsHMdMqUybDKw\n"
"juDEI/9bfU1lcKwrmz3O2+BtjjKAvpafkmO8l7tdufThcV4q5O8DIrGKZTqPwJNl\n"
"1IXNDw9bg1kWRxYtnCQ6yICmJhSFm/Y3m6xv+cXDBlHz4n/FsRC6UfTd\n"
"-----END CERTIFICATE-----\n";


WiFiClientSecure client;

// Define the size of the audio buffer
#define AUDIO_BUFFER_SIZE 1024
uint8_t audioBuffer[AUDIO_BUFFER_SIZE];
size_t audioDataLength; // Variable to store the length of audio data

bool recordingStarted = false;

I2S i2sMic(ICS43434); // Change to your microphone type if needed

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Initialize WiFi connection
  connectWiFi();

  // Initialize I2S (handled by I2S class constructor)
  
  // Wait for WiFi connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi");
}

void loop() {
  // Read audio input from I2S microphone and fill audioBuffer
  audioDataLength = i2sMic.Read((char*)audioBuffer, AUDIO_BUFFER_SIZE);

  // Check if there is audio input
  if (isAudioDetected(audioBuffer, audioDataLength)) {
    if (!recordingStarted) {
      Serial.println("RECORD START");
      recordingStarted = true;
    }

    // Convert audio to text using Google Cloud Speech-to-Text API
    String text = convertSpeechToText(audioBuffer, audioDataLength);

    // Print the recognized text
    Serial.println("Recognized Text: " + text);
  } else {
    if (recordingStarted) {
      Serial.println("RECORD MATI");
      recordingStarted = false;
    }
  }

  delay(5000); // Delay before next recognition
}

void connectWiFi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
}

bool isAudioDetected(uint8_t* buffer, size_t length) {
  int16_t* samples = (int16_t*)buffer;
  for (size_t i = 0; i < length / 2; i++) {
    if (abs(samples[i]) > 1000) { // Adjust threshold as necessary
      return true;
    }
  }
  return false;
}

String convertSpeechToText(uint8_t* audioData, size_t dataLength) {
  String result = "";

  client.setCACert(rootCACertificate);
  if (!client.connect("speech.googleapis.com", 443)) {
    Serial.println("Connection failed");
    return result;
  }

  // Encode audio data to base64
  String encodedData = base64::encode(audioData, dataLength);

  // Create JSON object
  DynamicJsonDocument doc(1024);
  JsonObject config = doc.createNestedObject("config");
  config["encoding"] = "LINEAR16";
  config["sampleRateHertz"] = SAMPLE_RATE;
  config["languageCode"] = "en-US";

  JsonObject audio = doc.createNestedObject("audio");
  audio["content"] = encodedData;

  String jsonString;
  serializeJson(doc, jsonString);

  // Build HTTP request
  String httpRequest = "POST /v1/speech:recognize?key=";
  httpRequest += googleApiKey;
  httpRequest += " HTTP/1.1\r\n";
  httpRequest += "Host: speech.googleapis.com\r\n";
  httpRequest += "Content-Type: application/json\r\n";
  httpRequest += "Content-Length: ";
  httpRequest += String(jsonString.length());
  httpRequest += "\r\n\r\n";
  httpRequest += jsonString;

  // Send HTTP request
  client.print(httpRequest);

  // Read response
  while (client.connected() || client.available()) {
    if (client.available()) {
      String line = client.readStringUntil('\n');
      Serial.println("Received line: " + line); // Debug output
      if (line.startsWith("{\"results\":")) {
        DynamicJsonDocument responseDoc(1024);
        deserializeJson(responseDoc, line);
        JsonArray results = responseDoc["results"];
        if (!results.isNull() && results.size() > 0) {
          JsonObject firstResult = results[0];
          JsonArray alternatives = firstResult["alternatives"];
          if (!alternatives.isNull() && alternatives.size() > 0) {
            result = alternatives[0]["transcript"].as<String>();
          }
        }
        break;
      }
    }
  }

  return result;
}
