#ifndef WEB_HTTP_RESPONSE_H
#define WEB_HTTP_RESPONSE_H

#include "../Types.h"
#include "../domain/JsonEntity.h"
#include <ArduinoJson.h>

typedef uint16_t http_code_t;

/**
 * Abstract HTTPResponse class
 */
class HTTPResponse {
public:
  /**
   * Sends char array response with given HTTP Code and Mime type
   * @param code    HTTP Code
   * @param mime    Mime type
   * @param content null-terminated char array
   */
  virtual void send(http_code_t code, const char *mime, const char *content) = 0;
  /**
   * Sends JsonObject converted to JSON string as response with given HTTP Code
   * @param code       HTTP Code
   * @param jsonObject JsonObject to send
   */
  virtual void send(http_code_t code, JsonObject *jsonObject) = 0;
  /**
   * Sends JsonEntity converted to JSON string as response with given HTTP Code
   * @param code       HTTP Code
   * @param jsonEntity JsonEntity to send
   */
  virtual void send(http_code_t code, JsonEntity *jsonEntity) = 0;

  /**
   * Is response already sent
   * @return response sent
   */
  virtual bool isSent() = 0;

  static const char *MIME_JS;
  static const char *MIME_CSS;
  static const char *MIME_HTML;
  static const char *MIME_JSON;
  static const char *MIME_TEXT;

  static const http_code_t CODE_OK;
  static const http_code_t CODE_NOT_FOUND;
  static const http_code_t CODE_SERVER_ERROR;
  static const http_code_t CODE_WRONG_REQUEST;
};

#endif
