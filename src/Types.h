#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

#define VOID_METHOD_TO_FUNCTION(M) ([=]() { return this->M(); })

typedef std::function<bool(const char *)> ErrorCallbackFunctionType;

typedef uint16_t led_index_t;
typedef uint16_t index_id_t;

struct Context;
class LedStripMode;
class Animation;

#endif
