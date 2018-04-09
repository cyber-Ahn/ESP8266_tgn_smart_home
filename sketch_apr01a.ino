#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <WiFiClient.h> 
#include "DHT.h"
#define DHTTYPE DHT22

const char* ssid = "Matrix";
const char* password = "rhjk0096#Matrix";
const int DHTPin = D4;
const int ButtonPin = D3;
const int inLED = D0;
char* b1 = "off";
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];
unsigned long myChannelNumber = 469382;
const char * myWriteAPIKey = "Q24CHI315VCNGER1";
int switchState = 0;

WiFiClient client;
WiFiServer server(80);
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
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  server.begin();
  Serial.println("Web server running. Waiting for the ESP IP...");
  delay(10000);
  Serial.println(WiFi.localIP());
  Serial.println(WiFi.macAddress());
  ThingSpeak.begin(client);
}
void loop() {
  WiFiClient client = server.available();
  switchState = digitalRead(ButtonPin);
  if (client) {
    Serial.println("New client");
    digitalWrite(inLED,LOW);
    boolean blank_line = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        if (c == '\n' && blank_line) {
            float h = dht2.readHumidity();
            float t = dht2.readTemperature();
            float f = dht2.readTemperature(true);
            if (isnan(h) || isnan(t) || isnan(f)) {
              Serial.println("Failed to read from DHT sensor!");
              strcpy(celsiusTemp,"Failed");
              strcpy(fahrenheitTemp, "Failed");
              strcpy(humidityTemp, "Failed");         
            }
            else{
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
              Serial.print("Button 1: ");
              Serial.println(b1);
              Serial.print("Humidity: ");
              Serial.print(h);
              Serial.print(" %\t Temperature: ");
              Serial.print(t);
              Serial.print(" *C ");
              Serial.print(f);
              Serial.print(" *F\t Heat index: ");
              Serial.print(hic);
              Serial.print(" *C ");
              Serial.print(hif);
              Serial.print(" *F");
              Serial.print("Humidity: ");
              Serial.print(h);
              Serial.print(" %\t Temperature: ");
              Serial.print(t);
              Serial.print(" *C ");
              Serial.print(f);
              Serial.print(" *F\t Heat index: ");
              Serial.print(hic);
              Serial.print(" *C ");
              Serial.print(hif);
              Serial.println(" *F");
            }
            long rssi = WiFi.RSSI();
            long ch = 0 - rssi;
            long prc_out = 0;
            if (ch <= 95 and ch > 89) {
              prc_out = 0;
            }
            if (ch <= 89 and ch > 83) {
              prc_out = 10;
            }
            if (ch <= 83 and ch > 77) {
              prc_out = 20;
            }
            if (ch <= 77 and ch > 71) {
              prc_out = 30;
            }
            if (ch <= 71 and ch > 65) {
              prc_out = 40;
            }
            if (ch <= 65 and ch > 59) {
              prc_out = 50;
            }
            if (ch <= 59 and ch > 53) {
              prc_out = 60;
            }
            if (ch <= 53 and ch > 47) {
              prc_out = 70;
            }
            if (ch <= 47 and ch > 41) {
              prc_out = 80;
            }
            if (ch <= 41 and ch > 35) {
              prc_out = 90;
            }
            if (ch <= 35 and ch > 0) {
              prc_out = 100;
            }
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<head></head><body><h3>IP:");
            client.println(WiFi.localIP());
            client.println("</h3><h3>Data|temp:");
            client.println(celsiusTemp);
            client.println("|humi:");
            client.println(humidityTemp);
            client.println("|b1:");
            client.println(b1);
            client.println("|rssi:");
            client.println(prc_out);
            client.println("$");
            client.println(rssi);
            client.println("</h3><h3>");
            client.println("</body></html>");
            ThingSpeak.writeField(myChannelNumber, 1, celsiusTemp, myWriteAPIKey);
            ThingSpeak.writeField(myChannelNumber, 2, humidityTemp, myWriteAPIKey);    
            break;
        }
        if (c == '\n') {
          blank_line = true;
        }
        else if (c != '\r') {
          blank_line = false;
        }
      }
    }  
    delay(1000);
    client.stop();
    digitalWrite(inLED,HIGH);
    Serial.println("Client disconnected.");
  }
}   
