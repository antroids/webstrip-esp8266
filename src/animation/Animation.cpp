#include "../Context.h"
#include "../domain/ColorSelectionMode.h"
#include "../domain/LedStripMode.h"
#include "animation/Animation.h"

RgbColor Animation::tempColor = BLACK;
led_index_t Animation::tempLedIndex;
ScaleDescriptor Animation::inputScale(0, 255);
Log Animation::logger("Animation");

index_id_t Animation::getMainAnimationIndex() { return getPixelCount(); }

RgbColor Animation::generateColor(led_index_t ledIndex) {
  return ColorSelectionMode::getFromIndex(getMode()->colorSelectionMode)->generateColor(ledIndex, getPixelCount(), getMode());
}

void Animation::generateColors() {
  logger.info("Generating all colors");
  for (led_index_t ledIndex = 0; ledIndex < getPixelCount(); ledIndex++) {
    RgbColor color = generateColor(ledIndex);
    getStrip()->setBufferColor(ledIndex, color);
  }
}

float Animation::calcProgress(const AnimationParam &param) {
  return AnimationProgressMode::getFromIndex(getMode()->animationProgressMode)->calcProgress(param.progress);
}

void Animation::updateLedColorChangeAnimation(const AnimationParam &param) {
  float progress = calcProgress(param);
  led_index_t ledIndex = param.index;
  RgbColor updatedColor;
  if (param.state == AnimationState_Completed) {
    updatedColor = getLedColorAnimationState()[ledIndex].endColor;
    getStrip()->setBufferColor(ledIndex, updatedColor);
  } else {
    updatedColor = RgbColor::LinearBlend(getLedColorAnimationState()[ledIndex].startColor, getLedColorAnimationState()[ledIndex].endColor, progress);
  }
  getStrip()->SetPixelColor(ledIndex, updatedColor);
}

void Animation::startUpdateLedColorChangeAnimation(led_index_t ledIndex, animation_duration_t duration, RgbColor fromColor, RgbColor toColor) {
  getLedColorAnimationState()[ledIndex].startColor = fromColor;
  getLedColorAnimationState()[ledIndex].endColor = toColor;
  startUpdateLedColorChangeAnimation(ledIndex, duration);
}

void Animation::startUpdateLedColorChangeAnimation(led_index_t ledIndex, animation_duration_t duration) {
  getAnimator()->StartAnimation(ledIndex, duration, [=](const AnimationParam &param) { return this->updateLedColorChangeAnimation(param); });
}

uint16_t Animation::convertToScale(ScaleDescriptor inputScale, ScaleDescriptor outputScale, uint16_t value) {
  double normalizedInputValue = outputScale.modifier(((double)value - inputScale.min) / (inputScale.max - inputScale.min));
  return outputScale.min + (outputScale.max - outputScale.min) * normalizedInputValue;
}

LedStripMode *Animation::getMode() { return context->mode; }

led_index_t Animation::getPixelCount() { return context->options->pixelCount; }

BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *Animation::getStrip() { return context->strip; }

LedColorAnimationState *Animation::getLedColorAnimationState() { return context->ledColorAnimationState; }

NeoPixelAnimator *Animation::getAnimator() { return context->animator; }

void Animation::showProgress(const RgbColor bgColor, const RgbColor doneColor, float progress) {
  for (led_index_t ledIndex = 0; ledIndex < getPixelCount(); ledIndex++) {
    getStrip()->SetPixelColor(ledIndex, ledIndex < getPixelCount() * progress ? doneColor : bgColor);
  }
  getStrip()->Show();
}

void Animation::processAnimation() {
  if (getAnimator()->IsAnimating()) {
    getAnimator()->UpdateAnimations();
    getStrip()->Show();
  }
}

void Animation::startMainAnimation() {
  logger.info("Starting main animation");
  getAnimator()->StartAnimation(getMainAnimationIndex(), calcAnimationTime(), [=](const AnimationParam &param) { return this->update(param); });
}

void Animation::restartMainAnimation() {
  logger.info("Restarting main animation");
  getAnimator()->RestartAnimation(getMainAnimationIndex());
}

void Animation::updateTransitionAnimation(const AnimationParam &param) {
  if (param.state == AnimationState_Completed) {
    startMainAnimation();
  }
}

void Animation::startTransitionAnimation() {
  for (led_index_t ledIndex = 0; ledIndex < getPixelCount(); ledIndex++) {
    getLedColorAnimationState()[ledIndex].startColor = getStrip()->GetPixelColor(ledIndex);
    getLedColorAnimationState()[ledIndex].endColor = getStrip()->getBufferColor(ledIndex);
    startUpdateLedColorChangeAnimation(ledIndex, TRANSITION_ANIMATION_DURATION);
  }
  getAnimator()->StartAnimation(getMainAnimationIndex(), TRANSITION_ANIMATION_DURATION,
                                [=](const AnimationParam &param) { return this->updateTransitionAnimation(param); });
}

animation_duration_t Animation::calcAnimationTime() { return convertToScale(inputScale, getAnimationSpeedScale(), inputScale.max - getMode()->animationSpeed); }
animation_duration_t Animation::calcAnimationIntensity() { return convertToScale(inputScale, getAnimationIntensityScale(), getMode()->animationIntensity); }

void Animation::start() {
  getStrip()->clearBufferColor(BLACK);
  startTransitionAnimation();
}

void Animation::update(const AnimationParam &param) {}

void Animation::stop() { getAnimator()->StopAll(); }

ScaleDescriptor Animation::getAnimationSpeedScale() { return ScaleDescriptor(SECONDS_TO_ANIMATION_TIME(0.1), SECONDS_TO_ANIMATION_TIME(2)); }
ScaleDescriptor Animation::getAnimationIntensityScale() { return ScaleDescriptor(1, 20); }
