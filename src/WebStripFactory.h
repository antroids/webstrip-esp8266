#ifndef WEB_STRIP_FACTORY_H
#define WEB_STRIP_FACTORY_H

#include "web/HTTPClient.h"
#include "web/HTTPServer.h"

class WebStripFactory {
public:
  virtual HTTPServer *buildHTTPServer(uint32_t port) = 0;
  virtual WebStrip::HTTPClient *buildHTTPClient() = 0;
};

#endif /* end of include guard: WEB_STRIP_FACTORY_H                                                                                                            \
 */
