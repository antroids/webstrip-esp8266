#ifndef TYPES_H
#define TYPES_H

#include <Arduino.h>

#define VOID_METHOD_TO_FUNCTION(M) ([=]() { return this->M(); })

// Error callback function bool onError(const char *errorMessage)
// Return value should be used as result of failed function
// If return value of function is false, then operation failed and error handled
//   else function executed successfully
typedef std::function<bool(const char *)> ErrorCallbackFunctionType;

typedef std::function<void(float)> ProgressListenerFunctionType;

typedef uint16_t led_index_t;
typedef uint16_t index_id_t;

struct Context;
class LedStripMode;
class Animation;

class UpdateHandler {
public:
  virtual void handleUpdate() = 0;
};

#endif
