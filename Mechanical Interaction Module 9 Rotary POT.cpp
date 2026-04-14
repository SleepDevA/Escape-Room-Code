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

const int potPin = A0;
const int buzzer = 3;
const int relay = 5;
const int playerW = 18;
const int playerH = 15;
const int gapWidth = 104;
const int targetGapWidth = playerH * 3;
const unsigned long lineMoveInterval = 100; //speed of redline movement Lower number = Slower DIFFICULTY

int playerX = 55;
const int playerY = 70;
int leftLineX = 12;
int rightLineX = 12 + gapWidth;
int lastPotValue = 0;
int lineDirection = 1;
bool gapClosed = false;
bool gameOver = false;

unsigned long lastLineMoveTime = 0;
unsigned long gameStartTime = 0;
unsigned long buzzerOffAt = 0;
unsigned long gameOverAt = 0;

void setup() {
  pinMode(potPin, INPUT);
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
  tft.drawFastVLine(leftLineX, 0, 160, ST7735_RED);
  tft.drawFastVLine(rightLineX, 0, 160, ST7735_RED);
}

void drawPlayerBar(int x, uint16_t color) {
  tft.fillRect(x, playerY, playerW, playerH, color);
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
  tft.print("Turn pot to restart");
}

void resetGame() {
  gameOver = false;
  gapClosed = false;
  leftLineX = 12;
  rightLineX = leftLineX + gapWidth;
  lineDirection = 1;
  lastLineMoveTime = millis();
  gameStartTime = millis();
  buzzerOffAt = 0;
  lastPotValue = analogRead(potPin);

  playerX = map(lastPotValue, 0, 1023, 0, 128 - playerW);
  playerX = constrain(playerX, 0, 128 - playerW);

  tft.fillScreen(ST7735_BLACK);
  drawBounds();
  drawPlayerBar(playerX, ST7735_WHITE);
}

void loop() {
  if (gameOver) {
    int potValue = analogRead(potPin);
    if (abs(potValue - lastPotValue) > 8 || (millis() - gameOverAt > 2000)) {
      resetGame();
    }
    return;
  }

  int potValue = analogRead(potPin);
  int oldX = playerX;
  playerX = map(potValue, 0, 1023, 0, 128 - playerW);
  playerX = constrain(playerX, 0, 128 - playerW);

  if (playerX != oldX) {
    drawPlayerBar(oldX, ST7735_BLACK);
    drawPlayerBar(playerX, ST7735_WHITE);
    buzzerOffAt = millis() + 60;
  }

  if (millis() - lastLineMoveTime > lineMoveInterval) {
    int oldLeft = leftLineX;
    int oldRight = rightLineX;

    if (!gapClosed) {
      if (rightLineX - leftLineX > targetGapWidth) {
        leftLineX += 1;
        rightLineX -= 1;
      } else {
        gapClosed = true;
      }
    } else {
      if (leftLineX + lineDirection < 0 || rightLineX + lineDirection > 127) {
        lineDirection = -lineDirection;
      }
      leftLineX += lineDirection;
      rightLineX += lineDirection;
    }

    tft.drawFastVLine(oldLeft, 0, 160, ST7735_BLACK);
    tft.drawFastVLine(oldRight, 0, 160, ST7735_BLACK);
    drawBounds();

    lastLineMoveTime = millis();
  }

  if (playerX < leftLineX || playerX + playerW > rightLineX) {
    showGameOver();
    return;
  }

  if (millis() < buzzerOffAt) {
    digitalWrite(buzzer, HIGH);
  } else {
    digitalWrite(buzzer, LOW);
  }
}
