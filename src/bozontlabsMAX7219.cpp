#include "bozontlabsMAX7219.h"

bozontlabsMAX7219::bozontlabsMAX7219(pin_size_t pin_CS, pin_size_t pin_MOSI, pin_size_t pin_CLK, uint8_t devices)
    : hw_spi_bus(nullptr) {
  this->display_pin_MOSI = pin_MOSI;
  this->display_pin_CLK = pin_CLK;
  this->display_pin_CS = pin_CS;

  if (devices <= 0 || devices > 8) {
    devices = 8;
  }
  this->num_devices = devices;
  this->display_height = 8u;
  this->display_width = 8u * this->num_devices;
  this->spi_bus_mode = BUSMODE_SOFTWARE_SPI;
}

bozontlabsMAX7219::bozontlabsMAX7219(pin_size_t pin_CS, uint8_t devices, SPIClass* spi_bus, uint32_t spi_bus_speed) {
  this->display_pin_CS = pin_CS;

  if (devices <= 0 || devices > 8) {
    devices = 8;
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

  for (uint8_t i = 0; i < this->num_devices; i++) {
    this->spiTransfer(i, REG_DISPLAYTEST, 0);
    this->spiTransfer(i, REG_DECODEMODE, 0);
  }
  this->clearDisplay();
  this->setScanLimit(7);
  this->shutdown(false);
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

void bozontlabsMAX7219::shutdown(bool shutdown) {
  uint8_t cmd_payload;
  if (!shutdown) {
    cmd_payload = 0x01;
  } else {
    cmd_payload = 0x00;
  }
  for (uint8_t addr = 0u; addr < this->num_devices; addr++) {
    this->spiTransfer(addr, REG_SHUTDOWN, cmd_payload);
  }
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
    int offset = addr * 8;
    for (int i = 0; i < 8; i++) {
      this->pixel_states[offset + i] = 0;
      this->spiTransfer(addr, i + 1, this->pixel_states[offset + i]);
    }
  }
}

void bozontlabsMAX7219::setPixel(uint8_t x, uint8_t y, bool value) {
  if (x >= this->display_width || y >= this->display_height) {
    return;
  }
  uint8_t module_number = x / 8;
  uint8_t x_inside_module = x % 8;
  this->setLed(module_number, 7 - y, 7 - x_inside_module, value);
}

void bozontlabsMAX7219::setLed(uint8_t addr, uint8_t row, uint8_t column, bool state) {
  int offset;
  uint8_t val = 0x00;
  if (addr < 0u || addr >= this->num_devices) {
    return;
  }
  if (row < 0u || row > 7u || column < 0u || column > 7u) {
    return;
  }
  offset = addr * 8u;
  val = B10000000 >> column;
  if (state) {
    this->pixel_states[offset + row] = this->pixel_states[offset + row] | val;
  } else {
    val = ~val;
    this->pixel_states[offset + row] = this->pixel_states[offset + row] & val;
  }
  this->spiTransfer(addr, row + 1, this->pixel_states[offset + row]);
}

void bozontlabsMAX7219::spiTransfer(uint8_t addr, uint8_t reg, uint8_t data) {
  static uint8_t spi_output_buffer[16];
  uint8_t offset = addr * 2;
  uint8_t maxbytes = this->num_devices * 2;

  memset(spi_output_buffer, 0u, maxbytes);

  spi_output_buffer[offset] = data;
  spi_output_buffer[offset + 1] = reg;

  digitalWrite(this->display_pin_CS, LOW);

  if (this->spi_bus_mode == BUSMODE_SOFTWARE_SPI) {
    for (uint8_t i = maxbytes; i > 0; i--) {
      shiftOut(this->display_pin_MOSI, this->display_pin_CLK, MSBFIRST, spi_output_buffer[i - 1]);
    }
  } else if (this->spi_bus_mode == BUSMODE_HARDWARE_SPI) {
    this->hw_spi_bus->beginTransaction(SPISettings(this->spi_bus_speed, MSBFIRST, SPI_MODE0));
    for (uint8_t i = maxbytes; i > 0; i--) {
      this->hw_spi_bus->transfer(spi_output_buffer[i-1]);
    }
    this->hw_spi_bus->endTransaction();
  }

  digitalWrite(this->display_pin_CS, HIGH);
}
