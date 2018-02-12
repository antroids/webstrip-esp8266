#include <Arduino.h>

#include "FileSystem.h"
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <ESP8266mDNS.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager

#include "Log.h"
#include "Types.h"

#include "animation/Animation.h"
#include "animation/ExplosionsAnimation.h"
#include "animation/FadeAnimation.h"
#include "animation/FadeOutLoopAnimation.h"
#include "animation/FlashPixelsAnimation.h"
#include "animation/NoneAnimation.h"
#include "animation/RandPixelsAnimation.h"
#include "animation/ShiftAnimation.h"
#include "animation/SolidExplosionsAnimation.h"
#include "animation/SolidFadeOutLoopAnimation.h"

#include "domain/Esp8266SysInfo.h"
#include "domain/Options.h"

#include "update/WebClientUpdater.h"

#define MODE_JSON_FILE_PATH(INDEX) (String("/modes/mode") + String(INDEX) + String(".json"))
#define OPTIONS_JSON_FILE_PATH "/web/options.json"

#define MIME_JSON "application/json"
#define MIME_HTML "text/html"
#define MIME_JS "text/javascript"
#define MIME_CSS "text/css"
#define HTTP_CODE_OK 200
#define HTTP_CODE_WRONG_REQUEST 400
#define HTTP_CODE_NOT_FOUND 404
#define HTTP_CODE_SERVER_ERROR 500

#define ARG_JSON "data"
#define ARG_INDEX "index"
#define ARG_AUTO_MODE_CHANGING "autoModeChanging"

#define DOMAIN_POSTFIX ".local"

Animation *animations[] = {new NoneAnimation(),        new ShiftAnimation(),           new FadeAnimation(),
                           new RandPixelsAnimation(),  new FlashPixelsAnimation(),     new SolidFadeOutLoopAnimation(),
                           new FadeOutLoopAnimation(), new SolidExplosionsAnimation(), new ExplosionsAnimation()};

AnimationProgressMode *animationProgressModes[] = {new LinearAnimationProgressMode(), new SinInAnimationProgressMode(), new SinOutAnimationProgressMode(),
                                                   new SinInOutAnimationProgressMode()};

ColorSelectionMode *colorSelectionModes[] = {new AscPaletteColorSelectionMode(), new RandPaletteColorSelectionMode(), new RandColorSelectionMode(),
                                             new AscPaletteStretchColorSelectionMode()};

Options currentOptions;
LedStripMode currentMode;

bool otaMode = false;
bool autoModeChanging = true;
unsigned long modeChangeTime = 0;

// Initialized after reading saved options
ESP8266WebServer *server;
HTTPClient *client;
BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
Animation *currentAnimation = Animation::getFromIndex(0);

bool validateRange(JsonObject &json, const char *fieldName, int min, int max, ErrorCallbackFunctionType errorCallback);
void SetRandomSeed();
void setLedStripAnimationMode(const index_id_t prevLedStripAnimationMode, const index_id_t newLedStripAnimationMode);
void sendJson(JsonObject &json, const int httpCode);
void sendError(const char *message, int httpCode);
void setupResponseHeaders();
JsonObject &loadJsonFromFS(DynamicJsonBuffer *jsonBuffer, String path, ErrorCallbackFunctionType errorCallback);
bool saveJsonToFS(JsonObject &json, String path, ErrorCallbackFunctionType errorCallback);
bool loadOptionsFromFS();
bool loadModeFromFS(index_id_t index, ErrorCallbackFunctionType errorCallback);
bool logErrorHandler(const char *errorMessage);
bool requestErrorHandler(const char *errorMessage);
void restart();

void restart() {
  Log::mainLogger.info("Restarting...");
  WiFi.forceSleepBegin();
  ESP.reset();
}

void initLedStrip() {
  strip = new BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(currentOptions.pixelCount);
  strip->Begin();
  strip->Show();
}

void initAnimations() { Animation::init(strip, &currentMode); }

void initOTA() {
  ArduinoOTA.setHostname(currentOptions.domain);
  ArduinoOTA.onStart([]() {
    Log::mainLogger.info("OTA update started");
    if (ArduinoOTA.getCommand() == U_SPIFFS) {
      SPIFFS.end();
    }
    strip->loadBufferColors([](RgbColor color, led_index_t ledIndex, float progress) { return YELLOW; }, 0);
    strip->Show();
  });
  ArduinoOTA.onEnd([]() {
    Log::mainLogger.info("OTA update completed");
    strip->loadBufferColors([](RgbColor color, led_index_t ledIndex, float progress) { return BLUE; }, 0);
    strip->Show();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    float p = ((float)progress) / total;
    Log::mainLogger.infof("OTA update progress %u / %u", progress, total);
    strip->loadBufferColors(
        [](RgbColor color, led_index_t ledIndex, float p) {
          if (ledIndex < currentOptions.pixelCount * p) {
            return GREEN;
          } else {
            return YELLOW;
          }
        },
        p);
    strip->Show();
  });
}

