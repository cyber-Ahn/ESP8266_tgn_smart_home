#include "ESPHelper.h"
#include <Metro.h>

const char* ssid = "name";
const char* wifi_password = "pwd";
const char* mqtt_server = "ip";
String homecode = "10101";
String modul = "6";

const int relay_pin = D6;
const int led_pin = D7;
const int button_pin = D3;

char* data_topic = "tgn/sonoff/data";
char* res_topic = "tgn/system/reboot/sonoff";
const char* con_topic = "tgn/sonoff_1/connection/ip";
const char* update_topic = "tgn/sonoff_1/update";
const char* clientID = "sonoff_1 V0.2";
int switchStateB = 0;
int button_sw = 0;
int relay_stat = 0;

netInfo homeNet = {  .mqttHost = mqtt_server,
          .mqttUser = "",
          .mqttPass = "", 
          .mqttPort = 1883,
          .ssid = ssid, 
          .pass = wifi_password};

ESPHelper myESP(&homeNet);
Metro publishTimer = Metro(30000);

void setup() {
  pinMode(relay_pin, OUTPUT);
  pinMode(led_pin, OUTPUT);
  pinMode(button_pin, INPUT);
  Serial.begin(9600);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  myESP.OTA_enable();
  myESP.OTA_setPassword("sonoff1");
  myESP.OTA_setHostnameWithVersion(clientID);
  myESP.addSubscription(data_topic);
  myESP.addSubscription(res_topic);
  myESP.addSubscription(update_topic);
  myESP.setMQTTCallback(callback);
  myESP.begin();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  digitalWrite(led_pin,HIGH);
  delay(5000);
  digitalWrite(led_pin,LOW);
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
      myESP.publish(res_topic, "0", true);
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
        relay_stat = 1;
      }
      if(chan == "0" and sta == "0"){
        Serial.println("OFF");
        digitalWrite(relay_pin,LOW);
        digitalWrite(led_pin,LOW);
        relay_stat = 0;
      }
    }
  }
}

void loop() {
  myESP.loop();
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
