#ifndef ANIMATION_H
#define ANIMATION_H

#include "../BufferedNeoPixelBus.h"
#include "../domain/AnimationProgressMode.h"
#include "../domain/Index.h"
#include <NeoPixelAnimator.h>

#define GENERATE_RANDOM_COLOR (HsbColor(((float)random(360)) / 360, 1, 0.5))

struct LedColorAnimationState {
  RgbColor startColor;
  RgbColor endColor;
};

class LedStripMode;

class Animation : public Index<Animation, 32> {
public:
  static NeoPixelAnimator *animations;
  static LedColorAnimationState *ledColorAnimationState;
  static BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
  static LedStripMode *mode;
  static uint16_t pixelCount;
  static RgbColor tempColor;

  static void init(BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, LedStripMode *mode);
  static uint16_t getMainAnimationIndex();
  static RgbColor generateColor(uint16_t ledIndex);
  static void generateColors();
  static float calcProgress(const AnimationParam &param);
  static void updateLedColorChangeAnimation(const AnimationParam &param);
  static void startUpdateLedColorChangeAnimation(uint16_t ledIndex, unsigned int duration);
  static void showGeneratedColors();
  static void showBlackColor();

  unsigned int calcAnimationTime();
  void processAnimation();
  void startMainAnimation();
  void restartMainAnimation();
  virtual void start();
  virtual void update(const AnimationParam &param);
  virtual void stop();
  virtual uint16_t getDuration();

  Animation() : Index(this){};
};

#include "../domain/LedStripMode.h"

#endif
