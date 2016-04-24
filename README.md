# MQTTSwitch

[![Build Status](https://travis-ci.org/r89m/MQTTSwitch.svg?branch=master)](https://travis-ci.org/r89m/MQTTSwitch)
[![Coverage Status](https://coveralls.io/repos/github/r89m/MQTTSwitch/badge.svg?branch=master)](https://coveralls.io/github/r89m/MQTTSwitch?branch=master)

This library makes controlling and monitoring switches over MQTT easy.

You can use this library to easily control a digital pin, RGB light or any other kind of switch.

## Examples
Here's a basic examples to show you just how easy using this library is!

```c++
// Connect LEDs and resistors to pins 3, 4 and ground
//
// 3--[LED]--[RES]--|
//                  |
// 4--[LED]--[RES]--|
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

void setup() {
  Serial.begin(115200);
  setup_wifi();
  client.setServer(MQTT_SERVER, MQTT_PORT);
  client.setCallback(mqttCallback);
  
  Switch1.init();
  Switch2.init();
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
}

void mqttCallback(char* topic, byte* payload, unsigned int length){

  // List all your switches here
  Switch1.handleMQTTCallback(topic, payload, length);
  Switch2.handleMQTTCallback(topic, payload, length);
}

```

## API

### MQTTSwitch

```c++
MQTTSwitch(PubSubClient& client, const char* command_topic, const char* state_topic, MQTTSwitchOnStateChangeCallback callback);
```

A generic switch which calls a callback function whenever the switch state is changed. The callback receives a pointer the related MQTTSwitch instance as well as the new state. 
From this information it is possible for you to determine what to do next.

The callback must have the following prototype:
```c++
void callback(MQTTSwitch& mqttSwitch, boolean new_state){}
```

#### void init(bool initialState = false)


#### void turnOn()

#### void turnOff()

#### void toggle()

#### bool isOn()

#### bool is(MQTTSwitch& compareSwitch)

#### void setStatePayload(const char* payloadOn, const char* payloadOff)

#### void setRetained(bool retained)

#### void handleMQTTCallback(char* topci, byte* payload, unsigned int length)

#### void onStateChange(MQTTSwitchOnStateChangeCallback callback)

### MQTTSimpleSwitch

```c++
MQTTSimpleSwitch(PubSubClient& client, const char* command_topic, const char* state_topic, uint8_t pin);
```
or
```c++
MQTTSimpleSwitch(PubSubClient& client, const char* command_topic, const char* state_topic, uint8_t pin, bool invert_logic);
```

This is a specific type of `MQTTSwitch` which directly support toggling the given digital output pin. For many projects, this is the easiest type to use.


### MQTTLight

```c++
MQTTLight(PubSubClient& client, const char* command_topic, const char* state_topic, MQTTSwitchOnStateChangeCallback callback);
````

This is a specific type of `MQTTSwitch` which provides additional methods for controlling lights, such as the brightness or the specific colour. 

These additional features do not have to be used for the `MQTTLight` to work.

#### void setRGBCallback(const char* rgb_command_topic, const char* rgb_state_topic, MQTTLightOnRGBChangeCallback rgb_change_callback);

#### void setBrightness(uint8_t new_brightness);

#### void setRGB(uint8_t red, uint8_t green, uint8_t blue);

## Tested and Reliable

This code has been unit tested using a virtual Arduino, ensuring that any reading and writing of digital pins has been verified to work.

Additionally, a virtual MQTT client was used to verify the behaviour of this library. The tests can be found in the [tests](tests) folder.

Code coverage is shown in the badge below the library name, and can be verified at the [Coveralls.io repo page](https://coveralls.io/github/r89m/MQTTSwitch?branch=master)