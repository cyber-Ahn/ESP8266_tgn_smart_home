#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define PIN D1
const int inLED = D0;
int inc_d = 0;
int inc_e = 0;

const char* ssid = "your ssid";
const char* wifi_password = "your wifi password";
const char* mqtt_server = "your broker ip";

char* color_topic = "tgn/esp_3/neopixel/color";
char* br_topic = "tgn/esp_3/neopixel/brightness";
char* mode_topic = "tgn/esp_3/neopixel/mode";
char* set_topic = "tgn/esp_3/neopixel/setneo"; //ledNum_color_brightness(1_255.0.0.255_50)
 
WiFiClient espClient;
PubSubClient client(espClient);
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, PIN, NEO_GRB + NEO_KHZ800);
long lastMsg = 0;
char msg[50];
int value = 0;
 
void setup() {
  strip.begin();
  strip.setBrightness(10);
  strip.show();
  pinMode(inLED, OUTPUT);
  Serial.begin(9600);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
}
 
void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, wifi_password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.macAddress());
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

void set_led(String data) {                                                                                //if data not "nothing"
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

  strip.setPixelColor(num, strip.Color(r, g, b));
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

void callback(char* topic, byte* payload, unsigned int length) {
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
  }
  if(strcmp(topic, set_topic) == 0) {
    set_led(data);
  }
  if(strcmp(topic, mode_topic) == 0) {
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
}
 
void reconnect() {
  while (!client.connected()) {
    Serial.println("Reconnecting MQTT...");
    if (!client.connect("ESP8266Client")) {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" retrying in 5 seconds");
      delay(5000);
    }
  }
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
  client.subscribe(mode_topic);
  client.subscribe(color_topic);
  client.subscribe(br_topic);
  client.subscribe(set_topic);
  Serial.println("MQTT Connected...");
}
 
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}
