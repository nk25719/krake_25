#ifndef DEBUG_MACROS_H
#define DEBUG_MACROS_H

#include <Arduino.h>

#ifndef DEBUG_LEVEL
#define DEBUG_LEVEL 0
#endif

#ifndef GPAD_DEBUG
#define GPAD_DEBUG DEBUG_LEVEL
#endif

#if (GPAD_DEBUG > 0)
#define DBG_PRINT(x) Serial.print(x)
#define DBG_PRINTLN(x) Serial.println(x)
#define DBG_PRINTF(...) Serial.printf(__VA_ARGS__)
#else
#define DBG_PRINT(x) do { } while (0)
#define DBG_PRINTLN(x) do { } while (0)
#define DBG_PRINTF(...) do { } while (0)
#endif

#endif
