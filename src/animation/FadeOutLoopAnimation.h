#ifndef ANIMATION_FADE_OUT_LOOP_ANIMATION_H
#define ANIMATION_FADE_OUT_LOOP_ANIMATION_H

#include "animation/Animation.h"

class FadeOutLoopAnimation : public Animation {
protected:
  void update(const AnimationParam &param) {
    if (param.state == AnimationState_Completed) {
      restartMainAnimation();
    } else {
      float progress = calcProgress(param);
      led_index_t ledIndex = pixelCount * progress;
      ledIndex = mode->animationDirection ? ledIndex : pixelCount - ledIndex;

      if (!animations->IsAnimationActive(ledIndex)) {
        startUpdateLedColorChangeAnimation(ledIndex, calcAnimationTime() / 10, getColorForLedIndex(ledIndex), BLACK);
      }
    }
  }

  virtual RgbColor getColorForLedIndex(led_index_t ledIndex) { return generateColor(ledIndex); }

  ScaleDescriptor getAnimationSpeedScale() { return ScaleDescriptor(SECONDS_TO_ANIMATION_TIME(0.5), SECONDS_TO_ANIMATION_TIME(5)); }
};

#endif
