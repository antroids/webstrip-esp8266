#include "animation/Animation.h"

class ShiftAnimation : public Animation {
  void start() {
    showGeneratedColors();
    startMainAnimation();
  }

  void update(const AnimationParam &param) {
    if (param.state == AnimationState_Completed) {
        if (mode->animationDirection) {
          strip->RotateRight(mode->animationIntensity);
        } else {
          strip->RotateLeft(mode->animationIntensity);
        }
        restartMainAnimation();
      }
  }

  uint16_t getDuration() { return 200; }
};
