#include <Servo.h>

// Array mapping: 0 = Base, 1 = Gripper
Servo servos[2];

int pins[2]    = {9, 11};   // Base on D9, Gripper on D11
int pos[2]     = {90, 90};
int targets[2] = {90, 90};
int mins[2]    = {10, 40};  // Mechanical safety minimums
int maxs[2]    = {170, 85}; // Mechanical safety maximums

unsigned long lastMove = 0;
String inputBuffer = ""; // Global buffer for incoming data

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 2; i++) {
    servos[i].attach(pins[i]);
    servos[i].write(pos[i]);
  }
  delay(1000);
  for (int i = 0; i < 2; i++) servos[i].detach();
}

void smoothMove(int i) {
  targets[i] = constrain(targets[i], mins[i], maxs[i]);
  if (pos[i] != targets[i]) {
    if (!servos[i].attached()) servos[i].attach(pins[i]);
    pos[i] += (pos[i] < targets[i]) ? 1 : -1;
    servos[i].write(pos[i]);
  } else if (servos[i].attached()) {
    servos[i].detach(); // Full low-power cutoff at target destination
  }
}

void parseCommand(String cmd) {
  cmd.trim();
  if      (cmd == "LEFT")        targets[0] += 5;
  else if (cmd == "RIGHT")       targets[0] -= 5;
  else if (cmd == "GRIP_OPEN")   targets[1] += 5;
  else if (cmd == "GRIP_CLOSE")  targets[1] -= 5;
}

void loop() {
  // Non-blocking serial read
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      parseCommand(inputBuffer);
      inputBuffer = ""; // Clear buffer for next command
    } else {
      inputBuffer += c; // Append character to buffer
    }
  }

  // Smooth movement interval
  if (millis() - lastMove > 45) {
    for (int i = 0; i < 2; i++) smoothMove(i);
    lastMove = millis();
  }
}
