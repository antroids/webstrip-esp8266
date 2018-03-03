#ifndef IMPL_ESP_8266_FACTORY_H
#define IMPL_ESP_8266_FACTORY_H

#include "../WebStripFactory.h"
#include "ESP8266HTTPClient.h"
#include "ESP8266HTTPServer.h"

class ESP8266Factory : public WebStripFactory {
public:
  HTTPServer *buildHTTPServer(uint32_t port) { return new ESP8266HTTPServer(port); }
  WebStrip::HTTPClient *buildHTTPClient() { return new ESP8266HTTPClient(); }
};

#endif /* end of include guard: IMPL_ESP_8266_FACTORY_H */
