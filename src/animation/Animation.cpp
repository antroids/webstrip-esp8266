#include "animation/Animation.h"

NeoPixelAnimator *Animation::animations;
LedColorAnimationState *Animation::ledColorAnimationState;
BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *Animation::strip;
LedStripMode *Animation::mode;
led_index_t Animation::pixelCount;
RgbColor Animation::tempColor = BLACK;
led_index_t Animation::tempLedIndex;
ScaleDescriptor Animation::inputScale(0, 255);
Log Animation::logger("Animation");

void Animation::init(BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, LedStripMode *mode) {
  Animation::pixelCount = strip->PixelCount();
  Animation::strip = strip;
  Animation::mode = mode;
  Animation::ledColorAnimationState = new LedColorAnimationState[Animation::pixelCount];
  Animation::animations = new NeoPixelAnimator(strip->PixelCount() + 1, ANIMATION_TIME_UNIT);

  logger.infof("Animations initialized with '%d' pixels", Animation::pixelCount);
}

index_id_t Animation::getMainAnimationIndex() { return pixelCount; }

RgbColor Animation::generateColor(led_index_t ledIndex) {
  return ColorSelectionMode::getFromIndex(mode->colorSelectionMode)->generateColor(ledIndex, pixelCount, mode);
}

void Animation::generateColors() {
  logger.info("Generating all colors");
  for (led_index_t ledIndex = 0; ledIndex < pixelCount; ledIndex++) {
    RgbColor color = generateColor(ledIndex);
    strip->setBufferColor(ledIndex, color);
  }
}

float Animation::calcProgress(const AnimationParam &param) {
  return AnimationProgressMode::getFromIndex(mode->animationProgressMode)->calcProgress(param.progress);
}

void Animation::updateLedColorChangeAnimation(const AnimationParam &param) {
  float progress = calcProgress(param);
  led_index_t ledIndex = param.index;
  RgbColor updatedColor;
  if (param.state == AnimationState_Completed) {
    updatedColor = ledColorAnimationState[ledIndex].endColor;
    strip->setBufferColor(ledIndex, updatedColor);
  } else {
    updatedColor = RgbColor::LinearBlend(ledColorAnimationState[ledIndex].startColor, ledColorAnimationState[ledIndex].endColor, progress);
  }
  strip->SetPixelColor(ledIndex, updatedColor);
}

void Animation::startUpdateLedColorChangeAnimation(led_index_t ledIndex, animation_duration_t duration, RgbColor fromColor, RgbColor toColor) {
  ledColorAnimationState[ledIndex].startColor = fromColor;
  ledColorAnimationState[ledIndex].endColor = toColor;
  startUpdateLedColorChangeAnimation(ledIndex, duration);
}

void Animation::startUpdateLedColorChangeAnimation(led_index_t ledIndex, animation_duration_t duration) {
  Animation::animations->StartAnimation(ledIndex, duration, updateLedColorChangeAnimation);
}

uint16_t Animation::convertToScale(ScaleDescriptor inputScale, ScaleDescriptor outputScale, uint16_t value) {
  double normalizedInputValue = outputScale.modifier(((double)value - inputScale.min) / (inputScale.max - inputScale.min));
  return outputScale.min + (outputScale.max - outputScale.min) * normalizedInputValue;
}

void Animation::processAnimation() {
  if (Animation::animations->IsAnimating()) {
    Animation::animations->UpdateAnimations();
    Animation::strip->Show();
  }
}

void Animation::startMainAnimation() {
  logger.info("Starting main animation");
  Animation::animations->StartAnimation(getMainAnimationIndex(), calcAnimationTime(), [=](const AnimationParam &param) { return this->update(param); });
}

void Animation::restartMainAnimation() {
  logger.info("Restarting main animation");
  Animation::animations->RestartAnimation(getMainAnimationIndex());
}

void Animation::updateTransitionAnimation(const AnimationParam &param) {
  if (param.state == AnimationState_Completed) {
    startMainAnimation();
  }
}

void Animation::startTransitionAnimation() {
  for (led_index_t ledIndex = 0; ledIndex < pixelCount; ledIndex++) {
    ledColorAnimationState[ledIndex].startColor = strip->GetPixelColor(ledIndex);
    ledColorAnimationState[ledIndex].endColor = strip->getBufferColor(ledIndex);
    startUpdateLedColorChangeAnimation(ledIndex, TRANSITION_ANIMATION_DURATION);
  }
  Animation::animations->StartAnimation(getMainAnimationIndex(), TRANSITION_ANIMATION_DURATION,
                                        [=](const AnimationParam &param) { return this->updateTransitionAnimation(param); });
}

animation_duration_t Animation::calcAnimationTime() { return convertToScale(inputScale, getAnimationSpeedScale(), inputScale.max - mode->animationSpeed); }
animation_duration_t Animation::calcAnimationIntensity() { return convertToScale(inputScale, getAnimationIntensityScale(), mode->animationIntensity); }

void Animation::start() {
  strip->clearBufferColor(BLACK);
  startTransitionAnimation();
}

void Animation::update(const AnimationParam &param) {}

void Animation::stop() { Animation::animations->StopAll(); }

ScaleDescriptor Animation::getAnimationSpeedScale() { return ScaleDescriptor(SECONDS_TO_ANIMATION_TIME(0.1), SECONDS_TO_ANIMATION_TIME(2)); }
ScaleDescriptor Animation::getAnimationIntensityScale() { return ScaleDescriptor(1, 20); }
