#ifndef ANIMATION_SOLID_EXPLOSIONS_ANIMATION_H
#define ANIMATION_SOLID_EXPLOSIONS_ANIMATION_H

#include "ExplosionsAnimation.h"
#include "animation/Animation.h"

class SolidExplosionsAnimation : public ExplosionsAnimation {
public:
  SolidExplosionsAnimation(Context *_context) : ExplosionsAnimation(_context){};

protected:
  void update(const AnimationParam &param) {
    if (param.state == AnimationState_Started) {
      tempColor = generateColor(0);
    }
    ExplosionsAnimation::update(param);
  }

  virtual RgbColor getColorForLedIndex(led_index_t ledIndex) { return tempColor; }
};

#endif
