#include <NeoPixelAnimator.h>

typedef float (*AnimationProgressModifierFunctionType)(float progress);

const AnimationProgressModifierFunctionType ANIMATION_PROGRESS_LINEAR = [](float progress) { return progress; };
const AnimationProgressModifierFunctionType ANIMATION_PROGRESS_SIN_IN = NeoEase::SinusoidalIn;
const AnimationProgressModifierFunctionType ANIMATION_PROGRESS_SIN_OUT = NeoEase::SinusoidalOut;
const AnimationProgressModifierFunctionType ANIMATION_PROGRESS_SIN_IN_OUT = NeoEase::SinusoidalInOut;
const AnimationProgressModifierFunctionType activeAnimationProgressModes[] = {ANIMATION_PROGRESS_LINEAR, ANIMATION_PROGRESS_SIN_IN, ANIMATION_PROGRESS_SIN_OUT,
                                                                              ANIMATION_PROGRESS_SIN_IN_OUT};

inline uint16_t getAnimationProgressModeIndex(AnimationProgressModifierFunctionType mode) {
  for (uint16_t i = 0; i < sizeof(activeAnimationProgressModes) / sizeof(AnimationProgressModifierFunctionType); i++) {
    if (activeAnimationProgressModes[i] == mode)
      return i;
  }
}
