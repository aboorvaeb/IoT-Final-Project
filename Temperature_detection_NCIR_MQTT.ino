// including relevant libraries
#include <M5StickCPlus.h>
#include <Wire.h>
// library for Pub and Sub functionality
#include <PubSubClient.h>
#include <WiFi.h>

// using the publicly available mosquitto server/broker
const char* mqtt_server = "test.mosquitto.org";

// declaring the client variables
WiFiClient espClient;
PubSubClient * client;

//utexas-iot credentials
const char* ssid       = "utexas-iot";
const char* password   = "71824669544815939224";

uint16_t result;
float temperature;

// declaring message configurations
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (500)
char msg[MSG_BUFFER_SIZE];
int value = 0;

// setting up WiFi
void setup_wifi() {
  delay(10);
  // checking the connection by printing ssid
  M5.Lcd.println();
  M5.Lcd.print("Connecting to ");
  M5.Lcd.println(ssid);

  //  beginning WiFi connection
  WiFi.begin(ssid, password);

  // waiting for connection to be established
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    // printing "." until connection is established
    M5.Lcd.print(".");
  }

  // pseudo random number generator
  randomSeed(micros());

  // "Connected" message printed if connection is established
  M5.Lcd.println("WiFi connected");
}


// callback function which will process the response received
void callback(char* topic, byte* payload, unsigned int length) {
   M5.Lcd.print("Message arrived [");
   // displaying the topic we subscribed to
   M5.Lcd.print(topic);
   M5.Lcd.print("] ");
   // printing the message received from the payload (from Client2)
   for (int i = 0; i < length; i++) {
     M5.Lcd.print((char)payload[i]);
   }
  M5.Lcd.println();
}

void reconnect() {
  // Looping until connection is established
  while (!client->connected()) {
   // M5.Lcd.print("Trying to establish MQTT connection .... ");
    String clientId = "ESP8266Client - AboorvaClient";
    // attempting connection to the server from client end
    if (client->connect(clientId.c_str(), "etassignment3", "etassignment3")) {
      // if connected, display message
      //M5.Lcd.println("connected");
      // once connected, publish the topic details
      client->publish("Topic1 - Aboorva", "Topic1 - Aboorva");
      // resubscribing to topic on which the other client will be publishing on
      client->subscribe("IotTopic- Siva");
    } else {
      // failed connection notification
      M5.Lcd.print("failed, rc = ");
      M5.Lcd.print(client->state());
      M5.Lcd.println(" try again in 5 seconds");
      // wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void setup() {
  // wifi
  delay(500);
  // when opening the Serial Monitor, select 9600 Baud
  M5.begin();
  delay(500);
  // setting text color to yellow
  M5.Lcd.setTextColor(YELLOW);
  // connecting to wifi
  setup_wifi();
  // initializing the PubSubClient with client details
  client = new PubSubClient(espClient);
  // specifying server address and port number to connect to
  client->setServer(mqtt_server, 1883);
  // client is pointing to the callback function which will then be processing the incoming message  
  client->setCallback(callback);
  M5.Lcd.setTextSize(1);




  // temperarture
    M5.begin();
    Wire.begin(0, 26);
    Serial.begin(115200);

   // M5.Lcd.setRotation(3);
    M5.Lcd.setTextColor(WHITE);
   // M5.Lcd.setTextSize(3);
    //M5.Lcd.clear(BLACK);
    M5.Lcd.setCursor(0, 40);
}

void loop() {

    
    Wire.beginTransmission(0x5A);  // Send Initial Signal and I2C Bus Address
    Wire.write(0x07);  // Send data only once and add one address automatically.
    Wire.endTransmission(false);  // Stop signal
    Wire.requestFrom(
        0x5A,
        2);  // Get 2 consecutive data from 0x5A, and the data is stored only.
    result = Wire.read();        // Receive DATA
    result |= Wire.read() << 8;  // Receive DATA

    temperature = ((result * 0.02 - 273.15) * 1.8)  + 32;

    M5.Lcd.fillRect(0, 40, 120, 100, BLACK);
    M5.Lcd.setCursor(0, 40);

   
    M5.Lcd.print(temperature);



    delay(500);
    M5.update();
    //pub sub
      // trying ot reconnect if connection is interrupted
  if (!client->connected()) {
    reconnect();
  }
  client->loop();

  
    M5.update();  // Read the press state of the key.
    // reads the state of the key (pressed/not)
    //if (M5.BtnA.wasReleased()) {  // If the button A is pressed. 
    if (temperature>99) {  
          M5.Lcd.setTextColor(WHITE);
          M5.Lcd.print("Button from Client1 was pressed");
          // setting the text color to WHITE
          M5.Lcd.setTextColor(WHITE);        
          // defining the message to the topic (publishing)  
          snprintf (msg, MSG_BUFFER_SIZE, "Hello server");
          // printing the published message on Client1 end once button is pressed
          M5.Lcd.print("\nPublish message: ");
          M5.Lcd.println(msg);
          client->publish("Topic1 - Aboorva", msg);
    }


}







