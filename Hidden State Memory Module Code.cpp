const int b1 = 2;
const int b2 = 3;
const int b3 = 9;
const int pot = A1;
const int led = 8;
int potValue1 = 0;
int stage = 0;

void setup() {
    Serial.begin(9600);
  pinMode(b1, INPUT_PULLUP);
  pinMode(b2, INPUT_PULLUP);
  pinMode(b3, INPUT_PULLUP);
  pinMode(led, OUTPUT);
  pinMode(pot, INPUT);
}

void loop() {
    int potValue = analogRead(pot);
    potValue1 = map(potValue, 0, 1023, 0, 100); // map to percentage for easier debugging
    Serial.print("Potentiometer value: ");
    Serial.println(potValue1);
    delay(100); // small delay to avoid flooding the serial output

    bool potOK = (potValue >= 200 && potValue <= 400);

    // only move to stage 1 once and when starting from stage 0
    if (stage == 0 && potOK) {
        Serial.println("Stage 1");
        stage = 1;
    }

    // advance to next stage only on button press when at the correct stage
    if (stage == 1 && digitalRead(b1) == LOW && potOK) {
        Serial.println("Button 1 pressed");
        stage = 2;
    }
    if (stage == 2 && digitalRead(b2) == LOW && potOK) {
        Serial.println("Button 2 pressed");
        stage = 3;
    }
    if (stage == 3 && digitalRead(b3) == LOW && potOK) {
        Serial.println("Button 3 pressed");
        stage = 4;
    }

    if (stage == 4) {
        Serial.println("Completed");
        digitalWrite(led, HIGH);
    }
}