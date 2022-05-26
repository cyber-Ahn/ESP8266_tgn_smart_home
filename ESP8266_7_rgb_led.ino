#include <Adafruit_NeoPixel.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define PIN D1

const char* ssid = "name";
const char* wifi_password = "pwd";
const char* mqtt_server = "mqtt ip";

const char* reset_topic = "tgn/system/reboot/esp4";
const char* color_topic = "tgn/esp_3/neopixel/color";
const char* br_topic = "tgn/esp_3/neopixel/brightness";
const char* set_topic = "tgn/esp_3/neopixel/setneo";
const char* con_topic = "tgn/esp_4/ip";
String clientID = "NodeMCU_4 V1.7";
const int inLED = D0;
long lastMsg = 0;
char msg[50];
int value = 0;
String c_d = "";
String b_d = "";
String m_d = "";
int z = 0;

WiFiClient espClient;
PubSubClient client(espClient);
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
  setup_wifi();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  strip.setBrightness(50);
  strip.show();
  delay(500);
  colorWipe(strip.Color(255, 0, 0), 10);
  delay(500);
  colorWipe(strip.Color(0, 255, 0), 10);
  delay(500);
  colorWipe(strip.Color(0, 0, 255), 10);
  delay(500);
}

void setup_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(100);
  }
  Serial.println(WiFi.localIP());
}

void reconnect(){
  while (!client.connected()){
  Serial.println("Reconnecting");
  clientID += String(random(0xffff), HEX);
  if(!client.connect(clientID.c_str())){
    Serial.print("faild, rc=");
    Serial.print(client.state());
    Serial.print("retrying in 5 s");
    delay(5000);
    }
  }
  client.subscribe(color_topic);
  client.subscribe(reset_topic);
  client.subscribe(br_topic);
  client.subscribe(set_topic);
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
  char msg[length+1];
  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
  }
  msg[length] = '\0';
  String data = msg;
  Serial.println(data);
  if(strcmp(topic, color_topic) == 0) {
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
      client.publish(reset_topic, "0", true);
      ESP.restart();
    }
  }
  digitalWrite(inLED,HIGH);
}

void loop() {

  if (!client.connected()) {
        reconnect();
    }
  client.loop();
  
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
  Serial.println(ip_out);
  client.publish(con_topic, ip_out, true);
  delay(1000);
  z = z + 1;
  if (z == 3600){
    Serial.println("reboot");
    ESP.restart();
  }
}