void initMDNS() {
  uint16_t attempts = 3;
  while (attempts-- > 0) {
    if (!MDNS.begin(currentOptions.domain)) {
      Log::mainLogger.err("Error setting up MDNS responder!");
    } else {
      Log::mainLogger.infof("MDNS configured for domain '%s'. Attempts: %d", currentOptions.domain, attempts);
      return;
    }
  }
}

void initDefaultColors() {
  const char *defaultColors[] = DEFAULT_COLORS;
  HtmlColor htmlColor;

  for (int i = 0; i < DEFAULT_COLORS_COUNT; i++) {
    htmlColor.Parse<HtmlColorNames>(defaultColors[i]);
    currentMode.colors[i] = RgbColor(htmlColor);
  }
  currentMode.colorsCount = DEFAULT_COLORS_COUNT;
  Log::mainLogger.infof("Default '%d' colors loaded", currentMode.colorsCount);
}

void initDefaultMode() {
  String filepath = MODE_JSON_FILE_PATH(currentMode.index);
  if (!SPIFFS.exists(filepath)) {
    initDefaultColors();
    setLedStripAnimationMode(0, 0);
  } else {
    DynamicJsonBuffer request;
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = loadJsonFromFS(&jsonBuffer, filepath, requestErrorHandler);
    if (json != JsonObject::invalid() && currentMode.updateEntityFromJson(json, requestErrorHandler)) {
      setLedStripAnimationMode(0, currentMode.animationMode);
    }
  }
  modeChangeTime = millis();
}

bool requestErrorHandler(const char *errorMessage) {
  sendError(errorMessage, HTTP_CODE_WRONG_REQUEST);
  Log::mainLogger.err(errorMessage);
  return false;
}

bool logErrorHandler(const char *errorMessage) {
  Log::mainLogger.err(errorMessage);
  return false;
}

bool loadOptionsFromFS() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = loadJsonFromFS(&jsonBuffer, OPTIONS_JSON_FILE_PATH, logErrorHandler);
  return json != JsonObject::invalid() && currentOptions.updateEntityFromJson(json, logErrorHandler);
}

bool saveJsonToFS(JsonObject &json, String path, ErrorCallbackFunctionType errorCallback) {
  File jsonFile = SPIFFS.open(path, "w");
  json.printTo(jsonFile);
  jsonFile.close();
  return true;
}

JsonObject &loadJsonFromFS(DynamicJsonBuffer *jsonBuffer, String path, ErrorCallbackFunctionType errorCallback) {
  File jsonFile = SPIFFS.open(path, "r");
  JsonObject &json = jsonBuffer->parseObject(jsonFile);
  jsonFile.close();
  return json;
}

bool loadModeFromFS(index_id_t index, ErrorCallbackFunctionType errorCallback) {
  String filepath = MODE_JSON_FILE_PATH(index);
  if (!SPIFFS.exists(filepath)) {
    return errorCallback("Saved mode not found");
  }
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = loadJsonFromFS(&jsonBuffer, filepath, errorCallback);
  index_id_t prevAnimationMode = currentMode.animationMode;
  if (json != JsonObject::invalid() && currentMode.updateEntityFromJson(json, requestErrorHandler)) {
    currentMode.index = index;
    setLedStripAnimationMode(prevAnimationMode, currentMode.animationMode);
    return true;
  }
  return false;
}

