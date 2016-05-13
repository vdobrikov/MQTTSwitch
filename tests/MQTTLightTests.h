#include "AxxTest.h"
#include "MQTTSwitch.h"

bool test_light_on = false;
uint8_t test_r = 0;
uint8_t test_g = 0;
uint8_t test_b = 0;
uint8_t test_brightness = 0;

void test_toggle_callback(MQTTSwitch& sw, bool new_state){
    
    test_light_on = new_state;
}

void test_rgb_callback(MQTTLight& light, uint8_t r, uint8_t g, uint8_t b){
    
    test_r = r;
    test_g = g;
    test_b = b;
}

void test_brightness_callback(MQTTLight& light, uint8_t brightness){
    
    test_brightness = brightness;
}

class MQTTLightTests : public CxxTest::TestSuite{

    private:
        PubSubClient* client;
        MQTTLight* light;
        const char* command_topic = "command";
        const char* state_topic = "state";
        const char* brightness_command_topic = "command_brightness";
        const char* brightness_state_topic = "state_brightness";
        const char* rgb_command_topic = "command_rgb";
        const char* rgb_state_topic = "state_rgb";
        const char* default_topic = "UNINITIALISED";
        byte payload_off[4] = {0x4F, 0x46, 0x46, 0x00}; // OFF
        byte payload_on[3] = {0x4F, 0x4E, 0x00};        // ON
        
    public:
  
        void setUp(){
            
            // This is called before each test.
            // Use it to initialise any variables or objects you might need
            client = new PubSubClient();
            light = new MQTTLight(*client, command_topic, state_topic, test_toggle_callback);
			light->init(false);
            
            // Reset test variables
            test_light_on = false;
            test_r = 0;
            test_g = 0;
            test_b = 0;
            test_brightness = 0;
        }
		
		void setUpBrightness(){
			
            light->setBrightnessCallback(brightness_command_topic, brightness_state_topic, test_brightness_callback);
		}
		
		void setUpRGB(){
			
            light->setRGBCallback(rgb_command_topic, rgb_state_topic, test_rgb_callback);
		}
        
        void testCallbackBrightness(){
			
			setUpBrightness();
            
            uint8_t brightness = 80;
                        
            light->setBrightness(brightness);
            
            TS_ASSERT_EQUALS(brightness, test_brightness);
        }
        
        void testCallbackRGB(){
			
			setUpRGB();
            
            uint8_t r = 40;
            uint8_t g = 170;
            uint8_t b = 245;
            
            light->setRGB(r, g, b);
			            
            TS_ASSERT_EQUALS(r, test_r);
            TS_ASSERT_EQUALS(g, test_g);
            TS_ASSERT_EQUALS(b, test_b);
        }
        
        void testPublishBrightness(){
            
			setUpBrightness();
			
            uint8_t brightness = 67;
            
            light->setBrightness(brightness);
            
            TS_ASSERT_SAME_DATA(brightness_state_topic, client->last_publish_topic, strlen(brightness_state_topic));
            TS_ASSERT_EQUALS(brightness, atoi(client->last_publish_payload));
            TS_ASSERT_EQUALS(2, client->publish_count);
        }
        
        void testPublishRGB(){
            
			setUpRGB();
			
            uint8_t r = 40;
            uint8_t g = 170;
            uint8_t b = 245;
            const char* rgb_state_payload = "040,170,245";
            
            light->setRGB(r, g, b);
            
            TS_ASSERT_SAME_DATA(rgb_state_topic, client->last_publish_topic, strlen(rgb_state_topic));
            TS_ASSERT_SAME_DATA(rgb_state_payload, client->last_publish_payload, strlen(rgb_state_payload));
            TS_ASSERT_EQUALS(2, client->publish_count);
            
        }
        
        void testSubscribeHandleBrightness(){
       
			setUpBrightness();
	   
            #define PAYLOAD_BRIGHTNESS_LENGTH 3
            byte payload_brightness[PAYLOAD_BRIGHTNESS_LENGTH] = {0x37, 0x34, 0x00};         // 74
            int brightness = 74;
            
            light->handleMQTTCallback(const_cast<char*>(brightness_command_topic), payload_brightness, PAYLOAD_BRIGHTNESS_LENGTH);
                        
            TS_ASSERT_EQUALS(brightness, test_brightness);
        }
		
		void testSubscribeHandleBrightnessInvalid(){
			
			setUpBrightness();
			
			#define PAYLOAD_INVALID_BRIGHTNESS_LENGTH 3
			byte payload_brightness[PAYLOAD_INVALID_BRIGHTNESS_LENGTH] = {0x4F, 0x4E, 0x00};	// ON
			
			// Set the brightness to some random value
			int dummy_brightness = 30;
			light->setBrightness(dummy_brightness);
			
			light->handleMQTTCallback(const_cast<char*>(brightness_command_topic), payload_brightness, PAYLOAD_BRIGHTNESS_LENGTH);
			
			// Ensure the brightness has not changed
			TS_ASSERT_EQUALS(dummy_brightness, test_brightness);
		}
		
		void testSubscribeHandleBrightnessZero(){
			
			setUpBrightness();
			
			#define PAYLOAD_ZERO_BRIGHTNESS_LENGTH 2
			byte payload_brightness[PAYLOAD_ZERO_BRIGHTNESS_LENGTH] = {0x30, 0x00};	// 0
			
			// Set the brightness to some random value
			int dummy_brightness = 55;
			light->setBrightness(dummy_brightness);
			
			light->handleMQTTCallback(const_cast<char*>(brightness_command_topic), payload_brightness, PAYLOAD_BRIGHTNESS_LENGTH);
			
			// Ensure the brightness is now zero
			TS_ASSERT_EQUALS(0, test_brightness);
		}
        
        void testSubscribeHandleRGB(){
            
			setUpRGB();
			
            #define PAYLOAD_RGB_LENGTH 12
            byte payload_rgb[PAYLOAD_RGB_LENGTH] = {0x31, 0x39, 0x33, 0x2C, 0x30, 0x32, 0x39, 0x2C, 0x32, 0x30, 0x30, 0x00};  // 193,029,200
            uint8_t r = 193;
            uint8_t g = 29;
            uint8_t b = 200;
            
            light->handleMQTTCallback(const_cast<char*>(rgb_command_topic), payload_rgb, PAYLOAD_RGB_LENGTH);
                        
            TS_ASSERT_EQUALS(r, test_r);
            TS_ASSERT_EQUALS(g, test_g);
            TS_ASSERT_EQUALS(b, test_b);
        }
        
       
};