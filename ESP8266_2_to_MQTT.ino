#include "ESPHelper.h"
#include <Metro.h>
#include <Wire.h>
#include "SSD1306Wire.h"
#include <ESP8266WiFi.h>
#include "DHT.h"

#define DHTTYPE DHT22
#define lightsensor A0

const char* ssid = "Matrix";
const char* wifi_password = "rhjk0096#Matrix";
const char* mqtt_server = "192.168.0.98";

const char* temp_topic = "tgn/esp_2/temp/sensor_1";
const char* hum_topic = "tgn/esp_2/temp/sensor_2";
const char* b1_topic = "tgn/esp_2/button/b1";
const char* wifi1_topic = "tgn/esp_2/wifi/pre";
const char* wifi2_topic = "tgn/esp_2/wifi/rssi";
const char* light_topic = "tgn/esp_2/analog/sensor_1";
const char* con_topic = "tgn/esp_2/connection/ip";
const char* update_topic = "tgn/esp_2/update";
const char* clientID = "NodeMCU_2 V1.1";
const int DHTPin = D4;
const int ButtonPin = D7;
const int DisplayPin = D8;
const int inLED = D0;
char* b1 = "off";
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];
int switchState = 0;
int switchStateB = 0;
int screen = 0;

netInfo homeNet = {  .mqttHost = mqtt_server,
          .mqttUser = "",
          .mqttPass = "",
          .mqttPort = 1883,
          .ssid = ssid, 
          .pass = wifi_password};

ESPHelper myESP(&homeNet);
Metro publishTimer = Metro(30000);
DHT dht2(DHTPin, DHTTYPE);
SSD1306Wire  display(0x3c, D3, D5);

#define WiFi_Logo_width 60
#define WiFi_Logo_height 36
const uint8_t WiFi_Logo_bits[] PROGMEM = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0xFF, 0x07, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xE0, 0xFF, 0x1F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF,
  0xFF, 0x03, 0x00, 0x00, 0x00, 0xFC, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00,
  0x00, 0xFF, 0xFF, 0xFF, 0x07, 0xC0, 0x83, 0x01, 0x80, 0xFF, 0xFF, 0xFF,
  0x01, 0x00, 0x07, 0x00, 0xC0, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x0C, 0x00,
  0xC0, 0xFF, 0xFF, 0x7C, 0x00, 0x60, 0x0C, 0x00, 0xC0, 0x31, 0x46, 0x7C,
  0xFC, 0x77, 0x08, 0x00, 0xE0, 0x23, 0xC6, 0x3C, 0xFC, 0x67, 0x18, 0x00,
  0xE0, 0x23, 0xE4, 0x3F, 0x1C, 0x00, 0x18, 0x00, 0xE0, 0x23, 0x60, 0x3C,
  0x1C, 0x70, 0x18, 0x00, 0xE0, 0x03, 0x60, 0x3C, 0x1C, 0x70, 0x18, 0x00,
  0xE0, 0x07, 0x60, 0x3C, 0xFC, 0x73, 0x18, 0x00, 0xE0, 0x87, 0x70, 0x3C,
  0xFC, 0x73, 0x18, 0x00, 0xE0, 0x87, 0x70, 0x3C, 0x1C, 0x70, 0x18, 0x00,
  0xE0, 0x87, 0x70, 0x3C, 0x1C, 0x70, 0x18, 0x00, 0xE0, 0x8F, 0x71, 0x3C,
  0x1C, 0x70, 0x18, 0x00, 0xC0, 0xFF, 0xFF, 0x3F, 0x00, 0x00, 0x08, 0x00,
  0xC0, 0xFF, 0xFF, 0x1F, 0x00, 0x00, 0x0C, 0x00, 0x80, 0xFF, 0xFF, 0x1F,
  0x00, 0x00, 0x06, 0x00, 0x80, 0xFF, 0xFF, 0x0F, 0x00, 0x00, 0x07, 0x00,
  0x00, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0x01, 0x00, 0x00, 0xF8, 0xFF, 0xFF,
  0xFF, 0x7F, 0x00, 0x00, 0x00, 0x00, 0xFE, 0xFF, 0xFF, 0x01, 0x00, 0x00,
  0x00, 0x00, 0xFC, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0xF8, 0xFF,
  0x7F, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0xFF, 0x1F, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x80, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  };

