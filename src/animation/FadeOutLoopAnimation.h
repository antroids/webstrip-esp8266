#include "animation/Animation.h"

class FadeOutLoopAnimation : public Animation {
  void update(const AnimationParam &param) {
    if (param.state == AnimationState_Completed) {
      restartMainAnimation();
    } else {
      float progress = calcProgress(param);
      uint16_t ledIndex = pixelCount * progress;
      ledIndex = mode->animationDirection ? ledIndex : pixelCount - ledIndex;

      if (!animations->IsAnimationActive(ledIndex)) {
        ledColorAnimationState[ledIndex].startColor = generateColor(ledIndex);
        ledColorAnimationState[ledIndex].endColor = BLACK;
        startUpdateLedColorChangeAnimation(ledIndex, calcAnimationTime() / 10);
      }
    }
  }

  uint16_t getDuration() { return 500; }
};
