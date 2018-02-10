#ifndef ANIMATION_FLASH_PIXELS_ANIMATION_H
#define ANIMATION_FLASH_PIXELS_ANIMATION_H

#include "animation/Animation.h"

class FlashPixelsAnimation : public Animation {
protected:
  void update(const AnimationParam &param) {
    if (param.state == AnimationState_Completed) {
      for (led_index_t i = 0; i < calcAnimationIntensity(); i++) {
        led_index_t ledIndex = random(pixelCount);

        if (!animations->IsAnimationActive(ledIndex)) {
          ledColorAnimationState[ledIndex].startColor = generateColor(ledIndex);
          ledColorAnimationState[ledIndex].endColor = BLACK;
          startUpdateLedColorChangeAnimation(ledIndex, calcAnimationTime() * 2);
        }
      }
      restartMainAnimation();
    }
  }
};

#endif
