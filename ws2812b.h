#if !defined(_WS2812_H)
#define _WS2812_H

#include <stdint.h>
#include <driver/rmt_tx.h>

#define WS2812B_MAX_LEDS 128

class Ws2812b
{
public:
  typedef uint8_t led_red_t;
  typedef uint8_t led_green_t;
  typedef uint8_t led_blue_t;

  struct LedData
  {
    led_red_t red = 0;
    led_green_t green = 0;
    led_blue_t blue = 0;
  };

  static const LedData LED_BLACK;
  static const LedData LED_WHITE;
  static const LedData LED_RED;
  static const LedData LED_GREEN;
  static const LedData LED_BLUE;

  static const int MIN_RESET_MS = 50;

  static const int DATA_PERIOD_NS = 1250; // 800 kHz
  static const int ZERO_DUTY_NS = 400;
  static const int ONE_DUTY_NS = 800;

  static const int TICK_RATE = 20 * 1000 * 1000; // 20 Mhz
  static const int TICK_NS = 50;                 // 20 Mhz
  static const int ENCODER_ZERO_HIGH_TICKS = ZERO_DUTY_NS / TICK_NS;
  static const int ENCODER_ZERO_LOW_TICKS = (DATA_PERIOD_NS - ZERO_DUTY_NS) / TICK_NS;
  static const int ENCODER_ONE_HIGH_TICKS = ONE_DUTY_NS / TICK_NS;
  static const int ENCODER_ONE_LOW_TICKS = (DATA_PERIOD_NS - ONE_DUTY_NS) / TICK_NS;

  static const rmt_symbol_word_t ZERO_BIT;
  static const rmt_symbol_word_t ONE_BIT;

  static const int data_size = 24;

  Ws2812b(gpio_num_t data_pin, size_t led_count);

  void begin();
  void end();
  void updateLed(LedData *data);

  static LedData mkLedData(led_red_t red, led_green_t green, led_blue_t blue);
  static LedData mkLedDataF(float red, float green, float blue);

private:
  uint8_t _data_pin;
  int _led_count;
  rmt_channel_handle_t _rmt_chan;
  rmt_encoder_handle_t _rmt_encoder;
  uint8_t _buf[3 * WS2812B_MAX_LEDS];
};

#endif // _WS2812_H
