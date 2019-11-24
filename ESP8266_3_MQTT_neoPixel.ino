#include <Adafruit_NeoPixel.h>
#include "ESPHelper.h"
#include <Metro.h>
#include <Wire.h>
#include "SSD1306Wire.h"

#define PIN D1

const char* ssid = "Matrix";
const char* wifi_password = "rhjk0096#Matrix";
const char* mqtt_server = "192.168.0.98";

char* reset_topic = "tgn/system/reboot/esp3";
char* color_topic = "tgn/esp_3/neopixel/color";
char* br_topic = "tgn/esp_3/neopixel/brightness";
char* mode_topic = "tgn/esp_3/neopixel/mode";
char* set_topic = "tgn/esp_3/neopixel/setneo";
const char* con_topic = "tgn/esp_3/connection/ip";
const char* update_topic = "tgn/esp_3/update";
const char* clientID = "NodeMCU_3 V1.6";
const int inLED = D0;
const int DisplayPin = D8;
int inc_d = 0;
int inc_e = 0;
int screen = 0;
long lastMsg = 0;
char msg[50];
int value = 0;
int switchStateB = 0;
String c_d = "";
String b_d = "";
String m_d = "";

netInfo homeNet = {  .mqttHost = mqtt_server,
          .mqttUser = "",
          .mqttPass = "", 
          .mqttPort = 1883,
          .ssid = ssid, 
          .pass = wifi_password};

ESPHelper myESP(&homeNet);
SSD1306Wire  display(0x3c, D3, D5);
Metro publishTimer = Metro(30000);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);

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
  display.display();
  strip.begin();
  strip.setBrightness(10);
  strip.show();
  pinMode(inLED, OUTPUT);
  pinMode(DisplayPin, INPUT);
  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  myESP.OTA_enable();
  myESP.OTA_setPassword("esp3");
  myESP.OTA_setHostnameWithVersion(clientID);
  myESP.addSubscription(color_topic);
  myESP.addSubscription(br_topic);
  myESP.addSubscription(mode_topic);
  myESP.addSubscription(set_topic);
  myESP.addSubscription(update_topic);
  myESP.addSubscription(reset_topic);
  myESP.setMQTTCallback(callback);
  myESP.begin();
  display.setFont(ArialMT_Plain_16);
  display.drawString(0, 0, "Connecting to ");
  display.drawString(0, 16, ssid);
  display.drawString(0, 32, "Brocker IP:");
  display.drawString(0, 48, mqtt_server);
  display.display();
  delay(5000);
  display.clear();
  display.display();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  set_led("0_255.0.0.255_50");
  delay(500);
  set_led("4_255.0.0.255_50");
  delay(500);
  set_led("8_255.0.0.255_50");
  delay(500);
  set_led("12_255.0.0.255_50");
  delay(500);
  set_led("1_0.255.0.255_50");
  delay(500);
  set_led("5_0.255.0.255_50");
  delay(500);
  set_led("9_0.255.0.255_50");
  delay(500);
  set_led("13_0.255.0.255_50");
  delay(500);
  set_led("2_0.0.255.255_50");
  delay(500);
  set_led("6_0.0.255.255_50");
  delay(500);
  set_led("10_0.0.255.255_50");
  delay(500);
  set_led("14_0.0.255.255_50");
  delay(500);
  set_led("3_255.0.255.255_50");
  delay(500);
  set_led("7_255.0.255.255_50");
  delay(500);
  set_led("11_255.0.255.255_50");
  delay(500);
  set_led("15_255.0.255.255_50");
  delay(500);
}

void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}

uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

void theaterChaseRainbow(uint8_t wait) {
  for (int j=0; j < 256; j++) {
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, Wheel( (i+j) % 255));
      }
      strip.show();
      delay(wait);
      for (uint16_t i=0; i < strip.numPixels(); i=i+3) {
        strip.setPixelColor(i+q, 0);
      }
    }
  }
}

