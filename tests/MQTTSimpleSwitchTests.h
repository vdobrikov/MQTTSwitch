#include "AxxTest.h"
#include "MQTTSwitch.h"

class MQTTSimpleSwitchTests : public CxxTest::TestSuite{

    private:
        PubSubClient* client;
        MQTTSimpleSwitch* sw;
        MQTTSimpleSwitch* sw_inv;
        const char* command_topic = "command";
        const char* state_topic = "state";
        const char* default_topic = "UNINITIALISED";
        int pin = 4;
        int pin_inv = 7;
        
    public:
  
        void setUp(){
            
            // This is called before each test.
            // Use it to initialise any variables or objects you might need
			AxxTest::initialiseArduinoMock();
			
            client = new PubSubClient();
            sw = new MQTTSimpleSwitch(*client, command_topic, state_topic, pin);
            sw_inv = new MQTTSimpleSwitch(*client, command_topic, state_topic, pin_inv, true);
        }
        
        
        void testInitialState(){
            			
            TS_ASSERT_PIN_MODE(pin, OUTPUT);
            TS_ASSERT_DIGITAL_WRITE(pin, LOW);
        }
        
        void testTurnOn(){
            
            sw->turnOn();
            
            TS_ASSERT_DIGITAL_WRITE(pin, HIGH);
        }
        
        void testTurnOnTurnOff(){
            
            sw->turnOn();
            sw->turnOff();
            
            TS_ASSERT_DIGITAL_WRITE(pin, LOW);
        }
        
        void testInitialStateInverted(){
            
            TS_ASSERT_PIN_MODE(pin_inv, OUTPUT);
            TS_ASSERT_DIGITAL_WRITE(pin_inv, HIGH);
        }
        
        void testTurnOnInverted(){
            
            sw_inv->turnOn();
            
            TS_ASSERT_DIGITAL_WRITE(pin_inv, LOW);
        }
        
        void testTurnOffInverted(){
            
            sw_inv->turnOn();
            sw_inv->turnOff();
            
            TS_ASSERT_DIGITAL_WRITE(pin_inv, HIGH);
        }
};