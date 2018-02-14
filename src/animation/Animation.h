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
  static ScaleDescriptor inputScale;

  void showProgress(const RgbColor bgColor, const RgbColor doneColor, float progress);

  void processAnimation();
  virtual void start();
  virtual void stop();

  Animation(Context *_context) : StaticIndex(this), context(_context){};

protected:
  static Log logger;

  static RgbColor tempColor;
  static led_index_t tempLedIndex;

  const Context *context;

  index_id_t getMainAnimationIndex();
  RgbColor generateColor(led_index_t ledIndex);
  void generateColors();
  float calcProgress(const AnimationParam &param);
  void updateLedColorChangeAnimation(const AnimationParam &param);
  void startUpdateLedColorChangeAnimation(led_index_t ledIndex, animation_duration_t duration);
  void startUpdateLedColorChangeAnimation(led_index_t ledIndex, animation_duration_t duration, RgbColor fromColor, RgbColor toColor);
  uint16_t convertToScale(ScaleDescriptor inputScale, ScaleDescriptor outputScale, uint16_t value);

  LedStripMode *getMode();
  led_index_t getPixelCount();
  BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *getStrip();
  LedColorAnimationState *getLedColorAnimationState();
  NeoPixelAnimator *getAnimator();

  animation_duration_t calcAnimationTime();
  animation_duration_t calcAnimationIntensity();
  void startMainAnimation();
  void restartMainAnimation();
  void updateTransitionAnimation(const AnimationParam &param);
  void startTransitionAnimation();
  virtual void update(const AnimationParam &param);
  virtual ScaleDescriptor getAnimationSpeedScale();
  virtual ScaleDescriptor getAnimationIntensityScale();
};

#include "../Context.h"
#include "../domain/ColorSelectionMode.h"
#include "../domain/LedStripMode.h"

#endif
