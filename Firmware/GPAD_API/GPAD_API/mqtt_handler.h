#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <PubSubClient.h>

bool publishAck(PubSubClient *client, const char *message);
bool publishGPAPResponse(PubSubClient *client, const char *responseType, const char *alarmId);
#endif
