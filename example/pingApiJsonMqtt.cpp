#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP32Ping.h>

const char* weather_url = "https://api.open-meteo.com/v1/forecast?latitude=50.0614&longitude=19.9366&current=temperature_2m";
const char* ssid = "ssid";
const char* password = "password";
const char* mqtt_server = "broker.hivemq.com";

const IPAddress remote_ip(8, 8, 8, 8);

WiFiClient wifiClient;
HTTPClient http;
PubSubClient client(wifiClient);

struct SensorData {
  const char* key;
  float value;
};

void setupWiFi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n[WIFI] Połączono!");
  Serial.print("[WIFI] Adres IP: ");
  Serial.println(WiFi.localIP());

  Serial.print("Pingowanie");
  Serial.println(remote_ip);

  if(Ping.ping(remote_ip)) {
    Serial.println("Sukces!!");
  } else {
    Serial.println("Blad :(");
  }
}

String fetchJsonFromUrl(const char* url) {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("[WIFI] Brak połączenia, próba ponownego łączenia...");
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

String packToJson(SensorData dataArray[], int arraySize) {
  JsonDocument doc;

  for (int i = 0; i < arraySize; i++) {
    doc[dataArray[i].key] = dataArray[i].value;
  }

  String output;
  serializeJson(doc, output);
  return output;
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

bool mqttPublish(const char* topic, const char* payload, bool retained = false) {
    if (!client.connected())
      return false;
    return client.publish(topic, payload, retained);
}

void reconnect() {
  while (!client.connected()) {
    String clientId = "client_id";
    
    if (client.connect(clientId.c_str())) {
      Serial.println("połączono");
    } else {
      Serial.print("błąd, rc=");
      Serial.print(client.state());
      delay(1000);
    }
  }
}


void setup() {
  Serial.begin(115200);

  setupWiFi();
  client.setServer(mqtt_server, 1883);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  String jsonData = fetchJsonFromUrl(weather_url);
  float currentTemp = extractTemperature(jsonData);

  SensorData measurements[] = {
    {"api", currentTemp},
    {"sensor", 21.37f} //tutaj podmianka wiadomka
  };
  
  String jsonPayload = packToJson(measurements, 2);
  mqttPublish("mrar/mrar/pogoda/temp", jsonPayload.c_str());

  delay(5000);
}
