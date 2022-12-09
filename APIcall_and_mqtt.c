#include <M5StickCPlus.h>
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>

WiFiMulti wifiMulti;
WiFiClient espClient;
PubSubClient * client;
HTTPClient http;

const char* mqtt_server = "test.mosquitto.org";


const int servo_pin = 26;
int freq            = 50;
int ledChannel      = 0;
int resolution      = 10;
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (500)
char msg[MSG_BUFFER_SIZE];
int value = 0;



void callback(char* topic, byte* payload, unsigned int length) {
   M5.Lcd.setTextColor(WHITE);  
   M5.Lcd.setTextSize(1);
   M5.Lcd.print("call had been made");
   M5.Lcd.println();
   M5.Lcd.print("[HTTP] begin...\n");
   http.begin(
            "https://sm79586-iot.uk.r.appspot.com/api/check_image_v2/pic_nm_1.jpg/");  // configure traged server and
   M5.Lcd.print("[HTTP] GET...\n");
   int httpCode = http.GET();  // start connection and send HTTP header.
   if (httpCode ==
        HTTP_CODE_OK) {  // file found at server.  
    String payload = http.getString();
    M5.Lcd.print(payload); 
    if (payload.indexOf('No') != -1)
        {
        M5.Lcd.print(payload.indexOf('No')); 
        ledcWriteTone(ledChannel, 1250);
        delay(1000);
        ledcWriteTone(ledChannel, 0);
        delay(1000);
        }
     }
  else {
            M5.Lcd.printf("[HTTP] GET... failed, error: %s\n",
                          http.errorToString(httpCode).c_str());
        }
        http.end();

}


void setup() {
    M5.begin();
    M5.Lcd.setRotation(3);              // Rotate the screen.  旋转屏幕
    wifiMulti.addAP("utexas-iot", "83835902811433164932");  // Storage wifi configuration info
    M5.Lcd.print("\nConnecting Wifi...\n");  // print format output string on
    ledcSetup(ledChannel, freq, resolution);
    client = new PubSubClient(espClient);
    client->setServer(mqtt_server, 1883);
    client->setCallback(callback);
}

void reconnect() {
  while (!client->connected()) {
    M5.Lcd.print("Trying to establish MQTT connection .. ");
    String clientId = "ESP8266Client - SivaClient";
    if (client->connect(clientId.c_str(), "etassignment2022", "etassignment2022")) {
      M5.Lcd.println("connected");
      client->publish("IotTopic- Siva", "IotTopic- Siva");
      client->subscribe("Topic1 - Aboorva");
    } else {
      M5.Lcd.print("failed, rc = ");
      M5.Lcd.print(client->state());
      M5.Lcd.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}



void loop() {
//    M5.Lcd.setCursor(0, 0);  // Set the cursor at (0,0).  
    if ((wifiMulti.run() ==
         WL_CONNECTED)) {  // wait for WiFi connection. 
           if (!client->connected()) {
                reconnect();
                 }
           client->loop();
           M5.update(); 

    } else {
        M5.Lcd.print("connect failed");
    }
    delay(5000);
//    M5.Lcd.fillRect(0, 0, 160, 80, BLACK);  

}