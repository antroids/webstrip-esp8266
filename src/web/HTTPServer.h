#ifndef WEB_HTTP_SERVER_H
#define WEB_HTTP_SERVER_H

#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include <functional>

/**
 * Converts instance method to HTTPServerHandlerFunction
 * @param  M method name
 * @return   HTTPServerHandlerFunction
 */
#define METHOD_TO_HTTP_HANDLER(M) ([=](HTTPRequest *request, HTTPResponse *response) { return this->M(request, response); })

/**
 * Abstract HTTP Server class
 */
class HTTPServer : public UpdateHandler {
public:
  typedef std::function<void(HTTPRequest *, HTTPResponse *)> HTTPServerHandlerFunction;

  /**
   * Adds Request handler for specified URI and Method
   * @param uri     URI, only exact match supported
   * @param method  HTTP method
   * @param handler handler function
   */
  virtual void on(const char *uri, HTTPRequest::Method method, HTTPServerHandlerFunction handler) = 0;
  /**
   * Adds Request handler for specified URI and HTTP GET method.
   * Content of filePath will be used as response with given Mime type.
   * @param uri      URI, only exact match supported
   * @param mime     Mime type, commonly used listed in HTTPResponse
   * @param filePath path to file in file system
   */
  virtual void on(const char *uri, const char *mime, const char *filePath) = 0;
  /**
   * Adds Request handler for specified URI and any Method
   * @param uri     URI, only exact match supported
   * @param handler handler function
   */
  virtual void on(const char *uri, HTTPServerHandlerFunction handler) = 0;
};

#endif
