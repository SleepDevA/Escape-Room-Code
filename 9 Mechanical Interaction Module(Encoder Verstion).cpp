#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <SPI.h>

// 1.8" 128x160 RGB TFT LCD (8-pin)
const int tft_cs = 6;    // Chip Select
const int tft_dc = 7;    // Data/Command
const int tft_mosi = 11;  // SDA/MOSI
const int tft_sck = 13;   // SCL/SCK
const int tft_rst = 10;  // Reset
const int tft_led = 9;  // Backlight LED

// Initialize Adafruit ST7735 using Software SPI pins
Adafruit_ST7735 tft = Adafruit_ST7735(tft_cs, tft_dc, tft_mosi, tft_sck, tft_rst);

const int clk = 4;
const int dt = 2;
const int buzzer = 3;
const int relay = 5;
const int stepPixels = 10; // one encoder step = ten screen pixels
const int playerX = 55;
const int playerW = 18;
const int playerH = 15;
const int gapHeight = 104;
const int targetGapHeight = playerH * 3;
const unsigned long lineMoveInterval = 100; //speed of redline movement Lower number = Slower DIFFICULTY

int playerY = 70;
int topLineY = 12;
int bottomLineY = 12 + gapHeight;
int lastEncoded = 0;
int encoderPosition = 0;
int lineDirection = 1;
bool gapClosed = false;
bool gameOver = false;

unsigned long lastLineMoveTime = 0;
unsigned long gameStartTime = 0;
unsigned long buzzerOffAt = 0;
unsigned long gameOverAt = 0;

const signed char quadTable[16] = {
  0, -1,  1,  0,
  1,  0,  0, -1,
 -1,  0,  0,  1,
  0,  1, -1,  0
};

void setup() {
  pinMode(clk, INPUT_PULLUP);
  pinMode(dt, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(relay, OUTPUT);
  pinMode(tft_led, OUTPUT); // Set backlight pin as output

  digitalWrite(relay, LOW);
  digitalWrite(tft_led, HIGH); // Turn on backlight

  Serial.begin(9600);

  tft.initR(INITR_BLACKTAB);
  delay(150);  // Allow display time to initialize
  tft.setTextColor(ST7735_WHITE);
  resetGame();
}

void drawBounds() {
  tft.drawFastHLine(0, topLineY, 128, ST7735_RED);
  tft.drawFastHLine(0, bottomLineY, 128, ST7735_RED);
}

void drawPlayerBar(int x, uint16_t color) {
  tft.fillRect(playerX, x, playerW, playerH, color);
}

int readEncoderStep() {
  int msb = digitalRead(clk);
  int lsb = digitalRead(dt);
  int encoded = (msb << 1) | lsb;
  int transition = (lastEncoded << 2) | encoded;
  int step = quadTable[transition];

  if (encoded != lastEncoded) {
    lastEncoded = encoded;
  }

  if (step != 0) {
    encoderPosition += step;
    lastEncoded = encoded;

    if (encoderPosition >= 4) {
      encoderPosition = 0;
      return 1;
    }
    if (encoderPosition <= -4) {
      encoderPosition = 0;
      return -1;
    }
  }

  return 0;
}

void showGameOver() {
  gameOver = true;
  gameOverAt = millis();
  tft.fillScreen(ST7735_BLACK);
  tft.setCursor(12, 40);
  tft.setTextColor(ST7735_RED);
  tft.setTextSize(2);
  tft.print("GAME OVER");
  tft.setCursor(10, 70);
  tft.setTextColor(ST7735_WHITE);
  tft.setTextSize(1);
  tft.print("Rotate to restart");
}

void resetGame() {
  gameOver = false;
  gapClosed = false;
  topLineY = 12;
  bottomLineY = topLineY + gapHeight;
  lineDirection = 1;
  playerY = (topLineY + bottomLineY - playerH) / 2;
  lastLineMoveTime = millis();
  gameStartTime = millis();
  buzzerOffAt = 0;
  lastEncoded = (digitalRead(clk) << 1) | digitalRead(dt);

  tft.fillScreen(ST7735_BLACK);
  drawBounds();
  drawPlayerBar(playerY, ST7735_WHITE);
}

void loop() {
  if (gameOver) {
    if (readEncoderStep() != 0 || (millis() - gameOverAt > 2000)) {
      resetGame();
    }
    return;
  }

  int step = readEncoderStep();
  if (step != 0) {
    int oldY = playerY;
    playerY = constrain(playerY + step * stepPixels, 0, 160 - playerH);

    if (playerY != oldY) {
      drawPlayerBar(oldY, ST7735_BLACK);
      drawPlayerBar(playerY, ST7735_WHITE);
      buzzerOffAt = millis() + 60;
    }
  }

  if (millis() - lastLineMoveTime > lineMoveInterval) {
    int oldTop = topLineY;
    int oldBottom = bottomLineY;

    if (!gapClosed) {
      if (bottomLineY - topLineY > targetGapHeight) {
        topLineY += 1;
        bottomLineY -= 1;
      } else {
        gapClosed = true;
      }
    } else {
      if (topLineY + lineDirection < 0 || bottomLineY + lineDirection > 159) {
        lineDirection = -lineDirection;
      }
      topLineY += lineDirection;
      bottomLineY += lineDirection;
    }

    tft.drawFastHLine(0, oldTop, 128, ST7735_BLACK);
    tft.drawFastHLine(0, oldBottom, 128, ST7735_BLACK);
    drawBounds();

    lastLineMoveTime = millis();
  }

  if (playerY < topLineY || playerY + playerH > bottomLineY) {
    showGameOver();
    return;
  }

  if (millis() < buzzerOffAt) {
    digitalWrite(buzzer, HIGH);
  } else {
    digitalWrite(buzzer, LOW);
  }
}
