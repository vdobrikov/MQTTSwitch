#include "MQTTSwitch.h"

MQTTSwitch::MQTTSwitch(PubSubClient& client, const char* command_topic, const char* state_topic, MQTTSwitchOnStateChangeCallback state_change_callback){

  // Store a reference to the MQTT client
  _client = &client;

  // Store the command and state topics
  _command_topic = command_topic;
  _state_topic = state_topic;

  // Default to off, but don't perform any callbacks
  _current_state = false;

  onStateChange(state_change_callback);
}

void MQTTSwitch::_setState(bool newState, bool force){
	
    // Only update the state if it has changed or if this is the first time the state has been set
    if(force || (_current_state != newState)){
        _current_state = newState;

        // Perform the state change callback
        if(_on_state_change_callback){
            _on_state_change_callback(*this, newState);
        }

        // Publish the new state
        _client->publish(_state_topic, (newState) ? _payload_on : _payload_off, _retained);
    }
}

void MQTTSwitch::init(bool initialState) {

	_setState(initialState, true);
}

void MQTTSwitch::turnOn(){

    turnOn(false);
}

void MQTTSwitch::turnOn(bool force){

	_setState(true, force);
}

void MQTTSwitch::turnOff(){
 
    turnOff(false);
}

void MQTTSwitch::turnOff(bool force){
	
	_setState(false, force);
}

void MQTTSwitch::toggle(){
	
  if(isOn()){
    turnOff();
  } else {
    turnOn();
  }
}

bool MQTTSwitch::isOn(){
	
  return _current_state;
}

bool MQTTSwitch::is(MQTTSwitch& mqtt_switch){
	
  return (this == &mqtt_switch);
}

void MQTTSwitch::setStatePayload(const char* payload_on, const char* payoad_off){

  _payload_on = payload_on;
  _payload_off = payoad_off;
}

void MQTTSwitch::setRetained(bool retained){

  _retained = retained;
}

void MQTTSwitch::handleMQTTCallback(char* topic, byte* payload, unsigned int length){
    
  // Check whether the MQTT topic matches the command topic. If so, handle it...
  if(strcmp(topic, _command_topic) == 0){
    if (strncmp((char*) payload, _payload_on, length) == 0){
      turnOn();
    } else if (strncmp((char*) payload, _payload_off, length) == 0){
      turnOff();
    }    
  }
}

bool MQTTSwitch::resubscribe(){
	
  return _client->subscribe(_command_topic);
}

void MQTTSwitch::onStateChange(MQTTSwitchOnStateChangeCallback callback){

  _on_state_change_callback = callback;	
}




MQTTSimpleSwitch::MQTTSimpleSwitch(PubSubClient& client, const char* command_topic, const char* state_topic, uint8_t pin)
  : MQTTSimpleSwitch(client, command_topic, state_topic, pin, false){
  
}

MQTTSimpleSwitch::MQTTSimpleSwitch(PubSubClient& client, const char* command_topic, const char* state_topic, uint8_t pin, bool inverted)
  : MQTTSwitch(client, command_topic, state_topic, MQTTSimpleSwitch::_pin_change_function){
  
  setOutputPin(pin, inverted);
}

void MQTTSimpleSwitch::_pin_change_function(MQTTSwitch& mqtt_switch, bool new_state){

  MQTTSimpleSwitch& simple = (MQTTSimpleSwitch&)mqtt_switch;
  digitalWrite(simple._pin, new_state != simple._pin_inverted);
}

void MQTTSimpleSwitch::setOutputPin(uint8_t output_pin, bool inverted){

  _pin = output_pin;
  _pin_inverted = inverted;
  
  // Set pin to output
  pinMode(output_pin, OUTPUT);
  // Pin defaults to being off
  turnOff(true);
}



MQTTLight::MQTTLight(PubSubClient& client, const char* command_topic, const char* state_topic, MQTTSwitchOnStateChangeCallback callback)
    : MQTTSwitch(client, command_topic, state_topic, callback){

	// Reset callbacks
	_on_brightness_change_callback = NULL;
	_on_rgb_change_callback = NULL;
	_brightness_state_topic = NULL;
        
}


