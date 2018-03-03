#include "../Context.h"
#include "../update/FirmwareUpdater.h"
#include "../update/WebClientUpdater.h"
#include "../web/HTTPServer.h"
#include "JsonApi.h"

const char *JsonApi::WEB_CLIENT_HTML_FILE_PATH = "/web/index.html";
const char *JsonApi::WEB_CLIENT_JS_FILE_PATH = "/web/static/js/main.js.gz";
const char *JsonApi::WEB_CLIENT_CSS_FILE_PATH = "/web/static/css/main.css.gz";

void JsonApi::handleUpdate() { server->handleUpdate(); }

void JsonApi::initUrlMappings() {
  server->on("/", HTTP::MIME_HTML, WEB_CLIENT_HTML_FILE_PATH);
  server->on("/static/js/main.js.gz", HTTP::MIME_JS, WEB_CLIENT_JS_FILE_PATH);
  server->on("/static/css/main.css.gz", HTTP::MIME_CSS, WEB_CLIENT_CSS_FILE_PATH);
  server->on("/static/js/main.js", HTTP::MIME_JS, WEB_CLIENT_JS_FILE_PATH);
  server->on("/static/css/main.css", HTTP::MIME_CSS, WEB_CLIENT_CSS_FILE_PATH);
  server->on("/api/mode", HTTPRequest::Method::POST, METHOD_TO_HTTP_HANDLER(onModePost));
  server->on("/api/mode", HTTPRequest::Method::GET, METHOD_TO_HTTP_HANDLER(onModeGet));
  server->on("/api/saveMode", HTTPRequest::Method::GET, METHOD_TO_HTTP_HANDLER(onSaveModeGet));
  server->on("/api/loadMode", HTTPRequest::Method::GET, METHOD_TO_HTTP_HANDLER(onLoadModeGet));
  server->on("/api/listModes", HTTPRequest::Method::GET, METHOD_TO_HTTP_HANDLER(onListModesGet));
  server->on("/api/options", HTTPRequest::Method::POST, METHOD_TO_HTTP_HANDLER(onOptionsPost));
  server->on("/api/options", HTTPRequest::Method::GET, METHOD_TO_HTTP_HANDLER(onOptionsGet));
  server->on("/api/otaUpdate", METHOD_TO_HTTP_HANDLER(onOtaUpdate));
  server->on("/api/webClientUpdate", METHOD_TO_HTTP_HANDLER(onWebClientUpdate));
  server->on("/api/firmwareUpdate", METHOD_TO_HTTP_HANDLER(onFirmwareUpdate));
  server->on("/api/sysInfo", METHOD_TO_HTTP_HANDLER(onSysInfoGet));
  // server->onNotFound(VOID_METHOD_TO_FUNCTION(handleNotFound));
}

void JsonApi::handleNotFound() {
  // String message = "File Not Found\n\n";
  // message += "URI: ";
  // message += server->uri();
  // message += "\nMethod: ";
  // message += (server->method() == HTTP_GET) ? "GET" : "POST";
  // message += "\nArguments: ";
  // message += server->args();
  // message += "\n";
  // for (uint8_t i = 0; i < server->args(); i++) {
  //   message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
  // }
  // server->send(404, "text/plain", message);
}

void JsonApi::initWebServer() {
  server = context->httpServer;
  initUrlMappings();
}

void JsonApi::sendError(HTTPResponse *response, const char *message, int httpCode) {
  Error error(message);
  Log::mainLogger.err(message);
  response->send(httpCode, &error);
}

ErrorCallbackFunctionType JsonApi::getRequestErrorHandler(HTTPResponse *response) {
  return ([=](const char *errorMessage) -> bool { return this->requestErrorHandler(response, errorMessage); });
}

bool JsonApi::requestErrorHandler(HTTPResponse *response, const char *errorMessage) {
  sendError(response, errorMessage, HTTP::CODE_WRONG_REQUEST);
  Log::mainLogger.err(errorMessage);
  return false;
}

