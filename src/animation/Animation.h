#ifndef ANIMATION_H
#define ANIMATION_H

#include "../BufferedNeoPixelBus.h"
#include "../Log.h"
#include "../Types.h"
#include "../domain/AnimationProgressMode.h"
#include "../domain/StaticIndex.h"
#include <NeoPixelAnimator.h>

struct LedColorAnimationState {
  RgbColor startColor;
  RgbColor endColor;
};

class Animation : public StaticIndex<Animation, 32> {
public:
  static Log logger;

  static NeoPixelAnimator *animations;
  static LedColorAnimationState *ledColorAnimationState;
  static BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip;
  static LedStripMode *mode;
  static led_index_t pixelCount;
  static RgbColor tempColor;

  static void init(BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, LedStripMode *mode);
  static index_id_t getMainAnimationIndex();
  static RgbColor generateColor(led_index_t ledIndex);
  static void generateColors();
  static float calcProgress(const AnimationParam &param);
  static void updateLedColorChangeAnimation(const AnimationParam &param);
  static void startUpdateLedColorChangeAnimation(led_index_t ledIndex, unsigned int duration);
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

  Animation() : StaticIndex(this){};
};

#include "../domain/ColorSelectionMode.h"
#include "../domain/LedStripMode.h"

#endif
