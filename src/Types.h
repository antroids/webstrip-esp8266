#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

typedef bool (*ErrorCallbackFunctionType)(const char *errorMessage);

typedef uint16_t led_index_t;
typedef uint16_t index_id_t;

class LedStripMode;
class Animation;

#endif
