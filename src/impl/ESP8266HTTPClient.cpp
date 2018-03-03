#include "ESP8266HTTPClient.h"

uint16_t ESP8266HTTPClientResponse::getCode() { return code; }

int ESP8266HTTPClientResponse::getContentLength() { return client->clientImpl.getSize(); }

Stream *ESP8266HTTPClientResponse::getContentStream() { return client->clientImpl.getStreamPtr(); }

int ESP8266HTTPClientResponse::writeToStream(Stream *stream) { return client->clientImpl.writeToStream(stream); }

bool ESP8266HTTPClientResponse::isConnected() { return client->clientImpl.connected(); }

void ESP8266HTTPClientResponse::close() { client->clientImpl.end(); }

const char *ESP8266HTTPClientResponse::getErrorMessage() { return errorMessage; }

ESP8266HTTPClient::RequestBuilder *ESP8266HTTPClient::ESP8266HTTPRequestBuilder::url(const char *url) {
  _url = url;
  return this;
}
ESP8266HTTPClient::RequestBuilder *ESP8266HTTPClient::ESP8266HTTPRequestBuilder::get() {
  method = GET;
  return this;
}
bool ESP8266HTTPClient::ESP8266HTTPRequestBuilder::validate() {
  if (_url == NULL) {
    response->code = ESP_8266_HTTP_CLIENT_VALIDATION_ERROR;
    strcpy(response->errorMessage, "URL required!");
    return false;
  }
}

HTTPClientResponse *ESP8266HTTPClient::ESP8266HTTPRequestBuilder::sendRequest() {
  if (!validate()) {
    return response;
  }

  client->clientImpl.begin(_url);
  int code;
  switch (method) {
  case GET:
  default:
    code = client->clientImpl.GET();
    break;
  }
  response->code = code;

  if (code < 0) {
    if (code != ESP_8266_HTTP_CLIENT_VALIDATION_ERROR) { // errorMessage already filled in validate method
      String msg = ::HTTPClient::errorToString(code);
      msg.toCharArray(response->errorMessage, ESP_8266_HTTP_CLIENT_RESPONSE_MAX_ERROR_SIZE);
    }
  } else {
    response->errorMessage[0] = 0;
  }

  return response;
}

void ESP8266HTTPClient::ESP8266HTTPRequestBuilder::clear() {
  method = GET;
  _url = NULL;
}

ESP8266HTTPClient::RequestBuilder *ESP8266HTTPClient::getRequestBuilder() {
  builder.clear();
  return &builder;
}
