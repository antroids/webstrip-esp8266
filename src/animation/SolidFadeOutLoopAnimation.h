#include "animation/Animation.h"

class SolidFadeOutLoopAnimation : public Animation {
  void update(const AnimationParam &param) {
    if (param.state == AnimationState_Completed) {
      restartMainAnimation();
    } else {
      float progress = calcProgress(param);
      led_index_t ledIndex = pixelCount * progress;
      ledIndex = mode->animationDirection ? ledIndex : pixelCount - ledIndex;

      if (param.state == AnimationState_Started) {
        tempColor = generateColor(0);
      }

      if (!animations->IsAnimationActive(ledIndex)) {
        ledColorAnimationState[ledIndex].startColor = tempColor;
        ledColorAnimationState[ledIndex].endColor = BLACK;
        startUpdateLedColorChangeAnimation(ledIndex, calcAnimationTime() / 10);
      }
    }
  }

  uint16_t getDuration() { return 500; }
};
