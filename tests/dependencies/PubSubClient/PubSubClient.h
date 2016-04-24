#ifndef _PubSubClient_H
#define _PubSubClient_H

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif

#define PUBLISH_SIZE 20

class PubSubClient { 

    public:
        PubSubClient();
        boolean publish(const char* topic, const char* payload, boolean retained);
        boolean subscribe(const char* topic);
        
        char last_publish_topic[PUBLISH_SIZE];
        char last_publish_payload[PUBLISH_SIZE];
        boolean last_publish_retained;
        uint8_t publish_count = 0;
        
        char last_subscribe_topic[PUBLISH_SIZE];
};

#endif // _PubSubClient_H