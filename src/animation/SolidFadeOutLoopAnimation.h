#ifndef ANIMATION_SOLID_FADE_OUT_LOOP_ANIMATION_H
#define ANIMATION_SOLID_FADE_OUT_LOOP_ANIMATION_H

#include "FadeOutLoopAnimation.h"
#include "animation/Animation.h"

class SolidFadeOutLoopAnimation : public FadeOutLoopAnimation {
public:
  SolidFadeOutLoopAnimation(Context *_context) : FadeOutLoopAnimation(_context){};

protected:
  void update(const AnimationParam &param) {
    if (param.state == AnimationState_Started) {
      tempColor = generateColor(0);
    }
    FadeOutLoopAnimation::update(param);
  }

  virtual RgbColor getColorForLedIndex(led_index_t ledIndex) { return tempColor; }

  ScaleDescriptor getAnimationSpeedScale() { return ScaleDescriptor(SECONDS_TO_ANIMATION_TIME(0.5), SECONDS_TO_ANIMATION_TIME(5)); }
};

#endif
