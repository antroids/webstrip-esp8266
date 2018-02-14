#ifndef ANIMATION_NONE_ANIMATION_H
#define ANIMATION_NONE_ANIMATION_H

#include "animation/Animation.h"

class NoneAnimation : public Animation {
public:
  NoneAnimation(Context *_context) : Animation(_context){};

protected:
  void start() {
    generateColors();
    startTransitionAnimation();
  }
};

#endif
