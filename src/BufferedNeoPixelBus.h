#include <NeoPixelBus.h>

template <typename T_COLOR_FEATURE, typename T_METHOD> class BufferedNeoPixelBus : public NeoPixelBus<T_COLOR_FEATURE, T_METHOD> {
public:
  // Constructor: number of LEDs, pin number
  // NOTE:  Pin Number maybe ignored due to hardware limitations of the method.
  typename T_COLOR_FEATURE::ColorObject *pixelBuffer;

  BufferedNeoPixelBus(uint16_t countPixels, uint8_t pin) : NeoPixelBus<T_COLOR_FEATURE, T_METHOD>(countPixels, pin) {
    pixelBuffer = new typename T_COLOR_FEATURE::ColorObject[countPixels];
  }

  BufferedNeoPixelBus(uint16_t countPixels, uint8_t pinClock, uint8_t pinData) : NeoPixelBus<T_COLOR_FEATURE, T_METHOD>(countPixels, pinClock, pinData) {
    pixelBuffer = new typename T_COLOR_FEATURE::ColorObject[countPixels];
  }

  BufferedNeoPixelBus(uint16_t countPixels) : NeoPixelBus<T_COLOR_FEATURE, T_METHOD>(countPixels) {
    pixelBuffer = new typename T_COLOR_FEATURE::ColorObject[countPixels];
  }

  ~BufferedNeoPixelBus() { delete[] pixelBuffer; }

  typedef typename T_COLOR_FEATURE::ColorObject (*ColorModificator)(typename T_COLOR_FEATURE::ColorObject color, uint16_t ledIndex, float progress);

  void setBufferColor(uint16_t indexPixel, typename T_COLOR_FEATURE::ColorObject color) {
    if (indexPixel < this->PixelCount()) {
      pixelBuffer[indexPixel] = color;
    }
  }

  void clearBufferColor(typename T_COLOR_FEATURE::ColorObject color) {
    for (uint16_t ledIndex = 0; ledIndex < this->PixelCount(); ledIndex++) {
      setBufferColor(ledIndex, color);
    }
  }

  typename T_COLOR_FEATURE::ColorObject getBufferColor(uint16_t indexPixel) const {
    if (indexPixel < this->PixelCount()) {
      return pixelBuffer[indexPixel];
    } else {
      // Pixel # is out of bounds, this will get converted to a
      // color object type initialized to 0 (black)
      return 0;
    }
  }

  void modifyBufferColors(BufferedNeoPixelBus<T_COLOR_FEATURE, T_METHOD>::ColorModificator modifier, float progress) {
    for (uint16_t ledIndex = 0; ledIndex < this->PixelCount(); ledIndex++) {
      setBufferColor(ledIndex, modifier(getBufferColor(ledIndex), progress));
    }
  }

  void loadBufferColors(BufferedNeoPixelBus<T_COLOR_FEATURE, T_METHOD>::ColorModificator modifier, float progress) {
    for (uint16_t ledIndex = 0; ledIndex < this->PixelCount(); ledIndex++) {
      this->SetPixelColor(ledIndex, modifier(getBufferColor(ledIndex), ledIndex, progress));
    }
  }

  void loadBufferColors() {
    for (uint16_t ledIndex = 0; ledIndex < this->PixelCount(); ledIndex++) {
      this->SetPixelColor(ledIndex, getBufferColor(ledIndex));
    }
  }
};
