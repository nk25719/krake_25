#include "mqtt_handler.h"

extern char publish_Ack_Topic[];

bool publishAck(PubSubClient *client, const char *message)
{
  if (client == nullptr || message == nullptr)
  {
    return false;
  }

  return client->publish(publish_Ack_Topic, message);
}


bool publishGPAPResponse(PubSubClient *client, const char *responseType, const char *alarmId)
{
  if (client == nullptr || responseType == nullptr)
  {
    return false;
  }

  char payload[32];

  if (alarmId != nullptr && strlen(alarmId) > 0)
  {
    snprintf(payload, sizeof(payload), "o%s{%s}", responseType, alarmId);
  }
  else
  {
    snprintf(payload, sizeof(payload), "o%s", responseType);
  }

  Serial.print("Publishing GPAP response: ");
  Serial.println(payload);

  return client->publish(publish_Ack_Topic, payload);
}