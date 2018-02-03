#include "animation/Animation.h"

class NoneAnimation : public Animation {
  void start() { Animation::showGeneratedColors(); }
};
