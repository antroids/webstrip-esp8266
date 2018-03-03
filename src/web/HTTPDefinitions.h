#ifndef WEB_HTTP_DEFINITIONS_H
#define WEB_HTTP_DEFINITIONS_H

#include <Arduino.h>

typedef uint16_t http_code_t;

class HTTP {
public:
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

#endif /* end of include guard: WEB_HTTP_DEFINITIONS_H */
