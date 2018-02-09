#include "animation/Animation.h"

class SolidExplosionsAnimation : public Animation {
  void update(const AnimationParam &param) {
    if (param.state == AnimationState_Completed) {
      restartMainAnimation();
    } else {
      if (param.state == AnimationState_Started) {
        tempColor = generateColor(0);
        tempLedIndex = random(pixelCount);
        startFlashPixelAnimation(tempLedIndex, tempColor);
      } else {
        float progress = calcProgress(param);
        led_index_t dLedIndex = mode->animationIntensity * progress;
        int leftLedIndex = ((int)tempLedIndex) - dLedIndex;
        int rightLedIndex = ((int)tempLedIndex) + dLedIndex;
        RgbColor color = changeColorBrightness(tempColor, NeoEase::SinusoidalIn(1.0 - 1.0 / mode->animationIntensity * dLedIndex));

        if (leftLedIndex >= 0) {
          startFlashPixelAnimation(leftLedIndex, color);
        }
        if (rightLedIndex < pixelCount) {
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
      updatedColor = ledColorAnimationState[ledIndex].endColor;
      ledColorAnimationState[ledIndex].startColor = updatedColor;
      ledColorAnimationState[ledIndex].endColor = BLACK;
      startUpdateLedColorChangeAnimation(ledIndex, calcAnimationTime() - abs(ledIndex - tempLedIndex) * calcAnimationTime() / mode->animationIntensity);
    } else {
      updatedColor = RgbColor::LinearBlend(ledColorAnimationState[ledIndex].startColor, ledColorAnimationState[ledIndex].endColor, progress);
    }
    strip->SetPixelColor(ledIndex, updatedColor);
  }

  void startFlashPixelAnimation(led_index_t ledIndex, RgbColor color) {
    unsigned int duration = calcAnimationTime() / mode->animationIntensity;

    ledColorAnimationState[ledIndex].startColor = BLACK;
    ledColorAnimationState[ledIndex].endColor = color;
    Animation::animations->StartAnimation(ledIndex, duration, [=](const AnimationParam &param) { return this->updateFlashPixelAnimation(param); });
  }

  uint16_t getDuration() { return 50; }
};
