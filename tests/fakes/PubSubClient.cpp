#include "PubSubClient.h"

PubSubClient::PubSubClient(){
    
    memcpy(last_publish_topic, "UNINITIALISED", PUBLISH_SIZE);
    memcpy(last_publish_payload, "UNINITIALISED", PUBLISH_SIZE);
    last_publish_retained = false;
    
    memcpy(last_subscribe_topic, "UNINITIALISED", PUBLISH_SIZE);
}

boolean PubSubClient::publish(const char* topic, const char* payload, boolean retained){
        
    if(topic){
        memcpy(last_publish_topic, topic, PUBLISH_SIZE);
        
    }
    if(payload){
        memcpy(last_publish_payload, payload, PUBLISH_SIZE);
    }
    last_publish_retained = retained;
    publish_count++;
}

boolean PubSubClient::subscribe(const char* topic){
    
    memcpy(last_subscribe_topic, topic, PUBLISH_SIZE);
}