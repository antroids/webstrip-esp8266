#ifndef WEB_HTTP_CLIENT_H
#define WEB_HTTP_CLIENT_H

#include "../FileSystem.h"
#include "../Log.h"
#include "../Types.h"
#include "HTTPClientResponse.h"
#include "HTTPDefinitions.h"

namespace WebStrip {

class HTTPClient {
public:
  enum Method { GET, POST, PUT, PATCH, DELETE, OPTIONS };
  class RequestBuilder {
  public:
    virtual RequestBuilder *url(const char *url) = 0;
    virtual RequestBuilder *get() = 0;
    // now only get method required

    virtual HTTPClientResponse *sendRequest() = 0;
  };

  virtual RequestBuilder *getRequestBuilder() = 0;

  bool downloadResource(const char *url, const char *filePath, ErrorCallbackFunctionType errorCallback) {
    HTTPClientResponse *response = getRequestBuilder()->url(url)->get()->sendRequest();

    if (response->getCode() == HTTP::CODE_OK) {
      Log::mainLogger.infof("URL opened '%s' code is '%d' content length '%d'", url, response->getCode(), response->getContentLength());
      File file = SPIFFS.open(filePath, "w");
      Stream *fileStream = &file;
      Log::mainLogger.infof("File opened '%s'", filePath);
      response->writeToStream(fileStream);
      file.close();
    } else if (response->getCode() < 0) {
      response->close();
      return errorCallback(response->getErrorMessage());
    } else {
      char messageBuffer[256];
      sprintf(messageBuffer, "'%s' code is %d", url, response->getCode());
      response->close();
      return errorCallback(messageBuffer);
    }
    response->close();
    Log::mainLogger.infof("Downloaded from '%s' to '%s'", url, filePath);
    return true;
  }
};
}

#endif /* end of include guard: WEB_HTTP_CLIENT_H */
