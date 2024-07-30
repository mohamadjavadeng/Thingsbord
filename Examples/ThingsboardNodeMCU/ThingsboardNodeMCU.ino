#include <DHT.h>
#include <ESP8266WiFi.h>  
#include <ThingsBoard.h>
#include <Arduino_MQTT_Client.h>

#define DHTPIN D5
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);


#define WIFI_AP "POCO X3"
#define WIFI_PASS "sss22222"

#define TB_SERVER "thingsboard.cloud"
#define TOKEN "XhkzWceHBKv0ODxPxAD2"

constexpr uint16_t MAX_MESSAGE_SIZE = 256U;

WiFiClient espClient;
Arduino_MQTT_Client mqttClient(espClient);
ThingsBoard tb(mqttClient, MAX_MESSAGE_SIZE);


void connectToWiFi() {
  Serial.println("Connecting to WiFi...");
  int attempts = 0;
  
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    WiFi.begin(WIFI_AP, WIFI_PASS, 6);
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to WiFi.");
  } else {
    Serial.println("\nConnected to WiFi");
  }
}

void connectToThingsBoard() {
  if (!tb.connected()) {
    Serial.println("Connecting to ThingsBoard server");
    
    if (!tb.connect(TB_SERVER, TOKEN)) {
      Serial.println("Failed to connect to ThingsBoard");
    } else {
      Serial.println("Connected to ThingsBoard");
    }
  }
}
void sendDataToThingsBoard(float temp, int hum) {
  String jsonData = "{\"temperature\":" + String(temp) + ", \"humidity\":" + String(hum) + "}";
  tb.sendTelemetryJson(jsonData.c_str());
  Serial.println("Data sent");
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Hello, ESP32!");
  pinMode(35, OUTPUT);
  dht.begin();
  connectToWiFi();
  connectToThingsBoard();
  
}

void loop() {
  float hum = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float temp = dht.readTemperature();
  // float temp = 2.036;
  // int hum = 105;
  Serial.println(temp);
  Serial.println(hum);
  if (!tb.connected()) {
    connectToThingsBoard();
  }
  sendDataToThingsBoard(temp, hum);

  delay(3000);

  tb.loop();
}
