#ifndef WEB_HTTP_CLIENT_RESPONSE
#define WEB_HTTP_CLIENT_RESPONSE

#include <Arduino.h>

class HTTPClientResponse {
public:
  /**
   * HTTP Code, less than 0 are HTTPClient errors
   * @return code
   */
  virtual uint16_t getCode() = 0;
  /**
   * Content-Length header value.
   * 0 - content empty.
   * less than 0 - header not found
   * @return length in OCTETS = 8 bits
   */
  virtual int getContentLength() = 0;
  /**
   * Returns stream for response content
   * @return stream
   */
  virtual Stream *getContentStream() = 0;
  /**
   * Writes response content to stream
   * @param  stream stream
   * @return        bytes written, negative are errors
   */
  virtual int writeToStream(Stream *stream) = 0;

  virtual bool isConnected() = 0;

  virtual void close() = 0;

  virtual const char *getErrorMessage() = 0;
};

#endif /* end of include guard: WEB_HTTP_CLIENT_RESPONSE */
