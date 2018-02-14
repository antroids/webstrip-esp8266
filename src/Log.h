#ifndef LOG_H
#define LOG_H

#include <Arduino.h>
#include <stdarg.h>

#define LOG_BUFFER_SIZE 256

#ifndef LOG_DEFAULT_LEVEL
#define LOG_DEFAULT_LEVEL INFO
#endif

class Log {
public:
  static const uint8_t INFO = 2;
  static const uint8_t WARN = 1;
  static const uint8_t ERR = 0;

  static const char *INFO_NAME;
  static const char *WARN_NAME;
  static const char *ERR_NAME;

  static Log mainLogger;

  uint8_t _level = LOG_DEFAULT_LEVEL;
  const char *_name;

  Log(const char *name) : _name(name){};

  const char *getLevelName(uint8_t level) {
    switch (level) {
    case INFO:
      return INFO_NAME;
    case WARN:
      return WARN_NAME;
    case ERR:
    default:
      return ERR_NAME;
    }
  }

  void log(const char *message) { Serial.println(message); }
  void log(const char *message, uint8_t level) {
    if (!isLogEnabled(level)) {
      return;
    }
    char buf[LOG_BUFFER_SIZE];
    sprintf(buf, "[%s] %.5lu : %s", getLevelName(level), millis(), message);
    log(message);
  }
  void info(const char *message) { log(message, INFO); }
  void warn(const char *message) { log(message, WARN); }
  void err(const char *message) { log(message, ERR); }

  void infof(const char *tpl, ...) {
    if (!isLogEnabled(INFO)) {
      return;
    }
    va_list vl;
    va_start(vl, tpl);
    vlogf(tpl, INFO, vl);
    va_end(vl);
  }
  void warnf(const char *tpl, ...) {
    if (!isLogEnabled(WARN)) {
      return;
    }
    va_list vl;
    va_start(vl, tpl);
    vlogf(tpl, WARN, vl);
    va_end(vl);
  }
  void errf(const char *tpl, ...) {
    if (!isLogEnabled(ERR)) {
      return;
    }
    va_list vl;
    va_start(vl, tpl);
    vlogf(tpl, ERR, vl);
    va_end(vl);
  }

  static bool logErrorHandler(const char *errorMessage) {
    Log::mainLogger.err(errorMessage);
    return false;
  }

protected:
  void vlogf(const char *tpl, uint8_t level, va_list args) {
    char buf[LOG_BUFFER_SIZE];
    vsprintf(buf, tpl, args);
    log(buf, level);
  }
  bool isLogEnabled(uint8_t level) { return _level >= level; }
};

#endif
