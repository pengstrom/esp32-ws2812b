#include "ws2812b.h"
#include <driver/rmt_tx.h>

const Ws2812b::LedData Ws2812b::LED_BLACK = Ws2812b::LedData();
const Ws2812b::LedData Ws2812b::LED_WHITE = {255, 255, 255};
const Ws2812b::LedData Ws2812b::LED_RED = {255, 0, 0};
const Ws2812b::LedData Ws2812b::LED_GREEN = {0, 255, 0};
const Ws2812b::LedData Ws2812b::LED_BLUE = {0, 0, 255};

const rmt_symbol_word_t Ws2812b::ZERO_BIT =
    {
        .duration0 = ENCODER_ZERO_HIGH_TICKS,
        .level0 = 1,
        .duration1 = ENCODER_ZERO_LOW_TICKS,
        .level1 = 0,
};

const rmt_symbol_word_t Ws2812b::ONE_BIT =
    {
        .duration0 = ENCODER_ONE_HIGH_TICKS,
        .level0 = 1,
        .duration1 = ENCODER_ONE_LOW_TICKS,
        .level1 = 0,
};

Ws2812b::Ws2812b(gpio_num_t data_pin, size_t led_count)
{
  _data_pin = data_pin;
  _led_count = led_count;

  for (size_t i = 0; i < 3 * led_count; i++)
  {
    _buf[i] = 0;
  }

  rmt_tx_channel_config_t tx_chan_config = {
      .gpio_num = data_pin,
      .clk_src = RMT_CLK_SRC_DEFAULT,
      .resolution_hz = TICK_RATE,
      .mem_block_symbols = 3 * led_count,
      .trans_queue_depth = 4,
      .intr_priority = 2,
      .flags = {
          .invert_out = false,
          .with_dma = false,
          .io_loop_back = false,
          .io_od_mode = false}};
  ESP_ERROR_CHECK(rmt_new_tx_channel(&tx_chan_config, &_rmt_chan));

  rmt_bytes_encoder_config_t bytes_config = {
      .bit0 = ZERO_BIT,
      .bit1 = ONE_BIT,
      .flags = {
          .msb_first = true}};
  ESP_ERROR_CHECK(rmt_new_bytes_encoder(&bytes_config, &_rmt_encoder));
}

void Ws2812b::begin()
{
  ESP_ERROR_CHECK(rmt_enable(_rmt_chan));
}

void Ws2812b::end()
{
  ESP_ERROR_CHECK(rmt_disable(_rmt_chan));
}

void Ws2812b::updateLed(LedData *data)
{
  for (size_t i = 0; i < _led_count; i++)
  {
    _buf[3 * i] = data[i].green;
    _buf[3 * i + 1] = data[i].red;
    _buf[3 * i + 2] = data[i].blue;
  }

  rmt_transmit_config_t cfg =
      {
          .loop_count = 0,
          .flags = {
              .eot_level = 0,
              .queue_nonblocking = false}};
  ESP_ERROR_CHECK(rmt_transmit(_rmt_chan, _rmt_encoder, _buf, 3 * _led_count, &cfg));
  ESP_ERROR_CHECK(rmt_tx_wait_all_done(_rmt_chan, -1));
}

Ws2812b::LedData Ws2812b::mkLedData(led_red_t red, led_green_t green, led_blue_t blue)
{
  return {
      .red = red,
      .green = green,
      .blue = blue};
}

Ws2812b::LedData Ws2812b::mkLedDataF(float red, float green, float blue)
{
  return mkLedData(255 * red, 255 * green, 255 * blue);
}
