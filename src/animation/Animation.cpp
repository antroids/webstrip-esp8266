#include "animation/Animation.h"

NeoPixelAnimator *Animation::animations;
LedColorAnimationState *Animation::ledColorAnimationState;
BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *Animation::strip;
LedStripMode *Animation::mode;
uint16_t Animation::pixelCount;
RgbColor Animation::tempColor = BLACK;

void Animation::init(BufferedNeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip, LedStripMode *mode) {
  Animation::pixelCount = strip->PixelCount();
  Animation::strip = strip;
  Animation::mode = mode;
  Animation::ledColorAnimationState = new LedColorAnimationState[Animation::pixelCount];
  Animation::animations = new NeoPixelAnimator(strip->PixelCount() + 1, NEO_CENTISECONDS);
}

uint16_t Animation::getMainAnimationIndex() { return pixelCount; }

RgbColor Animation::generateColor(uint16_t ledIndex) {
  switch (mode->colorSelectionMode) {
  case COLOR_SELECTION_MODE_RAND:
    return mode->colors[random(mode->colorsCount)];
  case COLOR_SELECTION_MODE_GENERATED:
    return RgbColor(GENERATE_RANDOM_COLOR);
  case COLOR_SELECTION_MODE_ASC:
  default:
    return mode->colors[ledIndex % mode->colorsCount];
  }
}

void Animation::generateColors() {
  for (uint16_t ledIndex = 0; ledIndex < pixelCount; ledIndex++) {
    RgbColor color = generateColor(ledIndex);
    strip->setBufferColor(ledIndex, color);
  }
}

float Animation::calcProgress(const AnimationParam &param) {
  return AnimationProgressMode::getFromIndex(mode->animationProgressMode)->calcProgress(param.progress);
}

void Animation::updateLedColorChangeAnimation(const AnimationParam &param) {
  float progress = calcProgress(param);
  uint16_t ledIndex = param.index;
  RgbColor updatedColor;
  if (param.state == AnimationState_Completed) {
    updatedColor = ledColorAnimationState[ledIndex].endColor;
    strip->setBufferColor(ledIndex, updatedColor);
  } else {
    updatedColor = RgbColor::LinearBlend(ledColorAnimationState[ledIndex].startColor, ledColorAnimationState[ledIndex].endColor, progress);
  }
  strip->SetPixelColor(ledIndex, updatedColor);
}

void Animation::startUpdateLedColorChangeAnimation(uint16_t ledIndex, unsigned int duration) {
  Animation::animations->StartAnimation(ledIndex, duration, updateLedColorChangeAnimation);
}

void Animation::showGeneratedColors() {
  generateColors();
  strip->loadBufferColors();
  strip->Show();
}

void Animation::showBlackColor() {
  generateColors();
  strip->ClearTo(BLACK);
  strip->Show();
}

void Animation::processAnimation() {
  if (Animation::animations->IsAnimating()) {
    Animation::animations->UpdateAnimations();
    Animation::strip->Show();
  }
}

void Animation::startMainAnimation() {
  Animation::animations->StartAnimation(getMainAnimationIndex(), calcAnimationTime(), [=](const AnimationParam &param) { return this->update(param); });
}

void Animation::restartMainAnimation() { Animation::animations->RestartAnimation(getMainAnimationIndex()); }

unsigned int Animation::calcAnimationTime() {
  unsigned int animationTime = getDuration() * (256 - Animation::mode->animationSpeed) / 128;
  return animationTime > 0 ? animationTime : 1;
}

void Animation::start() {
  showBlackColor();
  startMainAnimation();
}

void Animation::update(const AnimationParam &param) {}

void Animation::stop() { Animation::animations->StopAll(); }

uint16_t Animation::getDuration() { return 200; }
