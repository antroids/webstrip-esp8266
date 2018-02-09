#include "animation/Animation.h"

class ShiftAnimation : public Animation {
  void start() {
    generateColors();
    startTransitionAnimation();
  }

  void update(const AnimationParam &param) {
    if (param.state == AnimationState_Completed) {
      if (mode->animationDirection) {
        strip->RotateRight(calcAnimationIntensity());
      } else {
        strip->RotateLeft(calcAnimationIntensity());
      }
      restartMainAnimation();
    }
  }
};
