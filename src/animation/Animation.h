#ifndef ANIMATION_H
#define ANIMATION_H

#include "../BufferedNeoPixelBus.h"
#include "../Log.h"
#include "../Types.h"
#include "../domain/AnimationProgressMode.h"
#include "../domain/StaticIndex.h"
#include <NeoPixelAnimator.h>

#define TRANSITION_ANIMATION_DURATION 50

#define ANIMATION_TIME_UNIT NEO_CENTISECONDS
#define SECONDS_TO_ANIMATION_TIME(VALUE) (VALUE * 1000 / ANIMATION_TIME_UNIT)
#define MILLISECONDS_TO_ANIMATION_TIME(VALUE) (VALUE * ANIMATION_TIME_UNIT)

struct LedColorAnimationState {
  RgbColor startColor;
  RgbColor endColor;
};

typedef double (*ScaleModificationFunction)(const double value);
typedef uint16_t animation_duration_t;

struct ScaleDescriptor {
  uint16_t min;
  uint16_t max;
  ScaleModificationFunction modifier = [=](const double value) { return value; };

  ScaleDescriptor(uint16_t _min, uint16_t _max) : min(_min), max(_max){};
  ScaleDescriptor(uint16_t _min, uint16_t _max, ScaleModificationFunction _modifier) : min(_min), max(_max), modifier(_modifier){};
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
  static led_index_t tempLedIndex;
  static ScaleDescriptor inputScale;

  static void init(BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, LedStripMode *mode);
  static index_id_t getMainAnimationIndex();
  static RgbColor generateColor(led_index_t ledIndex);
  static void generateColors();
  static float calcProgress(const AnimationParam &param);
  static void updateLedColorChangeAnimation(const AnimationParam &param);
  static void startUpdateLedColorChangeAnimation(led_index_t ledIndex, animation_duration_t duration);
  static void startUpdateLedColorChangeAnimation(led_index_t ledIndex, animation_duration_t duration, RgbColor fromColor, RgbColor toColor);
  static uint16_t convertToScale(ScaleDescriptor inputScale, ScaleDescriptor outputScale, uint16_t value);

  animation_duration_t calcAnimationTime();
  animation_duration_t calcAnimationIntensity();
  void processAnimation();
  void startMainAnimation();
  void restartMainAnimation();
  void updateTransitionAnimation(const AnimationParam &param);
  void startTransitionAnimation();
  virtual void start();
  virtual void update(const AnimationParam &param);
  virtual void stop();
  virtual ScaleDescriptor getAnimationSpeedScale();
  virtual ScaleDescriptor getAnimationIntensityScale();

  Animation() : StaticIndex(this){};
};

#include "../domain/ColorSelectionMode.h"
#include "../domain/LedStripMode.h"

#endif
