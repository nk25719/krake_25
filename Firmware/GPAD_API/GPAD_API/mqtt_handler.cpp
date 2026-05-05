#include "mqtt_handler.h"
#include <stdio.h>
#include <string.h>

extern char publish_Ack_Topic[];

static bool isJsonObjectMessage(const char *message)
{
  const size_t len = strlen(message);
  if (len < 2)
  {
    return false;
  }

  size_t start = 0;
  while (start < len && (message[start] == ' ' || message[start] == '\n' || message[start] == '\r' || message[start] == '\t'))
  {
    ++start;
  }

  if (start >= len || message[start] != '{')
  {
    return false;
  }

  size_t end = len;
  while (end > start && (message[end - 1] == ' ' || message[end - 1] == '\n' || message[end - 1] == '\r' || message[end - 1] == '\t'))
  {
    --end;
  }

  return end > start && message[end - 1] == '}';
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
    else if (c == '\t' && j + 2 < destSize)
    {
      dest[j++] = '\\';
      dest[j++] = 't';
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
