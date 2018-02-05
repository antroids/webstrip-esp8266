#include <Arduino.h>

#include "FS.h"
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <ESP8266mDNS.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager

#include "Types.h"

#include "animation/Animation.h"
#include "animation/FadeAnimation.h"
#include "animation/FadeOutLoopAnimation.h"
#include "animation/FlashPixelsAnimation.h"
#include "animation/NoneAnimation.h"
#include "animation/RandPixelsAnimation.h"
#include "animation/ShiftAnimation.h"
#include "animation/SolidFadeOutLoopAnimation.h"

#include "domain/Options.h"

#define MODE_JSON_FILE_PATH(INDEX) (String("/modes/mode") + String(INDEX) + String(".json"))
#define OPTIONS_JSON_FILE_PATH "/web/options.json"
#define INDEX_HTML_FILE_PATH "/web/index.html"
#define INDEX_MIN_JS_GZ_FILE_PATH "/web/index.min.js.gz"

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
#define HTML_COLOR_LENGTH 10

#define DOMAIN_POSTFIX ".local"

Animation *animations[] = {new NoneAnimation(),       new ShiftAnimation(),       new FadeAnimation(),
                           new RandPixelsAnimation(), new FlashPixelsAnimation(), new SolidFadeOutLoopAnimation(),
                           new FadeOutLoopAnimation()};

AnimationProgressMode *animationProgressModes[] = {new LinearAnimationProgressMode(), new SinInAnimationProgressMode(), new SinOutAnimationProgressMode(),
                                                   new SinInOutAnimationProgressMode()};

ColorSelectionMode *colorSelectionModes[] = {new AscPaletteColorSelectionMode(), new RandPaletteColorSelectionMode(), new RandColorSelectionMode(),
                                             new AscPaletteStretchColorSelectionMode()};

Options currentOptions;
LedStripMode currentMode;

bool otaMode = false;
bool ledStripModeEditMode = true;

// temp values can be used in animations
RgbColor tempColor;
uint16_t tempLedIndex = 0;

// Initialized after reading saved options
ESP8266WebServer *server;
BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
Animation *currentAnimation = Animation::getFromIndex(0);

bool validateRange(JsonObject &json, const char *fieldName, int min, int max, ErrorCallbackFunctionType errorCallback);
void SetRandomSeed();
void setLedStripAnimationMode(const uint16_t prevLedStripAnimationMode, const uint16_t newLedStripAnimationMode);
void handleRoot();
void sendJson(JsonObject &json, const int httpCode);
void sendError(const char *message, int httpCode);
void setupResponseHeaders();
void handleNotFound();
void onOtaUpdate();
JsonObject &loadJsonFromFS(DynamicJsonBuffer *jsonBuffer, String path, ErrorCallbackFunctionType errorCallback);
bool saveJsonToFS(JsonObject &json, String path, ErrorCallbackFunctionType errorCallback);
bool loadOptionsFromFS();
void onOptionsPost();
void onOptionsGet();
void onFileGet();
void onFilePost();
void onModeGet();
void onModePost();
void onIndexMinJsGz();
void onRoot();
bool logErrorHandler(const char *errorMessage);
bool requestErrorHandler(const char *errorMessage);
void setupUrlMappings();
void initDefaultColors();
void initDefaultMode();
void initMDNS();
void initWebServer();
void initOTA();
void initAnimations();
void initLedStrip();
void log(const char *message);
void log(String message);
void restart();
void loop();
void setup();

void setup() {
  Serial.begin(115200);
  SetRandomSeed();
  SPIFFS.begin();
  // Print root dir content

  // Dir dir = SPIFFS.openDir("/web/");
  // Serial.println("");
  // while (dir.next()) {
  //   Serial.print(dir.fileName());
  //   File f = dir.openFile("r");
  //   Serial.println(f.size());
  // }
  if (!loadOptionsFromFS()) {
    log("Cannot load options from file, using predefined values");
  }

  // network
  WiFiManager wifiManager;
  wifiManager.autoConnect(currentOptions.domain);
  initOTA();
  initMDNS();
  initWebServer();

  // strip
  initLedStrip();
  initAnimations();
  initDefaultMode();
}

void loop() {
  MDNS.update();
  if (otaMode) {
    ArduinoOTA.handle();
  } else {
    server->handleClient();
    currentAnimation->processAnimation();
  }
}

void restart() {
  WiFi.forceSleepBegin();
  ESP.reset();
}

void log(String message) { Serial.println(message); }

void log(const char *message) { Serial.println(message); }

void initLedStrip() {
  strip = new BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(currentOptions.pixelCount);
  strip->Begin();
  strip->Show();
}

void initAnimations() { Animation::init(strip, &currentMode); }

