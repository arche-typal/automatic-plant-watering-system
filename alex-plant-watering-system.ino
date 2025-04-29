#include <WiFi.h>
#include <PubSubClient.h>
#include <Secrets.h>

//WiFi credentials
const char* ssid = Secrets::wifiSSID();
const char* password = Secrets::wifiPass();

//MQTT Broker IP and port
const char* mqtt_server = Secrets::mqttServer();
const int mqtt_port = Secrets::mqttPort();

WiFiClient espClient;
PubSubClient client(espClient);

uint32_t count = 0;
uint32_t mqtt_count = 0;

void setup() {
  Serial.begin(115200);  //Initialize serial communication at 115200 baud rate
  Serial.println("ESP32 is responsive!");  // Print a test message

  //Code for connecting to wi-fi
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");

  //Code for connecting to MQTT
  client.setServer(mqtt_server, mqtt_port);

  Serial.println("End of set-up."); 
}

void loop() {
  Serial.printf("Count: %d\n", count);
  count++;
  Serial.printf("ESP32 ip address: %s\n", WiFi.localIP());
  Serial.printf("Connected to SSID: %s\n", WiFi.SSID());
  Serial.printf("Signal strength (RSSI):  %d dBm\n", WiFi.RSSI()); 

  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  //Publish a fake sensor value every 2 seconds
  static unsigned long lastTime = 0;
  if (millis() - lastTime > 2000) {
    lastTime = millis();
    bool success = false;
    if (count % 2 == 0) {        
      success = client.publish("plant/sensori", "{\"soil\":45.0, \"moisture\":67.2}");
    } else {        
      success = client.publish("plant/sensori", "{\"soil\":46.0, \"moisture\":68.2}");
    }
    if (success) {
      Serial.println("MQTT publish succeeded");
      if (count % 2 == 0) {
        Serial.println("Published soil moisture: 45.0");
      } else {
        Serial.println("Published soil moisture: 46.0");
      }
    } else {
      Serial.println("MQTT publish FAILED");
    }
  }

  delay(3000);
}

void reconnect() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    Serial.printf("MQTT Count: %d\n", mqtt_count);
    mqtt_count++;
    if (client.connect("ESP32Client", Secrets::mqttUser(), Secrets::mqttPass())) {
      Serial.println("Connected to MQTT Broker");
    } else {
      Serial.print("Failed to connect. State= ");
      Serial.println(client.state());
      delay(5000);
    }
  }
}
