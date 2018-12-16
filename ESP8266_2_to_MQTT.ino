
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "DHT.h"
#define DHTTYPE DHT22
#define lightsensor A0

const char* ssid = "your ssid";
const char* wifi_password = "your wifi password";
const char* mqtt_server = "your broker ip";

const char* temp_topic = "tgn/esp_2/temp/sensor_1";
const char* hum_topic = "tgn/esp_2/temp/sensor_2";
const char* b1_topic = "tgn/esp_2/button/b1";
const char* wifi1_topic = "tgn/esp_2/wifi/pre";
const char* wifi2_topic = "tgn/esp_2/wifi/rssi";
const char* light_topic = "tgn/esp_2/analog/sensor_1";
const char* con_topic = "tgn/esp_2/connection/ip";
const char* clientID = "NodeMCU Modul 2";

const int DHTPin = D4;
const int ButtonPin = D7;
const int inLED = D0;
char* b1 = "off";
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];
int switchState = 0;

WiFiClient wifiClient;
PubSubClient client(mqtt_server, 1883, wifiClient);
DHT dht2(DHTPin, DHTTYPE);

void setup() {
  pinMode(inLED, OUTPUT);
  pinMode(ButtonPin, INPUT);
  Serial.begin(9600);
  delay(10);
  dht2.begin();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
}

void reconnect(){
  if (client.connect(clientID)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
    delay(5000);
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  switchState = digitalRead(ButtonPin);
  digitalWrite(inLED,LOW);
  float h = dht2.readHumidity();
  float t = dht2.readTemperature();
  float f = dht2.readTemperature(true);
  int val = analogRead(lightsensor);
  if (switchState == HIGH){
    b1 = "off";
    }
  else {
    b1 = "on";
    }
  float hic = dht2.computeHeatIndex(t, h, false);
  dtostrf(hic, 4, 1, celsiusTemp);
  float hif = dht2.computeHeatIndex(f, h);
  dtostrf(hif, 6, 1, fahrenheitTemp);
  dtostrf(h, 4, 1, humidityTemp);
  long rssi = WiFi.RSSI();
  long ch = 0 - rssi;
  char* prc_out = "0";
  if (ch <= 95 and ch > 89) {
    prc_out = "0";
    }
  if (ch <= 89 and ch > 83) {
    prc_out = "10";
    }
  if (ch <= 83 and ch > 77) {
    prc_out = "20";
    }
  if (ch <= 77 and ch > 71) {
    prc_out = "30";
    }
  if (ch <= 71 and ch > 65) {
    prc_out = "40";
    }
  if (ch <= 65 and ch > 59) {
    prc_out = "50";
    }
  if (ch <= 59 and ch > 53) {
    prc_out = "60";
    }
  if (ch <= 53 and ch > 47) {
    prc_out = "70";
    }
  if (ch <= 47 and ch > 41) {
    prc_out = "80";
    }
  if (ch <= 41 and ch > 35) {
    prc_out = "90";
    }
  if (ch <= 35 and ch > 0) {
    prc_out = "100";
    }
  char ip_out[50] = "";
  IPAddress ip_r = WiFi.localIP();
  int  rssi_r = WiFi.RSSI();
  byte first_octet = ip_r[0];
  byte second_octet = ip_r[1];
  byte third_octet = ip_r[2];
  byte fourth_octet = ip_r[3];
  static char ip_a[7];
  static char ip_b[7];
  static char ip_c[7];
  static char ip_d[7];
  static char rssi_x[7];
  static char lis[7];
  dtostrf(first_octet, 2, 0, ip_a);
  dtostrf(second_octet, 2, 0, ip_b);
  dtostrf(third_octet, 1, 0, ip_c);
  dtostrf(fourth_octet, 2, 0, ip_d);
  dtostrf(rssi_r, 2, 0, rssi_x);
  dtostrf(val, 2, 0, lis);
  strcat(ip_out,ip_a);
  strcat(ip_out,".");
  strcat(ip_out,ip_b);
  strcat(ip_out,".");
  strcat(ip_out,ip_c);
  strcat(ip_out,".");
  strcat(ip_out,ip_d);
  if (client.publish(temp_topic, (uint8_t*)celsiusTemp, strlen(celsiusTemp), true)) {
    Serial.println(celsiusTemp);
    }
  if (client.publish(hum_topic, (uint8_t*)humidityTemp, strlen(humidityTemp), true)) {
    Serial.println(humidityTemp);
    }
  if (client.publish(b1_topic, (uint8_t*)b1, strlen(b1), true)) {
    Serial.println(b1);
    }
  if (client.publish(wifi1_topic, (uint8_t*)prc_out, strlen(prc_out), true)) {
    Serial.println(prc_out);
    }
  if (client.publish(wifi2_topic, (uint8_t*)rssi_x, strlen(rssi_x), true)) {
    Serial.println(rssi_x);
    }
  if (client.publish(light_topic, (uint8_t*)lis, strlen(lis), true)) {
    Serial.println(lis);
    }
  if (client.publish(con_topic, (uint8_t*)ip_out, strlen(ip_out), true)) {
    Serial.println(ip_out);
    }
  digitalWrite(inLED,HIGH);
  Serial.println("---------------------------------------------------------");
  delay(610000);
}
