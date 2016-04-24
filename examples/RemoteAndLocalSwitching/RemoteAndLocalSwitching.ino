// Connect LEDs and resistors to pins 3, 4 and ground
// Connect a momentary normally open switch between 5, 6 and ground
//
// 3--[LED]--[RES]--|
//                  |
// 4--[LED]--[RES]--|
//                  |
// 5-------/ -------|
//                  |
// 6-------/ -------|
//                  |
//                 GND
//
// Once the sketch is loaded onto your ESP8266 device, publish ON or OFF to the topic home/switch/one/set or home/switch/two/set. (Make sure you copy the topics exactly)
//
// If you subscribe to the topics home/switch/one or home/switch/two you should be updated whenever the switch state changes.
//
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <MQTTSwitch.h>

#define MQTT_SERVER "server.ip"
#define MQTT_PORT 1883
//#define MQTT_USERNAME "username"
//#define MQTT_PASSWORD "password"

#define WIFI_SSID "*************"
#define WIFI_PASSWORD "*************"

WiFiClient espClient;
PubSubClient client(espClient);
MQTTSimpleSwitch Switch1(client, "home/switch/one/set", "home/switch/one", 3);
MQTTSimpleSwitch Switch2(client, "home/switch/two/set", "home/switch/two", 4);

const uint8_t onBoardTogglePin_One = 5;    // Connect a simple push button between this pin and GND
const uint8_t onBoardTogglePin_Two = 6;    // Connect a simple push button between this pin and GND

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(mqttCallback);

  Switch1.init();
  Switch2.init();
  
  // Setup pins
  pinMode(onBoardTogglePin_LivingRoom, INPUT_PULLUP);
  pinMode(onBoardTogglePin_Boiler, INPUT_PULLUP);
}

void setup_wifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    #if defined(MQTT_USERNAME) and defined(MQTT_PASSWORD)
      if (client.connect("ESP8266Client", MQTT_USERNAME, MQTT_PASSWORD)) {
    #else
      if (client.connect("ESP8266Client")) {
    #endif

      Serial.println("connected");
      } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }

  // Resubscribe to any topics
  Switch1.resubscribe();
  Switch2.resubscribe();
}

void loop(){
  
  // Ensure we're still connected to the MQTT server
  if(!client.connected()){
    reconnect();
  }
  
  // Check for any new broadcasts
  client.loop();
  
  // Check the pin state. If the pin is low it is pressed and we should toggle the state of the switch
  // Doing this locally should publish an MQTT message on the relevant topic
  if(digitalRead(onBoardTogglePin_One) == LOW){
    Switch1.toggle();    
  }

  if(digitalRead(onBoardTogglePin_Two) == LOW){
    Switch2.toggle();
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length){

  // List all your switches here
  Switch1.handleMQTTCallback(topic, payload, length);
  Switch2.handleMQTTCallback(topic, payload, length);
}