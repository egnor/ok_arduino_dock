#include "blub_station.h"

#include <Arduino.h>
#include <ok_little_layout.h>
#include <ok_logging.h>
#include <U8g2lib.h>
#include <Wire.h>

static const OkLoggingContext OK_CONTEXT("ok_micro_dock");

class DummyLayout : public OkLittleLayout {
  public:
    virtual void line_printf(int line, char const* format, ...) override {}
    virtual u8g2_t* get_u8g2() const override { return nullptr; }
};

static OkDockType dock_type = OK_NO_DOCK;
static bool buttons_ok = false;

static u8g2_t screen_driver;
static DummyLayout dummy_layout;
OkLittleLayout* ok_dock_screen = &dummy_layout;

static bool i2c_wr(uint8_t addr, std::initializer_list<int> bytes) {
  Wire.beginTransmission(addr);
  for (auto b : bytes) Wire.write(b);
  auto const status = Wire.endTransmission();
  if (status != 0) OK_ERROR("I2C(0x%02X) write failed (%d)", addr, status);
  return (status == 0);
}

static uint8_t i2c_wr_rd(uint8_t addr, std::initializer_list<int> w) {
  wbytes(i2c, addr, w);
  Wire.requestFrom(addr, 1);
  if (Wire.available() < 1) OK_ERROR("I2C(0x%02X) read failed (no data)", addr);
  return Wire.read();
}

static bool init_feather_v8() {
  OK_DETAIL("Feather dock I2C setup (SCL=%d SDA=%d)", SCL, SDA);
  if (!Wire.setSDA(SDA) || !Wire.setSCL(SCL)) {
    OK_ERROR("Bad I2C pins (SCL=%d SDA=%d)", SCL, SDA);
    return false;
  }

  static constexpr int gpio_i2c = 0x43;
  Wire.beginTransmission(gpio_i2c);
  auto const gpio_i2c_status = Wire.endTransmission();
  if (gpio_i2c_status != 0) {
    OK_ERROR("I2C error %d for GPIO (0x%x)", gpio_i2c_status, gpio_i2c);
  } else {
    OK_DEBUG("Starting Feather dock button GPIO (I2C 0x%x)...", gpio_i2c);
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
      buttons_ok = true;
    }
  }

  static constexpr int screen_i2c = 0x3D;
  Wire.begin();  // In case not already done
  Wire.beginTransmission(screen_i2c);
  auto const screen_i2c_status = Wire.endTransmission();
  if (screen_i2c_status != 0) {
    OK_ERROR("I2C error %d for screen (0x%x)", screen_i2c_status, screen_i2c);
  } else {
    OK_DEBUG("Starting Feather dock screen (I2C 0x%x)...", screen_i2c);
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(
        &screen_driver, U8G2_R0,
        u8x8_byte_arduino_hw_i2c, u8x8_gpio_and_delay_arduino
    );

    // See https://github.com/olikraus/u8g2/issues/2425
    u8g2_SetI2CAddress(&screen_driver, screen_i2c << 1);
    u8g2_InitDisplay(&screen_driver);
    u8g2_SetPowerSave(&screen_driver, 0);
    ok_dock_layout = new_ok_little_layout(&screen_driver);
    ok_dock_layout->line_printf(0, "\f9Starting...");
  }
}

bool ok_dock_init(OkMicroDockType type) {
  dock_type = type;
  if (type == OK_NO_DOCK) {
    OK_DETAIL("No dock hardware configured");
    return true;
  } else if (type == OK_FEATHER_DOCK_V8) {
    return init_feather_v8();
  } else {
    OK_ERROR("Unknown dock type: %d", (int) type);
    dock_type = OK_NO_DOCK;
    return false;
  }
}

bool ok_dock_button(int which) {
  if (!buttons_ok) {
    return false;
  } else if (dock_type == OK_FEATHER_DOCK_V8) {
    static constexpr int gpio_i2c = 0x43;
    if (which < 0 || which > 2) return false;
    uint8_t const read = i2c_wr_rd(gpio_i2c, {0x0F});
    return !(read & (1 << which));
  } else {
    return false;  // No buttons for dock type
  }
}
