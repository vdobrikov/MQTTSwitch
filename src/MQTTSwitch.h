
#ifndef MQTTSWITCH_H_
#define MQTTSWITCH_H_

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include "PubSubClient.h"

#define MQTTSWITCH_DEBUG

#if defined(MQTTSWITCH_DEBUG)
#define MQTTSWITCH_PRINT(str) Serial.print(str)
#define MQTTSWITCH_PRINTLN(str) Serial.println(str)
#else
#define MQTTSWITCH_PRINT(str)
#define MQTTSWITCH_PRINTLN(str)
#endif

// Forward definition
class MQTTSwitch;
class MQTTLight;

// Define callback types
typedef void (*MQTTSwitchOnStateChangeCallback)(MQTTSwitch&, bool);
typedef void (*MQTTLightOnBrightnessChangeCallback)(MQTTLight&, uint8_t);
typedef void (*MQTTLightOnRGBChangeCallback)(MQTTLight&, uint8_t, uint8_t, uint8_t);

class MQTTSwitch{
  protected:
    PubSubClient* _client;
    bool _current_state = false;
    bool needToPublishState = false;
    const char* _command_topic;
    const char* _state_topic;
    const char* _payload_on = "ON";
    const char* _payload_off = "OFF";
    bool _retained = true;
    MQTTSwitchOnStateChangeCallback _on_state_change_callback;

    void turnOn(bool, bool);
    void turnOff(bool, bool);
    void _setState(bool, bool, bool);
    void publishState();

  public:
    MQTTSwitch(const char*, const char*, MQTTSwitchOnStateChangeCallback);
    MQTTSwitch(PubSubClient&, const char*, const char*, MQTTSwitchOnStateChangeCallback);
    void setMqttClient(PubSubClient*);
	void init(bool initialState = false);
    void turnOn();
    void turnOff();
    void toggle();
    void toggle(bool);
    bool isOn();
    bool is(MQTTSwitch&);
    void setStatePayload(const char*, const char*);
    void setRetained(bool);
    virtual void handleMQTTCallback(char*, byte*, unsigned int);
    bool resubscribe();
    void loop();

    void onStateChange(MQTTSwitchOnStateChangeCallback);
};

class MQTTSimpleSwitch : public MQTTSwitch{
  protected:
    uint8_t _pin;
    bool _pin_inverted = false;
    static void _pin_change_function(MQTTSwitch&, bool);
    void setOutputPin(uint8_t, bool);

  public:
    MQTTSimpleSwitch(const char*, const char*, uint8_t);
    MQTTSimpleSwitch(PubSubClient&, const char*, const char*, uint8_t);
    MQTTSimpleSwitch(PubSubClient&, const char*, const char*, uint8_t, bool);
};

class MQTTLight : public MQTTSwitch{
  private:
    const char* _brightness_command_topic = NULL;
    const char* _brightness_state_topic = NULL;
    const char* _rgb_command_topic = NULL;
    const char* _rgb_state_topic = NULL;

    uint8_t _brightness;
    uint8_t _rgb_red;
    uint8_t _rgb_green;
    uint8_t _rgb_blue;

    MQTTLightOnBrightnessChangeCallback _on_brightness_change_callback;
    MQTTLightOnRGBChangeCallback _on_rgb_change_callback;

    void _publishBrightnessState();
    void _publishRGBState();

  public:
    MQTTLight(PubSubClient&, const char*, const char*, MQTTSwitchOnStateChangeCallback);
    void handleMQTTCallback(char*, byte*, unsigned int);

    void setBrightnessCallback(const char*, const char*, MQTTLightOnBrightnessChangeCallback);
    void setRGBCallback(const char*, const char*, MQTTLightOnRGBChangeCallback);

    void setBrightness(uint8_t);
    void setRGB(uint8_t, uint8_t, uint8_t);

};

#endif /* MQTTSWITCH_H_ */
