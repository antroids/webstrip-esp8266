#include <Arduino.h>

#include "impl/ESP8266Factory.h"

#include "FileSystem.h"
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <DNSServer.h>
#include <ESP8266WiFi.h> //https://github.com/esp8266/Arduino
#include <ESP8266mDNS.h>
#include <WiFiManager.h> //https://github.com/tzapu/WiFiManager

#include "Log.h"
#include "Types.h"

#include "animation/Animation.h"

#include "Context.h"
#include "domain/Options.h"

#define DOMAIN_POSTFIX ".local"

// Initialized after reading saved options
Context *context;

void initOTA() {
  ArduinoOTA.setHostname(context->options->domain);
  ArduinoOTA.onStart([]() {
    Log::mainLogger.info("OTA update started");
    if (ArduinoOTA.getCommand() == U_SPIFFS) {
      SPIFFS.end();
    }
    context->strip->loadBufferColors([](RgbColor color, led_index_t ledIndex, float progress) { return YELLOW; }, 0);
    context->strip->Show();
  });
  ArduinoOTA.onEnd([]() {
    Log::mainLogger.info("OTA update completed");
    context->strip->loadBufferColors([](RgbColor color, led_index_t ledIndex, float progress) { return BLUE; }, 0);
    context->strip->Show();
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    float p = ((float)progress) / total;
    Log::mainLogger.infof("OTA update progress %u / %u", progress, total);
    context->getCurrentAnimation()->showProgress(PROGRESS_BG, PROGRESS_DONE, progress);
  });
}

void initMDNS() {
  uint16_t attempts = 3;
  while (attempts-- > 0) {
    if (!MDNS.begin(context->options->domain)) {
      Log::mainLogger.err("Error setting up MDNS responder!");
    } else {
      Log::mainLogger.infof("MDNS configured for domain '%s'. Attempts: %d", context->options->domain, attempts);
      return;
    }
  }
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

void setup() {
  Serial.begin(115200);
  Log::mainLogger.info("Serial started");
  SetRandomSeed();
  SPIFFS.begin();
  Log::mainLogger.info("SPIFFS started");
  Options options;
  SPIFFS.exists(OPTIONS_JSON_FILE_PATH) && SPIFFS.loadJson(&options, OPTIONS_JSON_FILE_PATH, Log::logErrorHandler);
  // Print root dir content

  // Dir dir = SPIFFS.openDir("/web/");
  // Serial.println("");
  // while (dir.next()) {
  //   Serial.print(dir.fileName());
  //   File f = dir.openFile("r");
  //   Serial.println(f.size());
  // }
  // network
  WiFiManager wifiManager;
  wifiManager.autoConnect(options.domain);
  Log::mainLogger.info("WiFiManager setup complete");
  context = new Context(new ESP8266Factory(), options);
  initOTA();
  Log::mainLogger.info("OTA setup completed");
  initMDNS();
  Log::mainLogger.info("MDNS started");
}

void loop() {
  MDNS.update();
  if (context->otaMode) {
    ArduinoOTA.handle();
  } else {
    if (context->autoModeChanging && context->mode->nextModeDelay > 0 && (millis() - context->modeChangeTime) > context->mode->nextModeDelay &&
        context->mode->nextMode != context->mode->index) {
      SPIFFS.loadJson(context->mode, MODE_JSON_FILE_PATH(context->mode->nextMode), Log::logErrorHandler);
      context->modeChangeTime = millis();
    }
    context->api->handleUpdate();
    context->getCurrentAnimation()->processAnimation();
  }
}
