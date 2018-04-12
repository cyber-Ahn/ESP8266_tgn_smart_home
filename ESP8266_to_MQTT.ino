#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#define DHTTYPE DHT22

const char* ssid = "Your SSID";
const char* wifi_password = "Your password";
const char* mqtt_server = "Brocker IP";
const char* mqtt_topic = "esp";
const char* clientID = "NodeMCU Modul 1";
const int delay_in_sec = 1200;
const int DHTPin = D4;
const int ButtonPin = D3;
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
  if (client.connect(clientID)) {
    Serial.println("Connected to MQTT Broker!");
  }
  else {
    Serial.println("Connection to MQTT Broker failed...");
  }
}

void loop() {
  switchState = digitalRead(ButtonPin);
  digitalWrite(inLED,LOW);
  float h = dht2.readHumidity();
  float t = dht2.readTemperature();
  float f = dht2.readTemperature(true);
  if (switchState == HIGH){
    b1 = "on";
    }
   else {
    b1 = "off";
    }
  float hic = dht2.computeHeatIndex(t, h, false);
  dtostrf(hic, 6, 2, celsiusTemp);
  float hif = dht2.computeHeatIndex(f, h);
  dtostrf(hif, 6, 2, fahrenheitTemp);
  dtostrf(h, 6, 2, humidityTemp);
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
  char out[50] = "IP:";
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
  dtostrf(first_octet, 2, 0, ip_a);
  dtostrf(second_octet, 2, 0, ip_b);
  dtostrf(third_octet, 1, 0, ip_c);
  dtostrf(fourth_octet, 2, 0, ip_d);
  dtostrf(rssi_r, 2, 0, rssi_x);
  strcat(out,ip_a);
  strcat(out,".");
  strcat(out,ip_b);
  strcat(out,".");
  strcat(out,ip_c);
  strcat(out,".");
  strcat(out,ip_d);
  strcat(out,"Data|temp:");
  strcat(out,celsiusTemp);
  strcat(out,"|humi:");
  strcat(out,humidityTemp);
  strcat(out,"|b1:");
  strcat(out,b1);
  strcat(out,"|rssi:");
  strcat(out,prc_out);
  strcat(out,"$");
  strcat(out,rssi_x);
  Serial.println(out);
  if (client.publish(mqtt_topic, out)) {
    Serial.println("Message sent!");
    }
  digitalWrite(inLED,HIGH);
  delay(delay_in_sec*1000);
}