void JsonApi::onModeGet(HTTPRequest *request, HTTPResponse *response) {
  response->send(HTTP::CODE_OK, context->mode);
  context->autoModeChanging = false;
}

void JsonApi::onModePost(HTTPRequest *request, HTTPResponse *response) {
  if (!request->hasArg(ARG_JSON)) {
    sendError(response, "Json not found", HTTP::CODE_WRONG_REQUEST);
    return;
  }
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.parseObject(request->getArg(ARG_JSON));
  if (context->mode->updateEntityFromJson(json, getRequestErrorHandler(response))) {
    context->autoModeChanging = false;
    onModeGet(request, response);
  }
}

// Creates new config for given Index or overrides existing
void JsonApi::onSaveModeGet(HTTPRequest *request, HTTPResponse *response) {
  if (!request->hasArg(ARG_INDEX)) {
    sendError(response, "Index argument not found", HTTP::CODE_WRONG_REQUEST);
    return;
  }
  context->mode->index = String(request->getArg(ARG_INDEX)).toInt();
  String filepath = MODE_JSON_FILE_PATH(context->mode->index);
  if (SPIFFS.saveJson(context->mode, filepath, getRequestErrorHandler(response))) {
    onModeGet(request, response);
  }
}

void JsonApi::onLoadModeGet(HTTPRequest *request, HTTPResponse *response) {
  if (!request->hasArg(ARG_INDEX)) {
    sendError(response, "Index argument not found", HTTP::CODE_WRONG_REQUEST);
    return;
  }
  context->mode->index = String(request->getArg(ARG_INDEX)).toInt();

  DynamicJsonBuffer jsonBuffer;

  if (loadModeFromFS(context->mode->index, getRequestErrorHandler(response))) {
    onSaveModeGet(request, response);
  }
}

bool JsonApi::loadModeFromFS(index_id_t index, ErrorCallbackFunctionType errorCallback) {
  String filepath = MODE_JSON_FILE_PATH(index);
  if (!SPIFFS.exists(filepath)) {
    return errorCallback("Saved mode not found");
  }
  if (SPIFFS.loadJson(context->mode, filepath, errorCallback)) {
    context->mode->index = index;
    return true;
  }
  return false;
}

JsonObject &JsonApi::loadJsonFromFS(DynamicJsonBuffer *jsonBuffer, String path, ErrorCallbackFunctionType errorCallback) {
  File jsonFile = SPIFFS.open(path, "r");
  JsonObject &json = jsonBuffer->parseObject(jsonFile);
  jsonFile.close();
  return json;
}

void JsonApi::onListModesGet(HTTPRequest *request, HTTPResponse *response) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &jsonResponse = jsonBuffer.createObject();
  JsonArray &modes = jsonResponse.createNestedArray("modes");
  char descriptionsBuffer[MODE_INDEX_MAX + 1][MODE_DESCRIPTION_SIZE];

  for (uint8_t i = 0; i < MODE_INDEX_MAX; i++) {
    String filepath = MODE_JSON_FILE_PATH(i);
    if (SPIFFS.exists(filepath)) {
      DynamicJsonBuffer loadModeBuffer;
      JsonObject &json = loadJsonFromFS(&loadModeBuffer, filepath, getRequestErrorHandler(response));
      if (json == JsonObject::invalid()) {
        return;
      }
      JsonObject &mode = modes.createNestedObject();
      mode[JSON_FIELD_MODE_INDEX] = i;
      String description = json[JSON_FIELD_MODE_DESCRIPTION];
      description.toCharArray(descriptionsBuffer[i], MODE_DESCRIPTION_SIZE);
      mode[JSON_FIELD_MODE_DESCRIPTION] = descriptionsBuffer[i];
    }
  }
  response->send(HTTP::CODE_OK, &jsonResponse);
}

void JsonApi::onOptionsGet(HTTPRequest *request, HTTPResponse *response) { response->send(HTTP::CODE_OK, context->options); }

