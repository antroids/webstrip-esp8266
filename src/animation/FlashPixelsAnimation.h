#include "animation/Animation.h"

class FlashPixelsAnimation : public Animation {
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
