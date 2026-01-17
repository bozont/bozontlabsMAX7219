#include <bozontlabsMAX7219.h>

#define DISP_SPI_MOSI 3
#define DISP_SPI_CLK 4
#define DISP_SPI_CS 5
#define DISP_NUMBER_OF_DEVICES 4

bozontlabsMAX7219 max7219 = bozontlabsMAX7219(DISP_SPI_MOSI, DISP_SPI_CLK, DISP_SPI_CS, DISP_NUMBER_OF_DEVICES);

void setup() {
  max7219.begin();
  max7219.setBrightness(2);
  max7219.clearDisplay();
}

void loop() {
  uint8_t x = random(0, 32);
  uint8_t y = random(0, 8);
  max7219.setPixel(x, y, true);
  delay(500);
  max7219.setPixel(x, y, false);
}
