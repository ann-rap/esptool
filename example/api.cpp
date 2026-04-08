#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define WEATHER_URL "https://api.open-meteo.com/v1/forecast?latitude=50.0614&longitude=19.9366&current=temperature_2m"

WiFiMulti wifiMulti;
HTTPClient http;

void setupWiFi(const char* ssid, const char* password) {
  wifiMulti.addAP(ssid, password);
  Serial.println("[WIFI] Skonfigurowano wifi");
}

String fetchJsonFromUrl(const char* url) {
  if (wifiMulti.run() != WL_CONNECTED) {
    Serial.println("[WIFI] Brak połączenia.");
    return "";
  }

  HTTPClient http;
  String payload = "";
  http.begin(url);

  int httpCode = http.GET();

  if (httpCode > 0) {
    if (httpCode == HTTP_CODE_OK) {
      payload = http.getString();
    } else {
      Serial.printf("Błąd, kod odpowiedzi: %d\n", httpCode);
    }
  } else {
    Serial.printf(http.errorToString(httpCode).c_str());
  }

  http.end();
  return payload;
}

float extractTemperature(String jsonPayload) {
  if (jsonPayload.length() == 0) {
    return -999.0; 
  }

  JsonDocument doc; 
  DeserializationError error = deserializeJson(doc, jsonPayload);

  if (!error) {
    float temperature = doc["current"]["temperature_2m"];
    return temperature;
  } else {
    Serial.println(error.c_str());
    return -999.0;
  }
}


void setup() {
  Serial.begin(115200);

  setupWiFi("PapaNET", "12345678!");
}

void loop() {
  String jsonData = fetchJsonFromUrl(WEATHER_URL);

  float currentTemp = extractTemperature(jsonData);

  if (currentTemp != -999.0) {
    Serial.printf("temperatura to: %.2f °C\n", currentTemp);
  } else {
    Serial.println("Nie udało się uzyskać prawidłowej temperatury.");
  }

  delay(5000);
}
