#include <bozontlabsMAX7219.h>

#define DISP_SPI_MOSI 3
#define DISP_SPI_CLK 4
#define DISP_SPI_CS 5
#define DISP_NUMBER_OF_DEVICES 4

const int WIDTH = 32;
const int HEIGHT = 8;

bozontlabsMAX7219 max7219 = bozontlabsMAX7219(DISP_SPI_MOSI, DISP_SPI_CLK, DISP_SPI_CS, DISP_NUMBER_OF_DEVICES);
void plasmaEffect();

void setup() {
  Serial.begin(115200);
  max7219.begin();
  max7219.setBrightness(15);
  max7219.clearDisplay();
}

void loop() {
  plasmaEffect();
}

void plasmaEffect() {
  static bool currentFrame[WIDTH][HEIGHT];
  static bool nextFrame[WIDTH][HEIGHT];
  static bool initialized = false;

  static float t = 0.0f;
  static float dirX = 1.0f;       // current X multiplier
  static float dirY = 1.0f;       // current Y multiplier
  static float targetDirX = 1.0f; // target direction to interpolate to
  static float targetDirY = 1.0f;

  static int framesSinceChange = 0;

  const float speed = 0.1f;       // plasma motion speed
  const int changeInterval = 120; // frames before choosing a new target direction
  const float smoothing = 0.02f;  // how fast dirX/dirY interpolate

  // One-time init
  if (!initialized) {
    for (int x = 0; x < WIDTH; x++)
      for (int y = 0; y < HEIGHT; y++) {
        currentFrame[x][y] = false;
        nextFrame[x][y] = false;
        max7219.setPixel(x, y, false);
      }
    initialized = true;
  }

  // Occasionally choose a new target direction
  framesSinceChange++;
  if (framesSinceChange >= changeInterval) {
    framesSinceChange = 0;

    // Target values between -1.0 and 1.0, slightly randomized
    targetDirX = (random(60, 140) / 100.0f) * ((random(0, 2) == 0) ? 1 : -1);
    targetDirY = (random(60, 140) / 100.0f) * ((random(0, 2) == 0) ? 1 : -1);
  }

  // Interpolate current direction toward target
  dirX += (targetDirX - dirX) * smoothing;
  dirY += (targetDirY - dirY) * smoothing;

  // Generate next frame
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      float value = sin(x * 0.15f * dirX + t) + sin(y * 0.3f * dirY + t * 1.3f) + sin((x + y) * 0.08f + t * 0.7f);
      nextFrame[x][y] = (value > 0.3f); // lower threshold = bigger blobs
    }
  }

  // Apply differences only
  for (int x = 0; x < WIDTH; x++)
    for (int y = 0; y < HEIGHT; y++)
      if (nextFrame[x][y] != currentFrame[x][y]) {
        max7219.setPixel(x, y, nextFrame[x][y]);
        currentFrame[x][y] = nextFrame[x][y];
      }

  t += speed;
  delay(40);
}
