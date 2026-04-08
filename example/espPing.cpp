#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <ESP32Ping.h>
#include <PubSubClient.h>
#include <BMP280.h>
#include <Wire.h>

// --- WIFI ---
const char* ssid = "iPhone (Anna)";
const char* password = "natu12345";

// --- MQTT ---
const char* mqtt_server = "broker.hivemq.com";

WiFiClient espClient;
PubSubClient client(espClient);

// --- BMP ---
BMP280 bmp;

// --- TIMERY ---
unsigned long lastMeasure = 0;
int failCount = 0;

// --- WIFI CONNECT ---
void connectWiFi() {
  Serial.print("Connecting WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi OK");
}

// --- MQTT CONNECT ---
void reconnectMQTT() {
  while (!client.connected()) {
    Serial.print("Connecting MQTT...");

    if (client.connect("AniaBania")) {
      Serial.println("OK");
    } else {
      Serial.print("ERROR: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// --- SETUP ---
void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);

  if (!bmp.begin()) {
    Serial.println("BMP ERROR!");
  } else {
    Serial.println("BMP OK");
  }

  connectWiFi();

  client.setServer(mqtt_server, 1883);
}

// --- LOOP ---
void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }

  if (!client.connected()) {
    reconnectMQTT();
  }

  client.loop();

  // co 5 sekund
  if (millis() - lastMeasure > 5000) {
    lastMeasure = millis();

    bool pingOK = Ping.ping("www.google.com");

    double t_local = 0.0;
    double p_local = 0.0;
    bmp.getTemperatureAndPressure(t_local, p_local);

    // --- MQTT STATUS ---
    if (pingOK) {
      Serial.println("PING OK");
      client.publish("mrar/mrar/ania", "OK");
      failCount = 0;

      // --- HTTP ---
      HTTPClient http;
      http.begin("https://api.open-meteo.com/v1/forecast?latitude=50.0614&longitude=19.9366&current=temperature_2m");

      int httpCode = http.GET();

      if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();

        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
          double t_api = doc["current"]["temperature_2m"];

          char msg_api[50];
          snprintf(msg_api, 50, "%.2f", t_api);

          client.publish("mrar/mrar/ania/temp_api", msg_api);

          Serial.print("API temp: ");
          Serial.println(t_api);
        } else {
          Serial.println("JSON ERROR");
        }
      } else {
        Serial.println("HTTP ERROR");
      }

      http.end();

    } else {
      Serial.println("PING FAIL");
      client.publish("mrar/mrar/ania", "FAIL");
      failCount++;
    }

    // --- MQTT LOCAL TEMP ---
    char msg_local[50];
    snprintf(msg_local, 50, "%.2f", t_local);

    client.publish("mrar/mrar/ania/temp_local", msg_local);

    Serial.print("Local temp: ");
    Serial.println(t_local);

    // --- ALERT ---
    if (failCount >= 3) {
      client.publish("mrar/mrar/ania", "NO INTERNET");
      Serial.println("ALERT: NO INTERNET");
    }
  }
}
