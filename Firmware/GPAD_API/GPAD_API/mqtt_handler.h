#ifndef MQTT_HANDLER_H
#define MQTT_HANDLER_H

#include <PubSubClient.h>

struct PendingMqtt
{
  char topic[96];
  char payload[128];
  bool retain;
  bool active;
};

bool queueMqtt(const char *topic, const char *payload, bool retain = false);
void serviceMqttQueue(PubSubClient *client);
bool publishAck(PubSubClient *client, const char *message);
bool publishGPAPResponse(PubSubClient *client, const char *responseType, const char *alarmId);
#endif