void JsonApi::onOptionsPost(HTTPRequest *request, HTTPResponse *response) {
  if (!request->hasArg(ARG_JSON)) {
    sendError(response, "Json not found", HTTP::CODE_WRONG_REQUEST);
    return;
  }
  DynamicJsonBuffer jsonBuffer;
  JsonObject &requestJson = jsonBuffer.parseObject(request->getArg(ARG_JSON));
  if (context->options->updateEntityFromJson(requestJson, getRequestErrorHandler(response)) &&
      context->options->updateJsonFromEntity(requestJson, getRequestErrorHandler(response)) &&
      SPIFFS.saveJson(context->options, OPTIONS_JSON_FILE_PATH, getRequestErrorHandler(response))) {
    onOptionsGet(request, response);
    delay(1000);
    Log::mainLogger.info("Restarting...");
    WiFi.forceSleepBegin();
    ESP.reset();
  }
}

void JsonApi::onOtaUpdate(HTTPRequest *request, HTTPResponse *response) {
  ArduinoOTA.setHostname(context->options->domain);
  ArduinoOTA.begin();
  context->otaMode = true;
  response->send(HTTP::CODE_OK, HTTP::MIME_TEXT, "Waiting for OTA update\n");
}

void JsonApi::onWebClientUpdate(HTTPRequest *request, HTTPResponse *response) {
  WebClientUpdater updater(context->httpClient, [=](const uint8_t status, float progress) {
    Log::mainLogger.infof("Update status %d progress %f", status, progress);
    context->getCurrentAnimation()->showProgress(PROGRESS_BG, PROGRESS_DONE, progress);
  });
  if (request->hasArg("update") && ((bool)request->getArg("update"))) {
    if (updater.startUpdate(getRequestErrorHandler(response))) {
      response->send(HTTP::CODE_OK, HTTP::MIME_TEXT, "Web client updated\n");
    }
    context->mode->setAnimationMode(context->mode->animationMode);
  } else {
    UpdaterVersionInfo versionInfo = updater.getVersionInfo(getRequestErrorHandler(response));
    if (versionInfo != UpdaterVersionInfo::invalid) {
      DynamicJsonBuffer jsonBuffer;
      JsonObject &json = jsonBuffer.createObject();
      if (versionInfo.updateJsonFromEntity(json, getRequestErrorHandler(response))) {
        response->send(HTTP::CODE_OK, &json);
      }
    }
  }
}

void JsonApi::onFirmwareUpdate(HTTPRequest *request, HTTPResponse *response) {
  FirmwareUpdater updater(context->httpClient, [=](const uint8_t status, float progress) {
    Log::mainLogger.infof("Update status %d progress %f", status, progress);
    context->getCurrentAnimation()->showProgress(PROGRESS_BG, PROGRESS_DONE, progress);
    if (status == UPDATER_STATUS_END) {
      response->send(HTTP::CODE_OK, HTTP::MIME_TEXT, "Firmware updated\n");
    }
  });
  if (request->hasArg("update") && ((bool)request->getArg("update"))) {
    if (updater.startUpdate(getRequestErrorHandler(response))) {
      response->send(HTTP::CODE_OK, HTTP::MIME_TEXT, "Firmware updated\n");
      ESP.restart(); // add abstraction layer
    }
  } else {
    Log::mainLogger.info("Firmware version info request");
    UpdaterVersionInfo versionInfo = updater.getVersionInfo(getRequestErrorHandler(response));
    if (versionInfo != UpdaterVersionInfo::invalid) {
      DynamicJsonBuffer jsonBuffer;
      JsonObject &json = jsonBuffer.createObject();
      if (versionInfo.updateJsonFromEntity(json, getRequestErrorHandler(response))) {
        response->send(HTTP::CODE_OK, &json);
      }
    }
  }
}

void JsonApi::onSysInfoGet(HTTPRequest *request, HTTPResponse *response) {
  Esp8266SysInfo sysInfo;
  DynamicJsonBuffer jsonBuffer;
  JsonObject &responseJson = jsonBuffer.createObject();

  if (sysInfo.updateJsonFromEntity(responseJson, getRequestErrorHandler(response))) {
    response->send(HTTP::CODE_OK, &responseJson);
  }
}
