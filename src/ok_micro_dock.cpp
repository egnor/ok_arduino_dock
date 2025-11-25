#include "ok_micro_dock.h"

#include <array>

#include <Arduino.h>
#include <ok_little_layout.h>
#include <ok_logging.h>
#include <U8g2lib.h>
#include <Wire.h>

static const OkLoggingContext OK_CONTEXT("ok_dock");

U8G2* ok_dock_screen = nullptr;
OkLittleLayout* ok_dock_layout = nullptr;

static enum { BUTTONS_NONE, BUTTONS_PIx6408 } button_type = BUTTONS_NONE;

static bool i2c_wr(uint8_t addr, std::initializer_list<int> bytes) {
  Wire.beginTransmission(addr);
  for (auto b : bytes) Wire.write(b);
  auto const status = Wire.endTransmission();
  if (status != 0) OK_ERROR("I2C(0x%02X) write failed (%d)", addr, status);
  return (status == 0);
}

static uint8_t i2c_wr_rd(uint8_t addr, std::initializer_list<int> w) {
  i2c_wr(addr, w);
  Wire.requestFrom(addr, 1);
  if (Wire.available() < 1) OK_ERROR("I2C(0x%02X) read failed (no data)", addr);
  return Wire.read();
}

bool ok_dock_init_feather_v8() {
  // Feather boards should define SDA and SCL to match standard pins
  OK_DETAIL("Feather dock I2C setup (SCL=%d SDA=%d)", SCL, SDA);
  if (!Wire.setSDA(SDA) || !Wire.setSCL(SCL)) {
    OK_ERROR("Bad I2C pins (SCL=%d SDA=%d)", SCL, SDA);
    return false;
  }

  // Feather dock v8+ uses the PI4IOE5V6408 GPIO for buttons
  // https://www.diodes.com/datasheet/download/PI4IOE5V6408.pdf
  static constexpr int gpio_i2c = 0x43;
  Wire.beginTransmission(gpio_i2c);
  auto const gpio_i2c_status = Wire.endTransmission();
  if (gpio_i2c_status != 0) {
    OK_ERROR("I2C error %d for GPIO (0x%x)", gpio_i2c_status, gpio_i2c);
  } else {
    OK_DETAIL("Starting Feather dock button GPIO (I2C 0x%x)...", gpio_i2c);
    uint8_t const id_byte = i2c_wr_rd(gpio_i2c, {0x01});
    if ((id_byte & ~0x02) != 0xA0) {
      OK_ERROR("Button GPIO (I2C 0x%x) ID 0x%02X != 0xA0", gpio_i2c, id_byte);
    } else if (
      !i2c_wr(gpio_i2c, {0x01, 0x01}) ||  // soft reset
      !i2c_wr(gpio_i2c, {0x03, 0x00}) ||  // all input
      !i2c_wr(gpio_i2c, {0x07, 0xF8}) ||  // disable high-Z for 0, 1, 2
      !i2c_wr(gpio_i2c, {0x0D, 0x03}) ||  // pull upward for 0, 1, 2
      !i2c_wr(gpio_i2c, {0x0B, 0xF8})     // enable pull for 0, 1, 2
    ) {
      OK_ERROR("Button GPIO setup failed");
    } else {
      button_type = BUTTONS_PIx6408;
    }
  }

  // Feather dock v8+ uses an SSD1306-compatible 64x32 display
  // https://www.buydisplay.com/white-0-49-inch-oled-display-64x32-iic-i2c-ssd1315-connector-fpc
  static constexpr int screen_i2c = 0x3D;
  Wire.begin();  // In case not already done
  Wire.beginTransmission(screen_i2c);
  auto const screen_i2c_status = Wire.endTransmission();
  if (screen_i2c_status != 0) {
    OK_ERROR("I2C error %d for screen (0x%x)", screen_i2c_status, screen_i2c);
  } else {
    // See https://github.com/olikraus/u8g2/issues/2425
    OK_DETAIL("Starting Feather dock screen (I2C 0x%x)...", screen_i2c);
    ok_dock_screen = new U8G2_SSD1306_64X32_1F_F_HW_I2C(U8G2_R0);
    ok_dock_screen->setI2CAddress(screen_i2c << 1);
    ok_dock_screen->initDisplay();
    ok_dock_screen->setPowerSave(0);
    ok_dock_layout = new_ok_little_layout(ok_dock_screen->getU8g2());
    ok_dock_layout->line_printf(0, "\f9Starting...");
  }

  OK_DETAIL("Feather dock setup complete");
  return true;
}

bool ok_dock_button(int which) {
  if (button_type == BUTTONS_PIx6408) {
    static constexpr int gpio_i2c = 0x43;
    if (which < 0 || which > 2) return false;
    uint8_t const read = i2c_wr_rd(gpio_i2c, {0x0F});
    return !(read & (4 >> which));
  } else {
    return false;  // No buttons
  }
}
