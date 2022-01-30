#include "ESPHelper.h"
#include <Metro.h>

const char* ssid = "name";
const char* wifi_password = "pwd";
const char* mqtt_server = "mqtt ip";
String homecode = "10101";
String modul = "6";

const int relay_pin = D6;
const int led_pin = D7;
const int button_pin = D3;
const int inLED = D0;

char* data_topic = "tgn/sonoff/data";
char* res_topic = "tgn/system/reboot/sonoff";
const char* con_topic = "tgn/sonoff_1/connection/ip";
const char* update_topic = "tgn/sonoff_1/update";
String clientID = "sonoff_1 V0.2";
int switchStateB = 0;
int button_sw = 0;
int relay_stat = 0;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(relay_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(inLED, OUTPUT);
  pinMode(button_pin, INPUT);
  Serial.begin(9600);
  Serial.print("Connecting to ");
  Serial.println(ssid);
  setup_wifi();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
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
  client.subscribe(data_topic);
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

void callback(char* topic, byte* payload, unsigned int length) {
  char msg[length+1];
  for (int i = 0; i < length; i++) {
    msg[i] = (char)payload[i];
  }
  msg[length] = '\0';
  String data = msg;
  if(strcmp(topic, res_topic) == 0) {
    if(strcmp(msg, "1") == 0){
      client.publish(res_topic, "0", true);
      ESP.restart();
    }
  }
  if(strcmp(topic, data_topic) == 0) {
    Serial.print("New Data:");
    Serial.println(data);
    String homec = getValue(data, '-', 0);
    String cach = getValue(data, '-', 1);
    String mod = getValue(cach, '.', 0);
    String chan = getValue(cach, '.', 1);
    String sta = getValue(data, '-', 2);

    if(homec == homecode and mod == modul) {
      if(chan == "0" and sta == "1"){
        Serial.println("ON");
        digitalWrite(relay_pin,HIGH);
        digitalWrite(led_pin,HIGH);
        digitalWrite(inLED,LOW);
        relay_stat = 1;
      }
      if(chan == "0" and sta == "0"){
        Serial.println("OFF");
        digitalWrite(relay_pin,LOW);
        digitalWrite(led_pin,LOW);
        digitalWrite(inLED,HIGH);
        relay_stat = 0;
      }
    }
  }
}

void loop() {
  if (!client.connected()) {
        reconnect();
    }
  client.loop();
  switchStateB = digitalRead(button_pin);
  if (switchStateB == LOW and button_sw == 0){
    button_sw = 1;
    if(relay_stat == 0){
      relay_stat = 1;
      Serial.println("on");
      digitalWrite(relay_pin,HIGH);
      digitalWrite(led_pin,HIGH);
    }
    else if(relay_stat == 1){
      relay_stat = 0;
      Serial.println("off");
      digitalWrite(relay_pin,LOW);
      digitalWrite(led_pin,LOW);
    }
  }
  if (switchStateB == HIGH and button_sw == 1){
    button_sw = 0;
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
    delay(5000);
    client.publish(con_topic, ip_out, true);
    Serial.println(ip_out);
  delay(5000);
}
