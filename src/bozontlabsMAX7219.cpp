#include "bozontlabsMAX7219.h"

bozontlabsMAX7219::bozontlabsMAX7219(pin_size_t pin_CS, pin_size_t pin_MOSI, pin_size_t pin_CLK, uint8_t devices)
    : display_on(false)
    , hw_spi_bus(nullptr) {
  this->display_pin_MOSI = pin_MOSI;
  this->display_pin_CLK = pin_CLK;
  this->display_pin_CS = pin_CS;

  if (devices <= 0u || devices > 8u) {
    devices = 8u;
  }
  this->num_devices = devices;
  this->display_height = 8u;
  this->display_width = 8u * this->num_devices;
  this->spi_bus_mode = BUSMODE_SOFTWARE_SPI;
}

bozontlabsMAX7219::bozontlabsMAX7219(pin_size_t pin_CS, uint8_t devices, SPIClass* spi_bus, uint32_t spi_bus_speed) {
  this->display_pin_CS = pin_CS;

  if (devices <= 0u || devices > 8u) {
    devices = 8u;
  }
  this->num_devices = devices;
  this->display_height = 8u;
  this->display_width = 8u * this->num_devices;

  this->spi_bus_mode = BUSMODE_HARDWARE_SPI;
  this->hw_spi_bus = spi_bus;
  this->spi_bus_speed = spi_bus_speed;
}

void bozontlabsMAX7219::begin() {
  if (this->spi_bus_mode == BUSMODE_SOFTWARE_SPI) {
    pinMode(this->display_pin_MOSI, OUTPUT);
    pinMode(this->display_pin_CLK, OUTPUT);
  } else if (this->spi_bus_mode == BUSMODE_HARDWARE_SPI) {
    this->hw_spi_bus->begin();
  }

  pinMode(this->display_pin_CS, OUTPUT);
  digitalWrite(this->display_pin_CS, HIGH);

  memset(this->pixel_states, 0u, sizeof(this->pixel_states));

  for (uint8_t i = 0u; i < this->num_devices; i++) {
    this->spiTransfer(i, REG_DISPLAYTEST, 0u);
    this->spiTransfer(i, REG_DECODEMODE, 0u);
  }
  this->clearDisplay();
  this->setScanLimit(this->led_matrix_scan_limit);
  this->setDisplayOn(true);
  this->initialized = true;
}

uint8_t bozontlabsMAX7219::getDeviceCount() {
  return this->num_devices;
}

uint8_t bozontlabsMAX7219::getDisplayWidth() {
  return this->display_width;
}

uint8_t bozontlabsMAX7219::getDisplayHeight() {
  return this->display_height;
}

void bozontlabsMAX7219::setDisplayOn(bool on) {
  uint8_t cmd_payload;
  if (on) {
    cmd_payload = 1u;
    this->display_on = true;
  } else {
    cmd_payload = 0u;
    this->display_on = false;
  }
  for (uint8_t addr = 0u; addr < this->num_devices; addr++) {
    this->spiTransfer(addr, REG_SHUTDOWN, cmd_payload);
  }
}

bool bozontlabsMAX7219::getDisplayOn() {
  return this->display_on;
}

void bozontlabsMAX7219::setScanLimit(uint8_t limit) {
  if (limit > 7u) return;
  for (uint8_t addr = 0u; addr < this->num_devices; addr++) {
    this->spiTransfer(addr, REG_SCANLIMIT, limit);
  }
}

void bozontlabsMAX7219::setBrightness(uint8_t brightness) {
  if (brightness > 15u) return;
  for (uint8_t addr = 0u; addr < this->num_devices; addr++) {
    this->spiTransfer(addr, REG_INTENSITY, brightness);
  }
}

void bozontlabsMAX7219::clearDisplay() {
  for (uint8_t addr = 0u; addr < this->num_devices; addr++) {
    uint8_t offset = addr * 8u;
    for (uint8_t i = 0u; i < 8u; i++) {
      this->pixel_states[offset + i] = 0u;
      this->spiTransfer(addr, i + 1u, this->pixel_states[offset + i]);
    }
  }
}

void bozontlabsMAX7219::setPixel(uint8_t x, uint8_t y, bool value) {
  if (x >= this->display_width || y >= this->display_height) {
    return;
  }
  uint8_t module_number = x / 8u;
  uint8_t x_inside_module = x % 8u;
  this->setLed(module_number, 7u - y, 7u - x_inside_module, value);
}

void bozontlabsMAX7219::setLed(uint8_t addr, uint8_t row, uint8_t column, bool state) {
  if (addr < 0u || addr >= this->num_devices) {
    return;
  }
  if (row < 0u || row > 7u || column < 0u || column > 7u) {
    return;
  }
  uint8_t offset = addr * 8u;
  uint8_t val = B10000000 >> column;
  if (state) {
    this->pixel_states[offset + row] = this->pixel_states[offset + row] | val;
  } else {
    val = ~val;
    this->pixel_states[offset + row] = this->pixel_states[offset + row] & val;
  }
  this->spiTransfer(addr, row + 1u, this->pixel_states[offset + row]);
}

void bozontlabsMAX7219::spiTransfer(uint8_t addr, uint8_t reg, uint8_t data) {
  static uint8_t spi_output_buffer[16];
  uint8_t offset = addr * 2u;
  uint8_t maxbytes = this->num_devices * 2u;

  memset(spi_output_buffer, 0u, maxbytes);

  spi_output_buffer[offset] = data;
  spi_output_buffer[offset + 1u] = reg;

  digitalWrite(this->display_pin_CS, LOW);

  if (this->spi_bus_mode == BUSMODE_SOFTWARE_SPI) {
    for (uint8_t i = maxbytes; i > 0u; i--) {
      shiftOut(this->display_pin_MOSI, this->display_pin_CLK, MSBFIRST, spi_output_buffer[i - 1u]);
    }
  } else if (this->spi_bus_mode == BUSMODE_HARDWARE_SPI) {
    this->hw_spi_bus->beginTransaction(SPISettings(this->spi_bus_speed, MSBFIRST, SPI_MODE0));
    for (uint8_t i = maxbytes; i > 0u; i--) {
      this->hw_spi_bus->transfer(spi_output_buffer[i - 1u]);
    }
    this->hw_spi_bus->endTransaction();
  }

  digitalWrite(this->display_pin_CS, HIGH);
}
