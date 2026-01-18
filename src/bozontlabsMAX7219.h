#pragma once

#include <Arduino.h>
#include <SPI.h>

class bozontlabsMAX7219 {
public:
  bozontlabsMAX7219(pin_size_t pin_CS, pin_size_t pin_MOSI, pin_size_t pin_CLK, uint8_t devices = 4);
  bozontlabsMAX7219(pin_size_t pin_CS, uint8_t devices = 4, SPIClass* spi_bus = &SPI, uint32_t spi_bus_speed = 10000000);

  // Initializes the display
  void begin();

  // Returns the number of cascaded MAX7219 devices configured
  uint8_t getDeviceCount();

  // Returns the calculated display width in pixels
  uint8_t getDisplayWidth();

  // Returns the calculated display height in pixels
  uint8_t getDisplayHeight();

  // Turns the display on/off
  void shutdown(bool shutdown);

  // Sets the scan limit (rows scanned)
  void setScanLimit(uint8_t limit);

  // Sets the brigtness of the display (0-15)
  void setBrightness(uint8_t brightness);

  // Clears the contents of the display
  void clearDisplay();

  // Sets or clears a pixel in the specified location
  void setPixel(uint8_t x, uint8_t y, bool value);

private:
  bool initialized;

  uint8_t pixel_states[64];

  // Scan limit must be 7 when using the MAX7219 chip to dirve an LED matrix
  // 7 means that all 8 rows of a matrix is scanned and therefore usable
  // Lower scan values are only used when driving a seven segment display with the chip
  static const uint8_t led_matrix_scan_limit = 7u;

  pin_size_t display_pin_MOSI;
  pin_size_t display_pin_CLK;
  pin_size_t display_pin_CS;

  uint8_t num_devices;
  uint8_t display_width;
  uint8_t display_height;

  // Sets or clears a pixel on the specified device
  void setLed(uint8_t addr, uint8_t row, uint8_t col, bool state);

  // Transfers a byte of data to the specified register via SPI
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
