#ifndef ANIMATION_ANIMATION_PROGRESS_MODE_H
#define ANIMATION_ANIMATION_PROGRESS_MODE_H

#include "../Types.h"
#include "StaticIndex.h"
#include <NeoPixelAnimator.h>

class AnimationProgressMode : public StaticIndex<AnimationProgressMode, 32> {
public:
  virtual float calcProgress(float progress) { return progress; };
  AnimationProgressMode() : StaticIndex(this){};
};

class LinearAnimationProgressMode : public AnimationProgressMode {};

class SinInAnimationProgressMode : public AnimationProgressMode {
  float calcProgress(float progress) { return NeoEase::SinusoidalIn(progress); };
};

class SinOutAnimationProgressMode : public AnimationProgressMode {
  float calcProgress(float progress) { return NeoEase::SinusoidalOut(progress); };
};

class SinInOutAnimationProgressMode : public AnimationProgressMode {
  float calcProgress(float progress) { return NeoEase::SinusoidalInOut(progress); };
};

#endif
