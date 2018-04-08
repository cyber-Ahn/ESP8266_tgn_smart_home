#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <WiFiClient.h> 
#include "DHT.h"
#define DHTTYPE DHT22

const char* ssid = "Your SSID";
const char* password = "Your password";
const int DHTPin = D4;
const int ButtonPin = D3;
const int inLED = D0;
char* b1 = "off";
static char celsiusTemp[7];
static char fahrenheitTemp[7];
static char humidityTemp[7];
unsigned long myChannelNumber = 12234545; // your thinkspeak channel number
const char * myWriteAPIKey = "your thinkspeak write-key";
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
              ThingSpeak.writeField(myChannelNumber, 1, celsiusTemp, myWriteAPIKey);
              ThingSpeak.writeField(myChannelNumber, 2, humidityTemp, myWriteAPIKey);
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
            client.println("</h3><h3>");
            client.println("</body></html>");     
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
