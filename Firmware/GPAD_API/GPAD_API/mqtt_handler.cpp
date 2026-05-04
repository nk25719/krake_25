#include "mqtt_handler.h"
#include <stdio.h>
#include <string.h>

extern char publish_Ack_Topic[];

static bool isJsonObjectMessage(const char *message)
{
  const size_t len = strlen(message);
  return len >= 2 && message[0] == '{' && message[len - 1] == '}';
}

static void appendEscapedJsonString(char *dest, size_t destSize, const char *src)
{
  if (destSize == 0)
  {
    return;
  }

  size_t j = 0;
  for (size_t i = 0; src[i] != '\0' && j + 1 < destSize; ++i)
  {
    const char c = src[i];
    if ((c == '\\' || c == '"') && j + 2 < destSize)
    {
      dest[j++] = '\\';
      dest[j++] = c;
    }
    else if (c == '\n' && j + 2 < destSize)
    {
      dest[j++] = '\\';
      dest[j++] = 'n';
    }
    else if (c == '\r' && j + 2 < destSize)
    {
      dest[j++] = '\\';
      dest[j++] = 'r';
    }
    else
    {
      dest[j++] = c;
    }
  }
  dest[j] = '\0';
}

bool publishAck(PubSubClient *client, const char *message)
{
  if (client == nullptr || message == nullptr)
  {
    return false;
  }

  if (isJsonObjectMessage(message))
  {
    return client->publish(publish_Ack_Topic, message);
  }

  char escaped[256];
  appendEscapedJsonString(escaped, sizeof(escaped), message);

  char jsonMessage[320];
  snprintf(jsonMessage, sizeof(jsonMessage), "{\"message\":\"%s\"}", escaped);
  return client->publish(publish_Ack_Topic, jsonMessage);
}
