#ifndef ANIMATION_FADE_ANIMATION_H
#define ANIMATION_FADE_ANIMATION_H

#include "animation/Animation.h"

class FadeAnimation : public Animation {
protected:
  void start() {
    generateColors();
    startTransitionAnimation();
  }

  void update(const AnimationParam &param) {
    float progress = calcProgress(param);
    strip->loadBufferColors(
        [](RgbColor color, led_index_t ledIndex, float progress) -> RgbColor {
          if (progress < 0.5) {
            return changeColorBrightness(color, 1 - progress * 2);
          } else {
            return changeColorBrightness(color, progress * 2 - 1);
          }
        },
        progress);
    if (param.state == AnimationState_Completed) {
      restartMainAnimation();
    }
  }
};

#endif
