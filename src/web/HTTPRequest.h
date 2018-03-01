#ifndef WEB_HTTP_REQUEST_H
#define WEB_HTTP_REQUEST_H

#include "../Types.h"

typedef uint16_t http_request_index_t;

class HTTPRequest {
public:
  enum Method { GET, POST, PUT, PATCH, DELETE, OPTIONS, ANY };

  virtual const char *getURI() = 0;
  virtual HTTPRequest::Method getMethod() = 0;

  virtual const char *getArg(const char *name) = 0;
  virtual bool hasArg(const char *name) = 0;

  virtual const char *getHeader(const char *name) = 0;
  virtual bool hasHeader(const char *name) = 0;
};

#endif
