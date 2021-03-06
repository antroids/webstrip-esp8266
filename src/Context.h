#ifndef CONTEXT_H
#define CONTEXT_H

#include "WebStripFactory.h"

#include "animation/Animation.h"
#include "api/JsonApi.h"
#include "domain/ColorSelectionMode.h"
#include "domain/LedStripMode.h"
#include "domain/Options.h"
#include "web/HTTPClient.h"

#include "animation/ExplosionsAnimation.h"
#include "animation/FadeAnimation.h"
#include "animation/FadeOutLoopAnimation.h"
#include "animation/FlashPixelsAnimation.h"
#include "animation/NoneAnimation.h"
#include "animation/RandPixelsAnimation.h"
#include "animation/ShiftAnimation.h"
#include "animation/SolidExplosionsAnimation.h"
#include "animation/SolidFadeOutLoopAnimation.h"

#include "FileSystem.h"

class Context {
public:
  WebStripFactory *factory;

  // Animations
  NeoPixelAnimator *animator;
  LedColorAnimationState *ledColorAnimationState;
  BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;

  LedStripMode *mode;
  Options *options;

  HTTPServer *httpServer;
  WebStrip::HTTPClient *httpClient = NULL;
  JsonApi *api;

  bool otaMode = false;
  bool autoModeChanging = true;
  unsigned long modeChangeTime = 0;

  Animation *animations[9] = {new NoneAnimation(this),        new ShiftAnimation(this),           new FadeAnimation(this),
                              new RandPixelsAnimation(this),  new FlashPixelsAnimation(this),     new SolidFadeOutLoopAnimation(this),
                              new FadeOutLoopAnimation(this), new SolidExplosionsAnimation(this), new ExplosionsAnimation(this)};

  AnimationProgressMode *animationProgressModes[4] = {new LinearAnimationProgressMode(), new SinInAnimationProgressMode(), new SinOutAnimationProgressMode(),
                                                      new SinInOutAnimationProgressMode()};

  ColorSelectionMode *colorSelectionModes[4] = {new AscPaletteColorSelectionMode(), new RandPaletteColorSelectionMode(), new RandColorSelectionMode(),
                                                new AscPaletteStretchColorSelectionMode()};

  Context(WebStripFactory *_factory, Options _options) {
    factory = _factory;
    options = new Options();
    options->pixelCount = _options.pixelCount;
    strncpy(options->domain, _options.domain, JSON_FIELD_OPTIONS_DOMAIN_SIZE);

    strip = new BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(options->pixelCount);
    strip->Begin();
    strip->Show();

    ledColorAnimationState = new LedColorAnimationState[options->pixelCount];
    animator = new NeoPixelAnimator(options->pixelCount + 1, ANIMATION_TIME_UNIT);

    mode = new LedStripMode();
    initDefaultMode();

    httpServer = factory->buildHTTPServer(options->port);
    httpClient = factory->buildHTTPClient();
    api = new JsonApi(this);
  }

  ~Context() {
    delete api;
    delete httpClient;
    delete httpServer;
    delete mode;
    delete animator;
    delete[] ledColorAnimationState;
    delete strip;
    delete options;
  }

  Animation *getCurrentAnimation() { return Animation::getFromIndex(mode->animationMode); }

protected:
  void initDefaultColors() {
    const char *defaultColors[] = DEFAULT_COLORS;
    HtmlColor htmlColor;

    for (int i = 0; i < DEFAULT_COLORS_COUNT; i++) {
      htmlColor.Parse<HtmlColorNames>(defaultColors[i]);
      mode->colors[i] = RgbColor(htmlColor);
    }
    mode->colorsCount = DEFAULT_COLORS_COUNT;
    Log::mainLogger.infof("Default '%d' colors loaded", mode->colorsCount);
  }

  void initDefaultMode() {
    String filepath = MODE_JSON_FILE_PATH(mode->index);
    if (!SPIFFS.exists(filepath)) {
      initDefaultColors();
      mode->setAnimationMode(0);
    } else {
      SPIFFS.loadJson(mode, filepath, Log::logErrorHandler);
    }
    modeChangeTime = millis();
  }
};

#endif
