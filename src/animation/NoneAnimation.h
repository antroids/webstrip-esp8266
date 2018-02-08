#include "animation/Animation.h"

class NoneAnimation : public Animation {
  void start() {
    generateColors();
    startTransitionAnimation();
  }
};
