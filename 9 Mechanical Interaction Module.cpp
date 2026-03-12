// This code is for a rotary encoder connected to the Nano. It's intended to be in a little toy car that you have to roll a certain distance to complete.
//It counts the number of turns and buzzes when a turn is detected.
//When the required number of turns is reached, it prints a message and resets the count.
// 

const int clk = 4; // clock line
const int dt = 2; // Data line I think
const int buzzer = 3;
const int relay = 5;
const int requiredmovement = 10; // Completion goal
int count = 0; // turn count
unsigned long lastChangeTime = 0;
unsigned long lastResetTime = 0;

void setup() {
  pinMode(clk, OUTPUT);
  pinMode(dt, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  pinMode(relay, OUTPUT);
  digitalWrite(clk, LOW);
  digitalWrite(relay, LOW);
  Serial.begin(9600);
}

void loop() {
  int dtValue = digitalRead(dt);
  if (dtValue == LOW) {
    count++;
    lastChangeTime = millis(); // update when count changes
    digitalWrite(buzzer, HIGH);
    delay(200); // You have to mess around with the delay and phisical gearing connected to the rotary encoder to accuratly mesure spins
    digitalWrite(buzzer, LOW);
    Serial.println(count);
  }

  if (count >= requiredmovement) { // Check for completion
    Serial.println("Required movement goal met");
    digitalWrite(relay, HIGH); // Activate relay
    count = 0;
    lastChangeTime = millis(); // reset timers on completion
    lastResetTime = millis();
  }

  // Reset count every 10 seconds if it hasn't changed in 3 seconds
  if (millis() - lastChangeTime > 3000 && millis() - lastResetTime > 15000) {
    count = 0;
    lastResetTime = millis();
    Serial.println("Count reset due to inactivity");
  }
}
