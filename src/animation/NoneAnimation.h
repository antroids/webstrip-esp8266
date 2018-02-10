#ifndef ANIMATION_NONE_ANIMATION_H
#define ANIMATION_NONE_ANIMATION_H

#include "animation/Animation.h"

class NoneAnimation : public Animation {
protected:
  void start() {
    generateColors();
    startTransitionAnimation();
  }
};

#endif
