#ifndef ANIMATION_RAND_PIXELS_ANIMATION_H
#define ANIMATION_RAND_PIXELS_ANIMATION_H

#include "animation/Animation.h"

class RandPixelsAnimation : public Animation {
protected:
  void start() {
    generateColors();
    startTransitionAnimation();
  }

  void update(const AnimationParam &param) {
    if (param.state == AnimationState_Completed) {
      for (led_index_t i = 0; i < calcAnimationIntensity(); i++) {
        led_index_t ledIndex = random(pixelCount);

        if (!animations->IsAnimationActive(ledIndex)) {
          startUpdateLedColorChangeAnimation(ledIndex, calcAnimationTime() * 10, strip->getBufferColor(ledIndex), generateColor(ledIndex));
        }
      }
      restartMainAnimation();
    }
  }
};

#endif
