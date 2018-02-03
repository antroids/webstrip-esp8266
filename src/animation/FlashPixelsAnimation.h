#include "animation/Animation.h"

class FlashPixelsAnimation : public Animation {
  void update(const AnimationParam &param) {
    if (param.state == AnimationState_Completed) {
      for (uint16_t i = 0; i < mode->animationIntensity; i++) {
        uint16_t ledIndex = random(pixelCount);

        if (!animations->IsAnimationActive(ledIndex)) {
          ledColorAnimationState[ledIndex].startColor = generateColor(ledIndex);
          ledColorAnimationState[ledIndex].endColor = BLACK;
          startUpdateLedColorChangeAnimation(ledIndex, calcAnimationTime() * 2);
        }
      }
      restartMainAnimation();
    }
  }

  uint16_t getDuration() { return 20; }
};
