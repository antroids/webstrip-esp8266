#ifndef ANIMATION_SHIFT_ANIMATION_H
#define ANIMATION_SHIFT_ANIMATION_H

#include "animation/Animation.h"

class ShiftAnimation : public Animation {
protected:
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

#endif