#define tgn_width 128
#define tgn_height 64
const uint8_t tgn_bits[] PROGMEM = {
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x00, 0xFF, 0x01, 0xF8, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0x00, 0x00, 0x80, 0x7F, 0x00, 
  0xC0, 0x07, 0xFC, 0xFF, 0x87, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0xFC, 
  0x80, 0x80, 0x1F, 0xF8, 0x80, 0x01, 0xF8, 0xFF, 0xC3, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0x3F, 0xFE, 0xF0, 0xFF, 0x0F, 0xFF, 0x83, 0x1F, 0xF8, 0xFF, 
  0xC3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xE0, 0xFF, 0x87, 0xFF, 
  0x8F, 0x3F, 0xF0, 0xFF, 0xE3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 
  0xF1, 0xFF, 0xC3, 0xFF, 0x1F, 0x3F, 0xF0, 0xFF, 0xE3, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0x3F, 0xFF, 0xE0, 0xFF, 0xE1, 0xFF, 0x9F, 0x3F, 0xE0, 0xFF, 
  0xE1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x3F, 0xFF, 0xF0, 0xFF, 0xF1, 0xFF, 
  0x9F, 0x3F, 0xC1, 0xFF, 0xE3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xBF, 0xFF, 
  0xF1, 0xFF, 0xF0, 0xFF, 0x9F, 0x3F, 0xC1, 0xFF, 0xE3, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xF8, 0xFF, 0xBF, 0x3F, 0x83, 0xFF, 
  0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xE0, 0x7F, 0xF8, 0xFF, 
  0xFF, 0x3F, 0x87, 0xFF, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xF0, 0x7F, 0xF8, 0xFF, 0xFF, 0x3F, 0x07, 0xFF, 0xF1, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xF1, 0x3F, 0xF8, 0xFF, 0xFF, 0x3F, 0x0F, 0xFE, 
  0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x3F, 0xFC, 0xFF, 
  0xFF, 0x3F, 0x0F, 0xFE, 0xF3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xF0, 0x3F, 0xFC, 0xFF, 0xFF, 0x3F, 0x1F, 0xFC, 0xF1, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x3F, 0xFC, 0xFF, 0xFF, 0x3F, 0x3F, 0xFC, 
  0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x3F, 0xFC, 0xFF, 
  0xFF, 0x3F, 0x3F, 0xF8, 0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xF0, 0x3F, 0xFC, 0xFF, 0xFF, 0x1F, 0x7F, 0xF0, 0xF9, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x1F, 0xFC, 0xFF, 0xFF, 0x3F, 0xFF, 0xF0, 
  0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x3F, 0xFC, 0xFF, 
  0xFF, 0x1F, 0xFF, 0xE0, 0xF3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xF0, 0x3F, 0xFC, 0x1F, 0x80, 0x3F, 0xFF, 0xC1, 0xF1, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x3F, 0xF8, 0x1F, 0x80, 0x1F, 0xFF, 0xC1, 
  0xF1, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x3F, 0xF8, 0xFF, 
  0xC3, 0x1F, 0xFF, 0x83, 0xFB, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xF0, 0x3F, 0xF8, 0xFF, 0x83, 0x9F, 0xFF, 0x07, 0xF1, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x3F, 0xF8, 0xFF, 0x83, 0x1F, 0xFF, 0x07, 
  0xF3, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0x7F, 0xF0, 0xFF, 
  0x83, 0x1F, 0xFF, 0x0F, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xF0, 0x7F, 0xF0, 0xFF, 0x87, 0x8F, 0xFF, 0x1F, 0xF0, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xE0, 0xFF, 0xC3, 0x0F, 0xFF, 0x1F, 
  0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0xC1, 0xFF, 
  0x87, 0x8F, 0xFF, 0x3F, 0xF0, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xF8, 0xFF, 0x81, 0xFF, 0xC3, 0x8F, 0xFF, 0x7F, 0xF8, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0x03, 0xFF, 0xC3, 0x87, 0xFF, 0x7F, 
  0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0, 0xFF, 0x0F, 0xF8, 
  0xC0, 0x87, 0xFF, 0xFF, 0xF8, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xF8, 0xFF, 0x1F, 0x00, 0xC0, 0xC7, 0xFF, 0xFF, 0xF9, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xF8, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 
  0xFF, 0xFF, 0xFF, 0xFF, };

