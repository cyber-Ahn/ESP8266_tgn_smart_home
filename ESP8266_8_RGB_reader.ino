#include "TCS34725.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

char lu[20];
char ctemp[20];
char ccr[20];
char ccg[20];
char ccb[20];
char ccs[20];
char msg[50];
const int inLED = D0;
const int set_room = true;

const char* ssid = "name";
const char* wifi_password = "pwd";
const char* mqtt_server = "mqtt ip";

const char* color3_topic = "tgn/esp_3/neopixel/color";
const char* br_topic = "tgn/esp_3/neopixel/brightness";
const char* ctemp_topic = "tgn/esp_8/ctemp";
const char* color_topic = "tgn/esp_8/color";
const char* lux_topic = "tgn/esp_8/lux";
const char* con_topic = "tgn/esp_8/connection/ip";
const char* reset_topic = "tgn/system/reboot/esp4";
String clientID = "NodeMCU_8 V1.7";
int z = 0;

TCS34725 tcs;
WiFiClient espClient;
PubSubClient client(espClient);

void setup(void)
{
    Serial.begin(9600);
    pinMode(inLED, OUTPUT);
    Serial.print("Connecting to ");
    Serial.println(ssid);
    setup_wifi();
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    client.setServer(mqtt_server, 1883);
    client.setCallback(callback);
    Serial.println(WiFi.localIP());
    Serial.println(WiFi.macAddress());
    Wire.begin();
    if (!tcs.attach(Wire))
        Serial.println("ERROR: TCS34725 NOT FOUND !!!");
    tcs.integrationTime(700); // ms
    tcs.gain(TCS34725::Gain::X01);
    tcs.clearInterrupt();
    tcs.interrupt(true);
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
  client.subscribe(reset_topic);
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
  if(strcmp(topic, reset_topic) == 0) {
    if(strcmp(msg, "1") == 0){
      client.publish(reset_topic, "0", true);
      ESP.restart();
    }
  }
  digitalWrite(inLED,HIGH);
}

void loop(void)
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if (tcs.available())
  {
    static uint32_t prev_ms = millis();
    TCS34725::Color color = tcs.color();
    TCS34725::RawData raw = tcs.raw();
    dtostrf(tcs.lux(),-1, 0, lu);
    dtostrf(tcs.colorTemperature(),-1, 0, ctemp);
    dtostrf((raw.r / 256),-1, 0, ccr);
    dtostrf((raw.g / 256),-1, 0, ccg);
    dtostrf((raw.b / 256),-1, 0, ccb);
    strcpy (ccs,ccr);
    strcat (ccs,".");
    strcat (ccs,ccg);
    strcat (ccs,".");
    strcat (ccs,ccb);
    strcat (ccs,".");
    strcat (ccs,"255");
    Serial.print("Color Temp : "); Serial.println(ctemp);
    Serial.print("Lux        : "); Serial.println(lu);
    Serial.print("Raw R      : "); Serial.println(ccr);
    Serial.print("Raw G      : "); Serial.println(ccg);
    Serial.print("Raw B      : "); Serial.println(ccb);
    Serial.print("Colorcode  : "); Serial.println(ccs);
    prev_ms = millis();
  }
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
  client.publish(color_topic, ccs, true);
  client.publish(lux_topic, lu, true);
  client.publish(ctemp_topic, ctemp, true);
  if (set_room){
    Serial.println("Set RGB in Room");
    client.publish(br_topic, "150", true);
    client.publish(color3_topic, ccs, true);
  }
  delay(5000);
  z = z + 1;
  if (z == 720){
    Serial.println("reboot");
    ESP.restart();
  }
}
