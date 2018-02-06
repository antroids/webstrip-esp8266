#include "animation/Animation.h"

class RandPixelsAnimation : public Animation {
  void start() {
    showGeneratedColors();
    startMainAnimation();
  }

  void update(const AnimationParam &param) {
    if (param.state == AnimationState_Completed) {
      for (led_index_t i = 0; i < mode->animationIntensity; i++) {
        led_index_t ledIndex = random(pixelCount);

        if (!animations->IsAnimationActive(ledIndex)) {
          ledColorAnimationState[ledIndex].startColor = strip->getBufferColor(ledIndex);
          ledColorAnimationState[ledIndex].endColor = generateColor(ledIndex);
          startUpdateLedColorChangeAnimation(ledIndex, calcAnimationTime() * 10);
        }
      }
      restartMainAnimation();
    }
  }

  uint16_t getDuration() { return 10; }
};
