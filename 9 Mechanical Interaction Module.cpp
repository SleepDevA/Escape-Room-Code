// This code is for a rotary encoder connected to the Nano. It's intended to be in a little toy car that you have to roll a certain distance to complete.
//It counts the number of turns and buzzes when a turn is detected.
//When the required number of turns is reached, it prints a message and resets the count.
// 

const int clk = 4; // clock line
const int dt = 2; // Data line I think
const int buzzer = 3;
const int required movement = 10; // Completetion goal
int count = 0; // turn count

void setup() {
  pinMode(clk, OUTPUT);
  pinMode(dt, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  digitalWrite(clk, LOW);
  Serial.begin(9600);
}

void loop() {
  int dtValue = digitalRead(dt);
  if (dtValue == LOW) {
    count++;
    digitalWrite(buzzer, HIGH);
    delay(200); // You have to mess around with the delay and phisical gearing connected to the rotary encoder to accuratly mesure spins
    digitalWrite(buzzer, LOW);
    Serial.println(count);
  }
}

if (count >= required movement) { // Check for completion
  
  Serial.println("Required movement goal met");
  count = 0; 
}
