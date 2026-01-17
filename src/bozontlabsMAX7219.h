#pragma once

#include <Arduino.h>
#include <SPI.h>

class bozontlabsMAX7219 {
public:
  bozontlabsMAX7219(pin_size_t pin_CS, pin_size_t pin_MOSI, pin_size_t pin_CLK, uint8_t devices = 4);
  bozontlabsMAX7219(pin_size_t pin_CS, uint8_t devices = 4, SPIClass* spi_bus = &SPI, uint32_t spi_bus_speed = 10000000);

  void begin();
  uint8_t getDeviceCount();
  uint8_t getDisplayWidth();
  uint8_t getDisplayHeight();
  void shutdown(bool shutdown);
  void setScanLimit(uint8_t limit);
  void setBrightness(uint8_t brightness);
  void clearDisplay();
  void setPixel(uint8_t x, uint8_t y, bool value);

private:
  bool initialized;

  uint8_t pixel_states[64];

  pin_size_t display_pin_MOSI;
  pin_size_t display_pin_CLK;
  pin_size_t display_pin_CS;

  uint8_t num_devices;
  uint8_t display_width;
  uint8_t display_height;

  void setLed(uint8_t addr, uint8_t row, uint8_t col, bool state);
  void spiTransfer(uint8_t addr, uint8_t reg, uint8_t data);

  SPIClass* hw_spi_bus;

  enum spi_bus_mode_t {
    BUSMODE_SOFTWARE_SPI,
    BUSMODE_HARDWARE_SPI
  };

  spi_bus_mode_t spi_bus_mode;
  uint32_t spi_bus_speed;

  enum max7219_register_t {
    REG_NOOP = 0,
    REG_DIGIT0 = 1,
    REG_DIGIT1 = 2,
    REG_DIGIT2 = 3,
    REG_DIGIT3 = 4,
    REG_DIGIT4 = 5,
    REG_DIGIT5 = 6,
    REG_DIGIT6 = 7,
    REG_DIGIT7 = 8,
    REG_DECODEMODE = 9,
    REG_INTENSITY = 10,
    REG_SCANLIMIT = 11,
    REG_SHUTDOWN = 12,
    REG_DISPLAYTEST = 15
  };
};
