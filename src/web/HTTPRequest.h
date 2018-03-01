#ifndef WEB_HTTP_REQUEST_H
#define WEB_HTTP_REQUEST_H

#include "../Types.h"

/**
 * Abstract HTTPRequest class
 */
class HTTPRequest {
public:
  /**
   * HTTP Methods
   */
  enum Method { GET, POST, PUT, PATCH, DELETE, OPTIONS, ANY };

  /**
   * Returns URI of this request
   * @return URI
   */
  virtual const char *getURI() = 0;
  /**
   * Returns HTTP Method of this request
   * @return HTTP Method
   */
  virtual HTTPRequest::Method getMethod() = 0;

  /**
   * Returns request argument by name
   * @param  name argument name
   * @return      argument value
   */
  virtual const char *getArg(const char *name) = 0;
  /**
   * True if this request contains argument with given name
   * @param  name argument name
   * @return      contains
   */
  virtual bool hasArg(const char *name) = 0;

  /**
   * Returns request header by name
   * @param  name header name
   * @return      arguheaderment value
   */
  virtual const char *getHeader(const char *name) = 0;
  /**
   * True if this request contains header with given name
   * @param  name header name
   * @return      contains
   */
  virtual bool hasHeader(const char *name) = 0;
};

#endif
