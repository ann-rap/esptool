// komenda python.exe -m esptool --chip esp32 --port COM7 --baud 115200 write-flash -z 0x1000 bootloader.bin 0x8000 partitions.bin 0x10000 firmware.bin
// utworzyć projekt, zmienić nazwę bez tego dziwnego znaczka, odpalić ponownie, flash odpalić w katalogu build
// komendy wsl --unregister , --install FedoraLinux-43, 


#include "BMP280.h"
#include "Wire.h"
#include "Arduino.h"
#include "PubSubClient.h"
#include "ArduinoJson.h"
#include "WiFi.h"
#include <HTTPClient.h>
#include <ESP32Ping.h>


#define P0 1013.25

 const char* ssid     = "...";
const char* password = "...";
 HTTPClient http;
  JsonDocument doc;
  JsonDocument finaldoc;
const char* mqtt_server = "broker.hivemq.com";
BMP280 bmp;
const IPAddress remote_ip(8,8,8,8);

WiFiClient espClient;
PubSubClient client(espClient);

#define MSG_BUFFER_SIZE	(50)
char msg[MSG_BUFFER_SIZE];

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup()
{
  Serial.begin(115200);
  if(!bmp.begin()){
    Serial.println("BMP init failed!");
    while(1);
  }
  else Serial.println("BMP init success!");
  
  bmp.setOversampling(4);
   
  Serial.println();
   WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
    delay(100);
    Serial.print(".");
  }
   http.begin("https://api.open-meteo.com/v1/forecast?latitude=50.0614&longitude=19.9366&current=temperature_2m"); 
  client.setServer(mqtt_server, 1883);

  if(Ping.ping(remote_ip)) {
    Serial.println("Success!!");
  } else {
    Serial.println("Error :(");
  }
}

void loop()
{
  double T,P;
  char result = bmp.startMeasurment();
  reconnect();
 
  if(result!=0){
    delay(result);
    result = bmp.getTemperatureAndPressure(T,P);
      if(result!=0)
      { }
      else {
        Serial.println("BMP Error.");
      }
  }
  else {
    Serial.println("BMP Error.");
  }

  if(WiFi.status() == WL_CONNECTED){
    http.GET();
    String json = http.getString();
    deserializeJson(doc,json);

    finaldoc["bmp_temp"] = String(T,4) + " degC";
    finaldoc["api_temp"] = String(doc["current"]["temperature_2m"].as<double>(), 6) + " degC";
    String payload;
    serializeJsonPretty(finaldoc, payload);
    Serial.println(payload);
    client.publish("PK/DK", payload.c_str());
  }
  else{
    Serial.println("Wifi Error");
  }

  delay(2000);
}
