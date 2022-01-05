#include <Adafruit_NeoPixel.h>
#include "ESPHelper.h"
#include <Metro.h>

#define PIN D1

const char* ssid = "name";
const char* wifi_password = "pwd";
const char* mqtt_server = "192.168.0.98";

const char* color_topic = "tgn/esp_4/color";
const char* br_topic = "tgn/esp_4/brightness";
const char* time_topic = "tgn/system/time";
const char* con_topic = "tgn/esp_4/ip";
const char* update_topic = "tgn/esp_4/update";
const char* clientID = "NodeMCU_3 V1.7";
const int inLED = D0;
String c_d = "";
String b_d = "10";
int c_r_b = 248; int c_g_b = 0; int c_b_b = 255;
int c_r_d = 255; int c_g_d = 0; int c_b_d = 0;
int c_r_h = 0; int c_g_h = 255; int c_b_h = 0;
int c_r_m = 255; int c_g_m = 255; int c_b_m = 0;
int hour = 5;
int mi = 43;
String cl = "on";

netInfo homeNet = {  .mqttHost = mqtt_server,
          .mqttUser = "",
          .mqttPass = "", 
          .mqttPort = 1883,
          .ssid = ssid, 
          .pass = wifi_password};

ESPHelper myESP(&homeNet);
Metro publishTimer = Metro(30000);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);
 
void setup() {
  strip.begin();
  strip.setBrightness(10);
  strip.show();
  pinMode(inLED, OUTPUT);
  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  myESP.OTA_enable();
  myESP.OTA_setPassword("esp4");
  myESP.OTA_setHostnameWithVersion(clientID);
  myESP.addSubscription(color_topic);
  myESP.addSubscription(br_topic);
  myESP.addSubscription(time_topic);
  myESP.addSubscription(update_topic);
  myESP.setMQTTCallback(callback);
  myESP.begin();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  colorWipe(strip.Color(255, 255, 255), 15);
  strip.setBrightness(50);
  strip.show();
  delay(500);
  colorWipe(strip.Color(255, 0, 0), 10);
  delay(500);
  colorWipe(strip.Color(0, 255, 0), 10);
  delay(500);
  colorWipe(strip.Color(0, 0, 255), 10);
  delay(500);
  ini_layout(0);
}

void ini_layout(uint8_t mod) {
  colorWipe(strip.Color(0, 0, 0), 0);
  strip.setBrightness(atoi(b_d.c_str()));
  strip.show();
  strip.setPixelColor(5, strip.Color(c_r_b, c_g_b, c_b_b));
  strip.show();
  strip.setPixelColor(10, strip.Color(c_r_b, c_g_b, c_b_b));
  strip.show();
  strip.setPixelColor(20, strip.Color(c_r_b, c_g_b, c_b_b));
  strip.show();
  strip.setPixelColor(25, strip.Color(c_r_b, c_g_b, c_b_b));
  strip.show();
  strip.setPixelColor(35, strip.Color(c_r_b, c_g_b, c_b_b));
  strip.show();
  strip.setPixelColor(40, strip.Color(c_r_b, c_g_b, c_b_b));
  strip.show();
  strip.setPixelColor(50, strip.Color(c_r_b, c_g_b, c_b_b));
  strip.show();
  strip.setPixelColor(55, strip.Color(c_r_b, c_g_b, c_b_b));
  strip.show();
  strip.setPixelColor(0, strip.Color(c_r_d, c_g_d, c_b_d));
  strip.show();
  strip.setPixelColor(15, strip.Color(c_r_d, c_g_d, c_b_d));
  strip.show();
  strip.setPixelColor(30, strip.Color(c_r_d, c_g_d, c_b_d));
  strip.show();
  strip.setPixelColor(45, strip.Color(c_r_d, c_g_d, c_b_d));
  strip.show();
  set_time(hour, mi);
}

void set_time(uint8_t h, uint8_t m) {
  h = h * 5;
  strip.setPixelColor(h, strip.Color(c_r_h, c_g_h, c_b_h));
  strip.show();
  strip.setPixelColor(m, strip.Color(c_r_m, c_g_m, c_b_m));
  strip.show();
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
  digitalWrite(inLED,HIGH); 
  char msg[length+1];
  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
  }
  msg[length] = '\0';
  String data = msg;
  if(strcmp(topic, color_topic) == 0) {
    int r = atoi(getValue(data, '.', 0).c_str());
    int g = atoi(getValue(data, '.', 1).c_str());
    int b = atoi(getValue(data, '.', 2).c_str());
    int c = atoi(getValue(data, '.', 3).c_str());
    Serial.print(r);
    Serial.println();
    Serial.print(g);
    Serial.println();
    Serial.print(b);
    Serial.println();
    Serial.print(c);
    Serial.println();
    if(c == 255){
      cl = "on";
    }
    if(c == 1) {
      cl = "off";
      colorWipe(strip.Color(r, g, b), c);
    }
    
    
    c_d = data;
  }
  if(strcmp(topic, br_topic) == 0) {
    int br = atoi(data.c_str());
    strip.setBrightness(br);
    strip.show();
    Serial.print("Set Brightness: ");
    Serial.print(br);
    Serial.println();
    b_d = data;
  }
  if(strcmp(topic, time_topic) == 0) {
    String cach = getValue(data, ' ', 3).c_str();
    Serial.print(cach);
    Serial.println();
    Serial.print(cl);
    Serial.println();
    hour = atoi(getValue(cach, ':', 0).c_str());
    mi = atoi(getValue(cach, ':', 1).c_str());
    if(hour > 12) {
      hour = hour - 12;
    }
    if(cl == "on") {
      ini_layout(0);
    }
  }
  delay(1000);
  digitalWrite(inLED,LOW);
}
 
void loop() {
  myESP.loop();
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
    delay(5000);
    myESP.publish(con_topic, ip_out, true);
    Serial.println(ip_out);
  }
  yield();
}
