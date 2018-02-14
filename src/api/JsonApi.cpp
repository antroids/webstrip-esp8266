#include "JsonApi.h"

void JsonApi::initUrlMappings() {
  server->on("/", VOID_METHOD_TO_FUNCTION(onRoot));
  server->on("/static/js/main.js.gz", VOID_METHOD_TO_FUNCTION(onIndexJsGz));
  server->on("/static/css/main.css.gz", VOID_METHOD_TO_FUNCTION(onIndexCssGz));
  server->on("/static/js/main.js", VOID_METHOD_TO_FUNCTION(onIndexJsGz));
  server->on("/static/css/main.css", VOID_METHOD_TO_FUNCTION(onIndexCssGz));
  server->on("/api/mode", HTTP_POST, VOID_METHOD_TO_FUNCTION(onModePost));
  server->on("/api/mode", HTTP_GET, VOID_METHOD_TO_FUNCTION(onModeGet));
  server->on("/api/saveMode", HTTP_GET, VOID_METHOD_TO_FUNCTION(onSaveModeGet));
  server->on("/api/loadMode", HTTP_GET, VOID_METHOD_TO_FUNCTION(onLoadModeGet));
  server->on("/api/listModes", HTTP_GET, VOID_METHOD_TO_FUNCTION(onListModesGet));
  server->on("/api/options", HTTP_POST, VOID_METHOD_TO_FUNCTION(onOptionsPost));
  server->on("/api/options", HTTP_GET, VOID_METHOD_TO_FUNCTION(onOptionsGet));
  server->on("/api/otaUpdate", VOID_METHOD_TO_FUNCTION(onOtaUpdate));
  server->on("/api/webClientUpdate", VOID_METHOD_TO_FUNCTION(onWebClientUpdate));
  server->on("/api/sysInfo", VOID_METHOD_TO_FUNCTION(onSysInfoGet));
  server->onNotFound(VOID_METHOD_TO_FUNCTION(handleNotFound));
}

void JsonApi::handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server->uri();
  message += "\nMethod: ";
  message += (server->method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server->args();
  message += "\n";
  for (uint8_t i = 0; i < server->args(); i++) {
    message += " " + server->argName(i) + ": " + server->arg(i) + "\n";
  }
  server->send(404, "text/plain", message);
}

void JsonApi::initWebServer() {
  server = new WebServer(context->options->port);
  initUrlMappings();
  server->begin();
}

void JsonApi::sendError(const char *message, int httpCode) {
  Error error(message);
  Log::mainLogger.err(message);
  server->send(httpCode, &error);
}

ErrorCallbackFunctionType JsonApi::getRequestErrorHandler() {
  return ([=](const char *errorMessage) -> bool { return this->requestErrorHandler(errorMessage); });
}

bool JsonApi::requestErrorHandler(const char *errorMessage) {
  sendError(errorMessage, HTTP_CODE_WRONG_REQUEST);
  Log::mainLogger.err(errorMessage);
  return false;
}

void JsonApi::onRoot() {
  if (!SPIFFS.exists(WEB_CLIENT_HTML_FILE)) {
    sendError("File not found on server!", HTTP_CODE_SERVER_ERROR);
  }
  File file = SPIFFS.open(WEB_CLIENT_HTML_FILE, "r");
  server->streamFile(file, MIME_HTML);
  file.close();
}

void JsonApi::onIndexJsGz() {
  if (!SPIFFS.exists(WEB_CLIENT_JS_FILE)) {
    sendError("File not found on server!", HTTP_CODE_SERVER_ERROR);
  }
  File file = SPIFFS.open(WEB_CLIENT_JS_FILE, "r");
  server->streamFile(file, MIME_JS);
  file.close();
}

void JsonApi::onIndexCssGz() {
  if (!SPIFFS.exists(WEB_CLIENT_CSS_FILE)) {
    sendError("File not found on server!", HTTP_CODE_SERVER_ERROR);
  }
  File file = SPIFFS.open(WEB_CLIENT_CSS_FILE, "r");
  server->streamFile(file, MIME_CSS);
  file.close();
}

void JsonApi::onModeGet() {
  server->send(HTTP_CODE_OK, context->mode, getRequestErrorHandler());
  context->autoModeChanging = false;
}

