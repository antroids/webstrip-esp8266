#ifndef ANIMATION_EXPLOSIONS_ANIMATION_H
#define ANIMATION_EXPLOSIONS_ANIMATION_H

#include "animation/Animation.h"

class ExplosionsAnimation : public Animation {
public:
  ExplosionsAnimation(Context *_context) : Animation(_context){};

protected:
  void update(const AnimationParam &param) {
    if (param.state == AnimationState_Completed) {
      restartMainAnimation();
    } else {
      if (param.state == AnimationState_Started) {
        tempLedIndex = random(getPixelCount());
        startFlashPixelAnimation(tempLedIndex, getColorForLedIndex(tempLedIndex));
      } else {
        float progress = calcProgress(param);
        led_index_t dLedIndex = calcAnimationIntensity() * progress;
        int leftLedIndex = ((int)tempLedIndex) - dLedIndex;
        int rightLedIndex = ((int)tempLedIndex) + dLedIndex;
        float brightness = NeoEase::SinusoidalIn(1.0 - 1.0 / calcAnimationIntensity() * dLedIndex);

        if (leftLedIndex >= 0) {
          RgbColor color = changeColorBrightness(getColorForLedIndex(leftLedIndex), brightness);
          startFlashPixelAnimation(leftLedIndex, color);
        }
        if (rightLedIndex < getPixelCount()) {
          RgbColor color = changeColorBrightness(getColorForLedIndex(rightLedIndex), brightness);
          startFlashPixelAnimation(rightLedIndex, color);
        }
      }
    }
  }

  void updateFlashPixelAnimation(const AnimationParam &param) {
    float progress = calcProgress(param);
    led_index_t ledIndex = param.index;
    RgbColor updatedColor;
    if (param.state == AnimationState_Completed) {
      animation_duration_t duration = calcAnimationTime() - abs(ledIndex - tempLedIndex) * calcAnimationTime() / calcAnimationIntensity();
      updatedColor = getLedColorAnimationState()[ledIndex].endColor;
      startUpdateLedColorChangeAnimation(ledIndex, duration, updatedColor, BLACK);
    } else {
      updatedColor = RgbColor::LinearBlend(getLedColorAnimationState()[ledIndex].startColor, getLedColorAnimationState()[ledIndex].endColor, progress);
    }
    getStrip()->SetPixelColor(ledIndex, updatedColor);
  }

  void startFlashPixelAnimation(led_index_t ledIndex, RgbColor color) {
    unsigned int duration = calcAnimationTime() / calcAnimationIntensity();

    getLedColorAnimationState()[ledIndex].startColor = BLACK;
    getLedColorAnimationState()[ledIndex].endColor = color;
    getAnimator()->StartAnimation(ledIndex, duration, [=](const AnimationParam &param) { return this->updateFlashPixelAnimation(param); });
  }

  ScaleDescriptor getAnimationSpeedScale() { return ScaleDescriptor(SECONDS_TO_ANIMATION_TIME(0.3), SECONDS_TO_ANIMATION_TIME(3)); }

  virtual RgbColor getColorForLedIndex(led_index_t ledIndex) { return generateColor(ledIndex); }
};

#endif