void initOTA() {
  ArduinoOTA.setHostname(currentOptions.domain);
  ArduinoOTA.onStart([]() {
    if (ArduinoOTA.getCommand() == U_SPIFFS) {
      SPIFFS.end();
    }
    strip->loadBufferColors([](RgbColor color, uint16_t ledIndex, float progress) { return YELLOW; }, 0);
    strip->Show();
  });
  ArduinoOTA.onEnd([]() {
    strip->loadBufferColors([](RgbColor color, uint16_t ledIndex, float progress) { return BLUE; }, 0);
    strip->Show();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    float p = ((float)progress) / total;
    strip->loadBufferColors(
        [](RgbColor color, uint16_t ledIndex, float p) {
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

void initWebServer() {
  server = new ESP8266WebServer(currentOptions.port);
  setupUrlMappings();
  server->begin();
}

void initMDNS() {
  uint16_t attempts = 3;
  while (attempts-- > 0) {
    if (!MDNS.begin(currentOptions.domain)) {
      log("Error setting up MDNS responder!");
    } else {
      return;
    }
  }
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
}

void initDefaultColors() {
  const char *defaultColors[] = DEFAULT_COLORS;
  HtmlColor htmlColor;

  for (int i = 0; i < DEFAULT_COLORS_COUNT; i++) {
    htmlColor.Parse<HtmlColorNames>(defaultColors[i]);
    currentMode.colors[i] = RgbColor(htmlColor);
  }
  currentMode.colorsCount = DEFAULT_COLORS_COUNT;
}

void setupUrlMappings() {
  server->on("/", onRoot);
  server->on("/index.min.js.gz", onIndexMinJsGz);
  server->on("/api/mode", HTTP_POST, onModePost);
  server->on("/api/mode", HTTP_GET, onModeGet);
  server->on("/api/file", HTTP_POST, onFilePost);
  server->on("/api/file", HTTP_GET, onFileGet);
  server->on("/api/options", HTTP_POST, onOptionsPost);
  server->on("/api/options", HTTP_GET, onOptionsGet);
  server->on("/api/otaUpdate", onOtaUpdate);
  server->onNotFound(handleNotFound);
}

bool requestErrorHandler(const char *errorMessage) {
  sendError(errorMessage, HTTP_CODE_WRONG_REQUEST);
  log(errorMessage);
  return false;
}

bool logErrorHandler(const char *errorMessage) {
  log(errorMessage);
  return false;
}

void onRoot() {
  if (!SPIFFS.exists(INDEX_HTML_FILE_PATH)) {
    sendError("File not found on server!", HTTP_CODE_SERVER_ERROR);
  }
  File file = SPIFFS.open(INDEX_HTML_FILE_PATH, "r");
  server->streamFile(file, MIME_HTML);
  file.close();
}

void onIndexMinJsGz() {
  if (!SPIFFS.exists(INDEX_MIN_JS_GZ_FILE_PATH)) {
    sendError("File not found on server!", HTTP_CODE_SERVER_ERROR);
  }
  File file = SPIFFS.open(INDEX_MIN_JS_GZ_FILE_PATH, "r");
  server->streamFile(file, MIME_JS);
  file.close();
}

void onModePost() {
  if (!server->hasArg(ARG_JSON)) {
    sendError("Json not found", HTTP_CODE_WRONG_REQUEST);
    return;
  }
  DynamicJsonBuffer jsonBuffer;
  JsonObject &request = jsonBuffer.parseObject(server->arg(ARG_JSON));
  uint16_t previousAnimationMode = currentMode.animationMode;
  if (currentMode.updateEntityFromJson(request, requestErrorHandler)) {
    setLedStripAnimationMode(previousAnimationMode, currentMode.animationMode);
    onModeGet();
  }
}

void onModeGet() {
  DynamicJsonBuffer jsonBuffer;
  JsonObject &response = jsonBuffer.createObject();
  if (currentMode.updateJsonFromEntity(response, requestErrorHandler)) {
    sendJson(response, HTTP_CODE_OK);
  }
}

// Creates new config for given Index or overrides existing
void onFilePost() {
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

void onFileGet() {
  if (!server->hasArg(ARG_INDEX)) {
    sendError("Index argument not found", HTTP_CODE_WRONG_REQUEST);
    return;
  }
  currentMode.index = server->arg(ARG_INDEX).toInt();
  String filepath = MODE_JSON_FILE_PATH(currentMode.index);
  if (!SPIFFS.exists(filepath)) {
    sendError("Saved mode not found", HTTP_CODE_NOT_FOUND);
    return;
  }
  DynamicJsonBuffer jsonBuffer;

  JsonObject &json = loadJsonFromFS(&jsonBuffer, filepath, requestErrorHandler);
  uint16_t prevAnimationMode = currentMode.animationMode;
  if (json != JsonObject::invalid() && currentMode.updateEntityFromJson(json, requestErrorHandler)) {
    setLedStripAnimationMode(prevAnimationMode, currentMode.animationMode);
    sendJson(json, HTTP_CODE_OK);
  }
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

void onOtaUpdate() {
  ArduinoOTA.setHostname("WebStripOTA");
  ArduinoOTA.begin();
  otaMode = true;
  server->send(HTTP_CODE_OK, "text/plain", "Waiting for OTA update");
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
  sendJson(response, httpCode);
}

void sendJson(JsonObject &json, const int httpCode) {
  char jsonCharBuffer[512];
  json.printTo(jsonCharBuffer);
  setupResponseHeaders();
  server->send(httpCode, MIME_JSON, jsonCharBuffer);
}

void setLedStripAnimationMode(const uint16_t prevLedStripAnimationMode, const uint16_t newLedStripAnimationMode) {
  Animation::getFromIndex(prevLedStripAnimationMode)->stop();
  Animation::getFromIndex(newLedStripAnimationMode)->start();
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