void MQTTLight::handleMQTTCallback(char* topic, byte* payload, unsigned int length){
	
  // Do simple ON / OFF handling using the base class' method
  MQTTSwitch::handleMQTTCallback(topic, payload, length);
  
  // Check if the topic is the brightness command topic, only if a brightness topic has been set
  if (_brightness_command_topic != NULL && strcmp(topic, _brightness_command_topic) == 0) {
		  // Convert the byte array into a c-string
		  char* payload_str = (char*)payload;
		  // Convert the c-string payload to a uint8_t
		  uint8_t new_brightness = atoi(payload_str);
		  
		  // If the result is equal to zero but the string passed was not equal to "0" an error occurred.
		  if(new_brightness == 0 && strncmp(payload_str, "0", 2) != 0){
			  return;
		  }
		  
		  // Get the new brightness from the payload
		  setBrightness(new_brightness);
  }
  else if(_rgb_command_topic != NULL && strcmp(topic, _rgb_command_topic) == 0) {
		  // Parse the payload to retrieve the RGB elements
		  char* str_pointer = (char*)payload;
		  char* str_colour;
		  uint8_t rgb[3];
		  uint8_t rgb_index = 0;

		  while ((str_colour = strtok_r(str_pointer, ",", &str_pointer)) != NULL) {
			  uint8_t colour = atoi(str_colour);
			  
			  // If the colour is equal to zero but the string was not equal to "0", an error occurred
			  if(colour == 0 && strncmp(str_colour, "0", 2) != 0){
				  return;
			  }
			  
			  rgb[rgb_index] = colour;
			  rgb_index++;
		  }

		  // The loop should have gone round 3 times for a valid response
		  if (rgb_index == 3) {
			  setRGB(rgb[0], rgb[1], rgb[2]);
		  }
	  }
}

void MQTTLight::setBrightness(uint8_t brightness){
      
      //std::cout << std::endl << std::endl << "New Brightness: " << brightness;
      
  // Only carry on if the brightness is different
  if(_brightness == brightness){
    return;
  }

  _brightness = brightness;
  
  // Call the brightness change callback if one is defined
  if(_on_brightness_change_callback){
    _on_brightness_change_callback(*this, brightness);
  }	
  
  // Publish the new brightness if a brightness topic exists
  if (_brightness_state_topic) {
	  char brightness_buffer[4];
	  itoa(_brightness, brightness_buffer, 10);

	  _client->publish(_brightness_state_topic, brightness_buffer, _retained);
  }
}

void MQTTLight::setRGB(uint8_t red, uint8_t green, uint8_t blue){

  // Only carry on if a new colour has been defined
  if(_rgb_red == red && _rgb_green == green && _rgb_blue == blue){
    return;
  }
  _rgb_red = red;
  _rgb_green = green;
  _rgb_blue = blue;

  if(_on_rgb_change_callback){
    _on_rgb_change_callback(*this, red, green, blue);
  }

  uint8_t rgb_buffer_length = 12;
  char rgb_buffer[rgb_buffer_length];
  
  // Using snprintf uses 16 bytes more program memory - no need to investigate
  //	an alternative for now
  snprintf(rgb_buffer, rgb_buffer_length, "%03d,%03d,%03d", _rgb_red, _rgb_green, _rgb_blue);

  // Publish the new state
  _client->publish(_rgb_state_topic, rgb_buffer, _retained);
}

void MQTTLight::setBrightnessCallback(const char* command_topic, const char* state_topic, MQTTLightOnBrightnessChangeCallback callback){
    
    _brightness_command_topic = command_topic;
    _brightness_state_topic = state_topic;
    _on_brightness_change_callback = callback;
}

void MQTTLight::setRGBCallback(const char* command_topic, const char* state_topic, MQTTLightOnRGBChangeCallback callback){
    
    _rgb_command_topic = command_topic;
    _rgb_state_topic = state_topic;
    _on_rgb_change_callback = callback;
}