void setup() {
  display.init();
  display.flipScreenVertically();
  display.setContrast(100);
  display.drawXbm(34, 14, WiFi_Logo_width, WiFi_Logo_height, WiFi_Logo_bits);
  display.display();
  delay(5000);
  display.clear();
  display.drawXbm(0, 0, tgn_width, tgn_height, tgn_bits);
  display.display();
  delay(5000);
  display.clear();
  pinMode(inLED, OUTPUT);
  pinMode(ButtonPin, INPUT);
  Serial.begin(9600);
  delay(10);
  dht2.begin();
  Serial.println();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Connecting to ");
  display.drawString(0, 16, ssid);
  display.drawString(0, 32, "Brocker IP:");
  display.drawString(0, 48, mqtt_server);
  display.display();
  delay(5000);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  myESP.OTA_enable();
  myESP.OTA_setPassword("esp2");
  myESP.OTA_setHostnameWithVersion(clientID);
  myESP.addSubscription(update_topic);
  myESP.setMQTTCallback(callback);
  myESP.begin();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
}

void loop() {
  myESP.loop();
  switchState = digitalRead(ButtonPin);
  switchStateB = digitalRead(DisplayPin);
  float h = dht2.readHumidity();
  float t = dht2.readTemperature();
  float f = dht2.readTemperature(true);
  int val = analogRead(lightsensor);
  if (switchStateB == HIGH){
    screen = 0;
    }
  if (switchState == HIGH){
    b1 = "on";
    }
  else {
    b1 = "off";
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
  if(publishTimer.check()){
    digitalWrite(inLED,LOW);
    display.clear();
    display.display();
    if (screen == 0) {
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(Dialog_bold_10);
      display.setContrast(255);
      display.drawString(0, 0, "WIFI:           %");
      display.drawString(31, 0, prc_out);
      display.drawString(0, 12, "TEMP:          °C");
      display.drawString(31, 12, celsiusTemp);
      display.drawString(0, 24, "HUMI:           %");
      display.drawString(31, 24, humidityTemp);
      display.drawString(0, 36, "B1:");
      display.drawString(31, 36, b1);
      display.drawString(0, 48, "LIGH:");
      display.drawString(31, 48, lis);
      display.display();
      screen = 1;
    }
    else if (screen == 1) {
      display.drawXbm(0, 0, tgn_width, tgn_height, tgn_bits);
      display.display();
      screen = 2;
    }
    else if (screen == 2) {
      display.clear();
    }
    delay(5000);
    myESP.publish(temp_topic, celsiusTemp, true);
    Serial.println(celsiusTemp);
    myESP.publish(hum_topic, humidityTemp, true);
    Serial.println(humidityTemp);
    myESP.publish(b1_topic, b1, true);
    Serial.println(b1);
    myESP.publish(wifi1_topic, prc_out, true);
    Serial.println(prc_out);
    myESP.publish(wifi2_topic, rssi_x, true);
    Serial.println(rssi_x);
    myESP.publish(light_topic, lis, true);
    Serial.println(lis);
    myESP.publish(con_topic, ip_out, true);
    Serial.println(ip_out);
    digitalWrite(inLED,HIGH);
    Serial.println("---------------------------------------------------------");
  }
  yield();
}
void callback(char* topic, uint8_t* payload, unsigned int length) {
  //put mqtt callback code here
}
