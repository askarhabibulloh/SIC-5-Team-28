#include <WiFi.h>
#include <HTTPClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ArduinoJson.h>

const char* ssid = "Ngomah";
const char* password = "penginapan";
const char* serverName = "your-server-address/upload";
const char* transcriptionServer = "your-server-address/transcription";

const int analogPin = 34;
const int buttonPin = 25;
const int sampleRate = 16000;
const int sampleWindow = 200;
const int maxSamples = 3200;
const int repetitions = 20;
const int delayBetweenSamples = 0;

bool isRecording = false;

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.setTextSize(1);      
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Connecting to WiFi...");
    display.display();
  }
  Serial.println("Connected to WiFi");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("Connected to WiFi");
  display.display();
}

void loop() {
  if (digitalRead(buttonPin) == LOW && !isRecording) {
    isRecording = true;

    if (WiFi.status() == WL_CONNECTED) {
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Merekam...");
      display.display();

      for (int i = 0; i < repetitions; i++) {
        unsigned long startMillis = millis();
        String postData = "";
        int sampleCount = 0;

        while (millis() - startMillis < sampleWindow && sampleCount < maxSamples) {
          int sample = analogRead(analogPin);
          sample = map(sample, 0, 4095, -32768, 32767);
          postData += String(sample) + ",";
          sampleCount++;
        }

        postData = postData.substring(0, postData.length() - 1);

        HTTPClient http;
        http.begin(serverName);
        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        String httpRequestData = String("audioData=") + postData;
        int httpResponseCode = http.POST(httpRequestData);
        http.end();

        delay(delayBetweenSamples);
      }

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Perekaman selesai");
      display.display();
      delay(2000);

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("Memproses...");
      display.display();

      delay(10000);
      String response = "";
      bool success = false;

      for (int i = 0; i < 3; i++) {
        HTTPClient http;
        http.begin(transcriptionServer);
        int httpResponseCode = http.GET();

        if (httpResponseCode > 0) {
          response = http.getString();
          success = true;
          break;
        } else {
          response = "Error on HTTP request";
          delay(5000);
        }

        http.end();
      }

      if (!success) {
        response = "{\"status\":\"error\",\"message\":\"Failed to get transcription\"}";
      }

      DynamicJsonDocument doc(1024);
      deserializeJson(doc, response);

      String status = doc["status"];
      String transcript = status == "success" ? doc["transcript"] : doc["message"];

      display.clearDisplay();
      display.setCursor(0, 0);
      display.println(transcript);
      display.display();
      delay(5000);
    } else {
      Serial.println("WiFi Disconnected");
      display.clearDisplay();
      display.setCursor(0, 0);
      display.println("WiFi Disconnected");
      display.display();
      delay(1000);
    }

    isRecording = false;
  }
}
