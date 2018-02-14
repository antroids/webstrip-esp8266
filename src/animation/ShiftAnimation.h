#ifndef ANIMATION_SHIFT_ANIMATION_H
#define ANIMATION_SHIFT_ANIMATION_H

#include "animation/Animation.h"

class ShiftAnimation : public Animation {
public:
  ShiftAnimation(Context *_context) : Animation(_context){};

protected:
  void start() {
    generateColors();
    startTransitionAnimation();
  }

  void update(const AnimationParam &param) {
    if (param.state == AnimationState_Completed) {
      if (getMode()->animationDirection) {
        getStrip()->RotateRight(calcAnimationIntensity());
      } else {
        getStrip()->RotateLeft(calcAnimationIntensity());
      }
      restartMainAnimation();
    }
  }
};

#endif