void JsonApi::onModePost() {
  if (!server->hasArg(ARG_JSON)) {
    sendError("Json not found", HTTP_CODE_WRONG_REQUEST);
    return;
  }
  DynamicJsonBuffer jsonBuffer;
  JsonObject &request = jsonBuffer.parseObject(server->arg(ARG_JSON));
  if (context->mode->updateEntityFromJson(request, getRequestErrorHandler())) {
    context->autoModeChanging = false;
    onModeGet();
  }
}

// Creates new config for given Index or overrides existing
void JsonApi::onSaveModeGet() {
  if (!server->hasArg(ARG_INDEX)) {
    sendError("Index argument not found", HTTP_CODE_WRONG_REQUEST);
    return;
  }
  context->mode->index = server->arg(ARG_INDEX).toInt();
  String filepath = MODE_JSON_FILE_PATH(context->mode->index);
  if (SPIFFS.saveJson(context->mode, filepath, getRequestErrorHandler())) {
    onModeGet();
  }
}

void JsonApi::onLoadModeGet() {
  if (!server->hasArg(ARG_INDEX)) {
    sendError("Index argument not found", HTTP_CODE_WRONG_REQUEST);
    return;
  }
  context->mode->index = server->arg(ARG_INDEX).toInt();

  DynamicJsonBuffer jsonBuffer;

  if (loadModeFromFS(context->mode->index, getRequestErrorHandler())) {
    onSaveModeGet();
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

void JsonApi::onListModesGet() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &response = jsonBuffer.createObject();
  JsonArray &modes = response.createNestedArray("modes");
  char descriptionsBuffer[MODE_INDEX_MAX + 1][MODE_DESCRIPTION_SIZE];

  for (uint8_t i = 0; i < MODE_INDEX_MAX; i++) {
    String filepath = MODE_JSON_FILE_PATH(i);
    if (SPIFFS.exists(filepath)) {
      DynamicJsonBuffer loadModeBuffer;
      JsonObject &json = loadJsonFromFS(&loadModeBuffer, filepath, getRequestErrorHandler());
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
  server->send(HTTP_CODE_OK, &response);
}

void JsonApi::onOptionsGet() { server->send(HTTP_CODE_OK, context->options, getRequestErrorHandler()); }

void JsonApi::onOptionsPost() {
  if (!server->hasArg(ARG_JSON)) {
    sendError("Json not found", HTTP_CODE_WRONG_REQUEST);
    return;
  }
  DynamicJsonBuffer jsonBuffer;
  JsonObject &request = jsonBuffer.parseObject(server->arg(ARG_JSON));
  if (context->options->updateEntityFromJson(request, getRequestErrorHandler()) && context->options->updateJsonFromEntity(request, getRequestErrorHandler()) &&
      SPIFFS.saveJson(context->options, OPTIONS_JSON_FILE_PATH, getRequestErrorHandler())) {
    onOptionsGet();
    delay(1000);
    Log::mainLogger.info("Restarting...");
    WiFi.forceSleepBegin();
    ESP.reset();
  }
}

void JsonApi::onOtaUpdate() {
  ArduinoOTA.setHostname(context->options->domain);
  ArduinoOTA.begin();
  context->otaMode = true;
  server->send(HTTP_CODE_OK, "text/plain", "Waiting for OTA update\n");
}

void JsonApi::onWebClientUpdate() {
  WebClientUpdater updater([=](const uint8_t status, float progress) {
    Log::mainLogger.infof("Update status %d progress %f", status, progress);
    context->getCurrentAnimation()->showProgress(PROGRESS_BG, PROGRESS_DONE, progress);
  });
  if (server->hasArg("update") && ((bool)server->arg("update"))) {
    if (updater.startUpdate(getRequestErrorHandler())) {
      server->send(HTTP_CODE_OK, "text/plain", "Web client updated\n");
    }
    context->mode->setAnimationMode(context->mode->animationMode);
  } else {
    UpdaterVersionInfo versionInfo = updater.getVersionInfo(getRequestErrorHandler());
    if (versionInfo != UpdaterVersionInfo::invalid) {
      DynamicJsonBuffer jsonBuffer;
      JsonObject &json = jsonBuffer.createObject();
      if (versionInfo.updateJsonFromEntity(json, getRequestErrorHandler())) {
        server->send(HTTP_CODE_OK, &json);
      }
    }
  }
}

void JsonApi::onSysInfoGet() {
  Esp8266SysInfo sysInfo;
  DynamicJsonBuffer jsonBuffer;
  JsonObject &response = jsonBuffer.createObject();

  if (sysInfo.updateJsonFromEntity(response, getRequestErrorHandler())) {
    server->send(HTTP_CODE_OK, &response);
  }
}