void handleNotFound() {
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

void setupResponseHeaders() { server->sendHeader("Access-Control-Allow-Origin", "*"); }

void sendError(const char *message, int httpCode) {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &response = jsonBuffer.createObject();
  response["errorMessage"] = message;
  Log::mainLogger.err(message);
  sendJson(response, httpCode);
}

void sendJson(JsonObject &json, const int httpCode) {
  char jsonCharBuffer[512];
  json.printTo(jsonCharBuffer);
  setupResponseHeaders();
  Log::mainLogger.info("Sending JSON response");
  server->send(httpCode, MIME_JSON, jsonCharBuffer);
}

void setLedStripAnimationMode(const index_id_t prevLedStripAnimationMode, const index_id_t newLedStripAnimationMode) {
  Log::mainLogger.infof("Changing animation mode from '%d' to '%d'", prevLedStripAnimationMode, newLedStripAnimationMode);
  Animation::getFromIndex(prevLedStripAnimationMode)->stop();
  Log::mainLogger.info("Stopping previous animation mode");
  Animation::getFromIndex(newLedStripAnimationMode)->start();
  Log::mainLogger.info("Starting next animation mode");
  currentAnimation = Animation::getFromIndex(newLedStripAnimationMode);
}

void SetRandomSeed() {
  uint32_t seed;
  seed = analogRead(0);
  delay(1);
  for (int shifts = 3; shifts < 31; shifts += 3) {
    seed ^= analogRead(0) << shifts;
    delay(1);
  }
  randomSeed(seed);
}

void onRoot() {
  if (!SPIFFS.exists(WEB_CLIENT_HTML_FILE)) {
    sendError("File not found on server!", HTTP_CODE_SERVER_ERROR);
  }
  File file = SPIFFS.open(WEB_CLIENT_HTML_FILE, "r");
  server->streamFile(file, MIME_HTML);
  file.close();
}

void onIndexJsGz() {
  if (!SPIFFS.exists(WEB_CLIENT_JS_FILE)) {
    sendError("File not found on server!", HTTP_CODE_SERVER_ERROR);
  }
  File file = SPIFFS.open(WEB_CLIENT_JS_FILE, "r");
  server->streamFile(file, MIME_JS);
  file.close();
}

void onIndexCssGz() {
  if (!SPIFFS.exists(WEB_CLIENT_CSS_FILE)) {
    sendError("File not found on server!", HTTP_CODE_SERVER_ERROR);
  }
  File file = SPIFFS.open(WEB_CLIENT_CSS_FILE, "r");
  server->streamFile(file, MIME_CSS);
  file.close();
}

void onModeGet() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &response = jsonBuffer.createObject();
  if (currentMode.updateJsonFromEntity(response, requestErrorHandler)) {
    autoModeChanging = false;
    sendJson(response, HTTP_CODE_OK);
  }
}

void onModePost() {
  if (!server->hasArg(ARG_JSON)) {
    sendError("Json not found", HTTP_CODE_WRONG_REQUEST);
    return;
  }
  DynamicJsonBuffer jsonBuffer;
  JsonObject &request = jsonBuffer.parseObject(server->arg(ARG_JSON));
  index_id_t previousAnimationMode = currentMode.animationMode;
  if (currentMode.updateEntityFromJson(request, requestErrorHandler)) {
    setLedStripAnimationMode(previousAnimationMode, currentMode.animationMode);
    autoModeChanging = false;
    onModeGet();
  }
}

// Creates new config for given Index or overrides existing
void onSaveModeGet() {
  if (!server->hasArg(ARG_INDEX)) {
    sendError("Index argument not found", HTTP_CODE_WRONG_REQUEST);
    return;
  }
  currentMode.index = server->arg(ARG_INDEX).toInt();
  char filePathBuf[32];
  String filepath = MODE_JSON_FILE_PATH(currentMode.index);
  DynamicJsonBuffer jsonBuffer;
  JsonObject &modeJson = jsonBuffer.createObject();

  if (currentMode.updateJsonFromEntity(modeJson, requestErrorHandler) && saveJsonToFS(modeJson, filepath, requestErrorHandler)) {
    sendJson(modeJson, HTTP_CODE_OK);
  }
}

void onLoadModeGet() {
  if (!server->hasArg(ARG_INDEX)) {
    sendError("Index argument not found", HTTP_CODE_WRONG_REQUEST);
    return;
  }
  currentMode.index = server->arg(ARG_INDEX).toInt();

  DynamicJsonBuffer jsonBuffer;

  if (loadModeFromFS(currentMode.index, requestErrorHandler)) {
    onSaveModeGet();
  }
}

void onListModesGet() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &response = jsonBuffer.createObject();
  JsonArray &modes = response.createNestedArray("modes");
  char descriptionsBuffer[MODE_INDEX_MAX + 1][MODE_DESCRIPTION_SIZE];

  for (uint8_t i = 0; i < MODE_INDEX_MAX; i++) {
    String filepath = MODE_JSON_FILE_PATH(i);
    if (SPIFFS.exists(filepath)) {
      DynamicJsonBuffer loadModeBuffer;
      JsonObject &json = loadJsonFromFS(&loadModeBuffer, filepath, requestErrorHandler);
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
  sendJson(response, HTTP_CODE_OK);
}

void onOptionsGet() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &response = jsonBuffer.createObject();
  if (currentOptions.updateJsonFromEntity(response, requestErrorHandler)) {
    sendJson(response, HTTP_CODE_OK);
  }
}