void set_led(String data) {
  if (data != "nothing"){
    int num = atoi(getValue(data, '_', 0).c_str());
    String col = getValue(data, '_', 1);
    int br = atoi(getValue(data, '_', 2).c_str());
    int r = atoi(getValue(col, '.', 0).c_str());
    int g = atoi(getValue(col, '.', 1).c_str());
    int b = atoi(getValue(col, '.', 2).c_str());
    Serial.print("Set LED: ");
    Serial.print(num);
    Serial.println();
    strip.setBrightness(br);
    strip.show();
    Serial.print("Brightness: ");
    Serial.print(br);
    Serial.println();
    delay(50);
    strip.setPixelColor(num, strip.Color(r, g, b, br));
    strip.show();
    Serial.print("Color: ");
    Serial.print(r);
    Serial.print(", ");
    Serial.print(g);
    Serial.print(", ");
    Serial.print(b);
    Serial.println();
    delay(50);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  digitalWrite(inLED,LOW); 
  inc_d = 0;
  inc_e = 0;
  char msg[length+1];
  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
  }
  msg[length] = '\0';
  String data = msg;
  if(strcmp(topic, color_topic) == 0) {
    inc_d = 0;
    inc_e = 0;
    int r = atoi(getValue(data, '.', 0).c_str());
    int g = atoi(getValue(data, '.', 1).c_str());
    int b = atoi(getValue(data, '.', 2).c_str());
    colorWipe(strip.Color(r, g, b), 50);
    Serial.print("Set Color: ");
    Serial.print(r);
    Serial.print(", ");
    Serial.print(g);
    Serial.print(", ");
    Serial.print(b);
    Serial.println();
    c_d = data;
  }
  if(strcmp(topic, br_topic) == 0) {
    inc_d = 0;
    inc_e = 0;
    int br = atoi(data.c_str());
    strip.setBrightness(br);
    strip.show();
    Serial.print("Set Brightness: ");
    Serial.print(br);
    Serial.println();
    b_d = data;
  }
  if(strcmp(topic, set_topic) == 0) {
    set_led(data);
  }
  if(strcmp(topic, reset_topic) == 0) {
    if(strcmp(msg, "1") == 0){
      myESP.publish(reset_topic, "0", true);
      ESP.restart();
    }
  }
  if(strcmp(topic, mode_topic) == 0) {
    m_d = msg;
    if(strcmp(msg, "rainbow") == 0){
       inc_d = 1;
       inc_e = 0;
       rainbowCycle(20);
       Serial.print("Mode: Rainbow");
       Serial.println();
    }
    else if(strcmp(msg, "theater") == 0){
       inc_d = 0;
       inc_e = 1;
       theaterChaseRainbow(50);
       Serial.print("Mode: Theater");
       Serial.println();
    }
    else if(strcmp(msg, "normal") == 0){
       inc_d = 0;
       inc_e = 0;
       Serial.print("Mode: Normal");
       Serial.println();
    }
    else{
       inc_d = 0;
       inc_e = 0;
       Serial.print("Mode: no Mode set");
       Serial.println();
    }
  }
  if(inc_d == 1) {
    rainbowCycle(20);
  }
  if(inc_e == 1) {
    theaterChaseRainbow(50);
  }
  delay(1000);
  digitalWrite(inLED,HIGH);
}
 
void loop() {
  myESP.loop();
  switchStateB = digitalRead(DisplayPin);
  if (switchStateB == HIGH){
    screen = 0;
    }
  if(publishTimer.check()){
    char ip_out[50] = "";
    IPAddress ip_r = WiFi.localIP();
    byte first_octet = ip_r[0];
    byte second_octet = ip_r[1];
    byte third_octet = ip_r[2];
    byte fourth_octet = ip_r[3];
    static char ip_a[7];
    static char ip_b[7];
    static char ip_c[7];
    static char ip_d[7];
    dtostrf(first_octet, 2, 0, ip_a);
    dtostrf(second_octet, 2, 0, ip_b);
    dtostrf(third_octet, 1, 0, ip_c);
    dtostrf(fourth_octet, 2, 0, ip_d);
    strcat(ip_out,ip_a);
    strcat(ip_out,".");
    strcat(ip_out,ip_b);
    strcat(ip_out,".");
    strcat(ip_out,ip_c);
    strcat(ip_out,".");
    strcat(ip_out,ip_d);
    display.clear();
    display.display();
    delay(5000);
    myESP.publish(con_topic, ip_out, true);
    Serial.println(ip_out);
    if (screen == 0) {
      display.setTextAlignment(TEXT_ALIGN_LEFT);
      display.setFont(Dialog_bold_10);
      display.setContrast(255);
      display.drawString(0, 24, "Color:");
      display.drawString(32, 24, c_d);
      display.drawString(0, 12, "Brightness:");
      display.drawString(70, 12, b_d);
      display.drawString(0, 0, "Mode:");
      display.drawString(70, 0, m_d);
      display.drawString(0, 48, "IP:");
      display.drawString(20, 48, ip_out);
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
  }
  yield();
}
