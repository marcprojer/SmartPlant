#include <WiFi.h>
#include <PubSubClient.h>
#include "config.h"

// Pin Definitionen
const int moistureSensor1 = 32; // Feuchtigkeitssensor 1 an D32
const int moistureSensor2 = 33; // Feuchtigkeitssensor 2 an D33

WiFiClient espClient;
PubSubClient client(espClient);

unsigned long lastPublishTime = 0;
const unsigned long publishInterval = 5000; // Publish alle 5 Sekunden

void setup() {
  // Serielle Kommunikation starten
  Serial.begin(115200);
  delay(100);

  Serial.println("\n\nSmartPlant - Starting...");
  
  // WiFi verbinden
  connectToWiFi();
  
  // MQTT Broker verbinden
  client.setServer(MQTT_BROKER, MQTT_PORT);
  client.setCallback(mqttCallback);
}

void loop() {
  // WiFi Verbindung prüfen und ggf. reconnect
  if (!client.connected()) {
    connectToMQTT();
  }
  
  client.loop();

  // Sensorwerte veröffentlichen
  if (millis() - lastPublishTime >= publishInterval) {
    publishSensorData();
    lastPublishTime = millis();
  }
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi connection failed!");
  }
}

void connectToMQTT() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    if (client.connect("ESP32_SmartPlant", MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("connected!");
      // Subscribe zu Topics falls benötigt
      // client.subscribe("smartplant/control");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void publishSensorData() {
  // Feuchtigkeitssensoren auslesen
  int moistureValue1 = analogRead(moistureSensor1);
  int moistureValue2 = analogRead(moistureSensor2);

  // MQTT Topics erstellen
  char topic1[50];
  char topic2[50];
  char payload1[10];
  char payload2[10];
  
  snprintf(topic1, sizeof(topic1), "%s/sensor/moisture1", MQTT_TOPIC);
  snprintf(topic2, sizeof(topic2), "%s/sensor/moisture2", MQTT_TOPIC);
  snprintf(payload1, sizeof(payload1), "%d", moistureValue1);
  snprintf(payload2, sizeof(payload2), "%d", moistureValue2);
  
  // Werte veröffentlichen
  client.publish(topic1, payload1);
  client.publish(topic2, payload2);
  
  // Debug-Ausgabe
  Serial.print("Sensor 1: ");
  Serial.print(moistureValue1);
  Serial.print(" | Sensor 2: ");
  Serial.println(moistureValue2);
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}