void onOptionsPost() {
  if (!server->hasArg(ARG_JSON)) {
    sendError("Json not found", HTTP_CODE_WRONG_REQUEST);
    return;
  }
  DynamicJsonBuffer jsonBuffer;
  JsonObject &request = jsonBuffer.parseObject(server->arg(ARG_JSON));
  if (currentOptions.updateEntityFromJson(request, requestErrorHandler) && currentOptions.updateJsonFromEntity(request, requestErrorHandler) &&
      saveJsonToFS(request, OPTIONS_JSON_FILE_PATH, requestErrorHandler)) {
    onOptionsGet();
    delay(1000);
    restart();
  }
}

void onOtaUpdate() {
  ArduinoOTA.setHostname(currentOptions.domain);
  ArduinoOTA.begin();
  otaMode = true;
  server->send(HTTP_CODE_OK, "text/plain", "Waiting for OTA update\n");
}

void onWebClientUpdate() {
  WebClientUpdater updater([](const uint8_t status, float progress) { Log::mainLogger.infof("Update status %d progress %f", status, progress); });
  if (updater.startUpdate(requestErrorHandler)) {
    server->send(HTTP_CODE_OK, "text/plain", "Web client updated\n");
  }
}

void onSysInfoGet() {
  Esp8266SysInfo sysInfo;
  DynamicJsonBuffer jsonBuffer;
  JsonObject &response = jsonBuffer.createObject();

  if (sysInfo.updateJsonFromEntity(response, requestErrorHandler)) {
    sendJson(response, HTTP_CODE_OK);
  }
}

void setupUrlMappings() {
  server->on("/", onRoot);
  server->on("/static/js/main.js.gz", onIndexJsGz);
  server->on("/static/css/main.css.gz", onIndexCssGz);
  server->on("/static/js/main.js", onIndexJsGz);
  server->on("/static/css/main.css", onIndexCssGz);
  server->on("/api/mode", HTTP_POST, onModePost);
  server->on("/api/mode", HTTP_GET, onModeGet);
  server->on("/api/saveMode", HTTP_GET, onSaveModeGet);
  server->on("/api/loadMode", HTTP_GET, onLoadModeGet);
  server->on("/api/listModes", HTTP_GET, onListModesGet);
  server->on("/api/options", HTTP_POST, onOptionsPost);
  server->on("/api/options", HTTP_GET, onOptionsGet);
  server->on("/api/otaUpdate", onOtaUpdate);
  server->on("/api/webClientUpdate", onWebClientUpdate);
  server->on("/api/sysInfo", onSysInfoGet);
  server->onNotFound(handleNotFound);
}

void initWebServer() {
  server = new ESP8266WebServer(currentOptions.port);
  client = new HTTPClient();
  setupUrlMappings();
  server->begin();
}

void setup() {
  Serial.begin(115200);
  Log::mainLogger.info("Serial started");
  SetRandomSeed();
  SPIFFS.begin();
  Log::mainLogger.info("SPIFFS started");
  // Print root dir content

  // Dir dir = SPIFFS.openDir("/web/");
  // Serial.println("");
  // while (dir.next()) {
  //   Serial.print(dir.fileName());
  //   File f = dir.openFile("r");
  //   Serial.println(f.size());
  // }
  if (!loadOptionsFromFS()) {
    Log::mainLogger.err("Cannot load options from file, using predefined values");
  }

  // network
  WiFiManager wifiManager;
  wifiManager.autoConnect(currentOptions.domain);
  Log::mainLogger.info("WiFiManager setup complete");
  initOTA();
  Log::mainLogger.info("OTA setup completed");
  initMDNS();
  Log::mainLogger.info("MDNS started");
  initWebServer();
  Log::mainLogger.info("Web server started");

  // strip
  initLedStrip();
  Log::mainLogger.info("Led strip initialized");
  initAnimations();
  Log::mainLogger.info("Animations initialized");
  initDefaultMode();
  Log::mainLogger.info("Default mode loaded");
}

void loop() {
  MDNS.update();
  if (otaMode) {
    ArduinoOTA.handle();
  } else {
    if (autoModeChanging && currentMode.nextModeDelay > 0 && (millis() - modeChangeTime) > currentMode.nextModeDelay &&
        currentMode.nextMode != currentMode.index) {
      loadModeFromFS(currentMode.nextMode, logErrorHandler);
      modeChangeTime = millis();
    }
    server->handleClient();
    currentAnimation->processAnimation();
  }
}
