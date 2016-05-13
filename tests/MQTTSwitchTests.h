#include "AxxTest.h"
#include "MQTTSwitch.h"

boolean test_callback_state = false;
boolean test_callback_state2 = false;

void test_callback(MQTTSwitch& mqttSwitch, boolean new_state){
    
    test_callback_state = new_state;
}

void test_callback2(MQTTSwitch& mqttSwitch, boolean new_state){
    
    test_callback_state2 = new_state;
}

class MQTTSwitchTests : public CxxTest::TestSuite{

    private:
        PubSubClient* client;
        MQTTSwitch* sw;
        const char* command_topic = "command";
        const char* state_topic = "state";
        const char* default_topic = "UNINITIALISED";
        byte payload_off[4] = {0x4F, 0x46, 0x46, 0x00}; // OFF
        byte payload_on[3] = {0x4F, 0x4E, 0x00};        // ON
		byte invalid_payload[7] = {0x53, 0x57, 0x46, 0x54, 0x43, 0x48, 0x00}; // SWITCH
        
    public:
  
        void setUp(){
            
            // This is called before each test.
            // Use it to initialise any variables or objects you might need
            client = new PubSubClient();
            sw = new MQTTSwitch(*client, command_topic, state_topic, test_callback);
            
			sw->init(false);
        }
        
        void setUpCallbackTests(){
            
            // Reset callback state
            test_callback_state = false;
            test_callback_state2 = false;
        }
        
        
        // Basic functionality
        void testInitialState(){
            
            TS_ASSERT_EQUALS(false, sw->isOn());
        }
        
        void testTurnOn(){
            
            sw->turnOn();
            
            TS_ASSERT_EQUALS(true, sw->isOn());
        }
        
        void testTurnOnTurnOff(){
            
            sw->turnOn();
            sw->turnOff();
            
            TS_ASSERT_EQUALS(false, sw->isOn());
        }
        
        void testToggle(){
            
            sw->toggle();
            
            TS_ASSERT_EQUALS(true, sw->isOn());
        }
        
        void testTestDoubleToggle(){
            
            sw->toggle();
            sw->toggle();
            
            TS_ASSERT_EQUALS(false, sw->isOn());
        }
		
		void testEquality(){
			
			MQTTSwitch* sw2 = new MQTTSwitch(*client, command_topic, state_topic, test_callback);
			MQTTSwitch* sw_compare = sw;
			
			TS_ASSERT(sw_compare->is(*sw));
			TS_ASSERT(!sw_compare->is(*sw2));
			
		}

        
        // MQTT Publishing Tests
        void testPublishInitialState(){
			
            TS_ASSERT_SAME_DATA(state_topic, client->last_publish_topic, strlen(state_topic));
            TS_ASSERT_SAME_DATA(payload_off, client->last_publish_payload, strlen((char*)payload_off));
            TS_ASSERT_EQUALS(true, client->last_publish_retained);
            TS_ASSERT_EQUALS(1, client->publish_count);
        }
        
        void testPublishTopic(){
                        
            sw->turnOn();
            
            TS_ASSERT_SAME_DATA(state_topic, client->last_publish_topic, strlen(state_topic));
            TS_ASSERT_EQUALS(2, client->publish_count);
        }
        
        void testPublishOnlyOnNewState(){
            
            // As switch defaults to being off, no publishing should occur
            sw->turnOff();
            sw->turnOff();
            sw->turnOff();
            sw->turnOff();
            sw->turnOff();
            
            TS_ASSERT_SAME_DATA(state_topic, client->last_publish_topic, strlen(state_topic));  
            TS_ASSERT_EQUALS(1, client->publish_count);
        }
        
        void testPublishPayloadTurnOn(){
            
            sw->turnOn();
            TS_ASSERT_SAME_DATA("ON", client->last_publish_payload, 3);
        }
        
        void testPublishPayloadTurnOff(){
            
            sw->turnOn();
            sw->turnOff();
            TS_ASSERT_SAME_DATA("OFF", client->last_publish_payload, 4);
        }
        
        void testPublishRetainedFalse(){
            
            sw->setRetained(false);
            sw->turnOn();
            
            TS_ASSERT_EQUALS(false, client->last_publish_retained);
        }
		
		void testPublishNewPayload(){
			
			sw->setStatePayload("OPEN", "CLOSED");
			sw->turnOn();
			TS_ASSERT_SAME_DATA("OPEN", client->last_publish_payload, 5);
			
			sw->turnOff();
			TS_ASSERT_SAME_DATA("CLOSED", client->last_publish_payload, 7);	
		}
        
        
        // MQTT Subscribe Tests
        void testSubscribeInitialState(){
            
            TS_ASSERT_SAME_DATA(default_topic, client->last_subscribe_topic, strlen(default_topic));
        }
        void testSubscribeResubscribe(){
            
            
            sw->resubscribe();
            
            TS_ASSERT_SAME_DATA(command_topic, client->last_subscribe_topic, strlen(command_topic));
        }
        
        void testSubscribeHandleTopicTurnOn(){
            
            sw->handleMQTTCallback(const_cast<char*>(command_topic), payload_on, 2);
            
            TS_ASSERT_EQUALS(true, sw->isOn());
        }
        
        void testSubscribeHandleTopicTurnOff(){
            
            sw->turnOn();
            
            sw->handleMQTTCallback(const_cast<char*>(command_topic), payload_off, 4);
            
            TS_ASSERT_EQUALS(false, sw->isOn());
        }
        
        void testSubscribeHandleWrongTopic(){
            
            sw->handleMQTTCallback(const_cast<char*>(default_topic), payload_on, 3);
            
            TS_ASSERT_EQUALS(false, sw->isOn());
        }
		
		void testSubscribeHandleWrongPayload(){
		
			sw->handleMQTTCallback(const_cast<char*>(command_topic), invalid_payload, 7);
			
			TS_ASSERT_EQUALS(false, sw->isOn());
		}
        
        
        
        // Callback Tests
        void testCallbackTurnOn(){
            
            setUpCallbackTests();
            
            sw->turnOn();
            
            TS_ASSERT_EQUALS(true, test_callback_state);
        }
        
        void testCallbackTurnOff(){
            
            setUpCallbackTests();
            
            sw->turnOn();
            sw->turnOff();
            
            TS_ASSERT_EQUALS(false, test_callback_state);
        }
		
		void testChangeCallback(){
			
			setUpCallbackTests();
			
			sw->onStateChange(test_callback2);
			
			sw->turnOn();
			
			TS_ASSERT_EQUALS(true, test_callback_state2);
		}
};