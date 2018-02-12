#include "HTTPUpdater.h"

bool HTTPUpdater::saveFile(const char *url, const char *filePath, ErrorCallbackFunctionType errorCallback) {
  client.begin(url);
  Log::mainLogger.infof("URL opened %s", url);
  int httpCode = client.GET();
  bool result = true;
  if (httpCode == HTTP_CODE_OK) {
    int len = client.getSize();
    uint8_t buff[128] = {0};
    WiFiClient *stream = client.getStreamPtr();
    File file = SPIFFS.open(filePath, "w");
    Log::mainLogger.infof("File opened %s", filePath);
    while (client.connected() && (len > 0 || len == -1)) {
      size_t size = stream->available();
      if (size) {
        int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
        file.write(buff, c);
        if (len > 0) {
          len -= c;
        }
      }
      delay(1);
    }
    file.close();
  } else {
    char messageBuffer[256];
    sprintf(messageBuffer, "%s code is %d", url, httpCode);
    Log::mainLogger.err(messageBuffer);
    errorCallback(messageBuffer);
    result = false;
  }
  client.end();
  return result;
}
