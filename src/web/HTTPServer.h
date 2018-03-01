#ifndef WEB_HTTP_SERVER_H
#define WEB_HTTP_SERVER_H

#include "HTTPRequest.h"
#include "HTTPResponse.h"
#include <functional>

#define METHOD_TO_HTTP_HANDLER(M) ([=](HTTPRequest *request, HTTPResponse *response) { return this->M(request, response); })

class HTTPServer : public UpdateHandler {
public:
  typedef std::function<void(HTTPRequest *, HTTPResponse *)> HTTPServerHandlerFunction;

  virtual void on(const char *uri, HTTPRequest::Method method, HTTPServerHandlerFunction handler) = 0;
  virtual void on(const char *uri, const char *mime, const char *filePath) = 0;
  virtual void on(const char *uri, HTTPServerHandlerFunction handler) = 0;
};

#endif
