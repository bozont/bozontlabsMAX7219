#include <bozontlabsMAX7219.h>

#define DISP_SPI_MOSI 3
#define DISP_SPI_CLK 4
#define DISP_SPI_CS 5
#define DISP_NUMBER_OF_DEVICES 4

bozontlabsMAX7219 max7219 = bozontlabsMAX7219(DISP_SPI_CS, DISP_SPI_MOSI, DISP_SPI_CLK, DISP_NUMBER_OF_DEVICES);

void setup() {
  max7219.begin();
  max7219.setBrightness(2);
  max7219.clearDisplay();
}

void loop() {
  for(uint32_t y = 0; y < 8; y++) {
    for(uint32_t x = 0; x < 32; x++) {
      max7219.setPixel(x, y, true);
      delay(10);
    }
  }

  for(uint32_t y = 0; y < 8; y++) {
    for(uint32_t x = 0; x < 32; x++) {
      max7219.setPixel(x, y, false);
      delay(10);
    }
  }
}
