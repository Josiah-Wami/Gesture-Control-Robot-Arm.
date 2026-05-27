#include <Servo.h>

// Declare Servo objects for all 4 joints
Servo baseServo;
Servo shoulderServo;
Servo elbowServo;
Servo gripServo;

// Current real-time positions of the physical joints
int basePos = 90;
int shoulderPos = 90;
int elbowPos = 90;
int gripPos = 90;

// Independent target goal parameters for proper smooth-stepping math
int targetBase = 90;
int targetShoulder = 90;
int targetElbow = 90;
int targetGrip = 90;

unsigned long lastMove = 0;

void setup() {
  Serial.begin(9600);

  // Mapped exactly to your Adeept board specifications
  baseServo.attach(9);      // D9 / Servo 1 (Horizontal Rotation Axis)
  shoulderServo.attach(5);  // D5 / Servo 3 (Vertical Up/Down Pitch Axis)
  elbowServo.attach(3);     // D3 / Servo 4 (Vertical Up/Down Pitch Axis)
  gripServo.attach(11);     // D11 / Servo 5 (Gripper Open/Close Mechanical Jaw)

  // Move all axes to initial starting calibration positions
  baseServo.write(basePos);
  shoulderServo.write(shoulderPos);
  elbowServo.write(elbowPos);
  gripServo.write(gripPos);
  
  delay(1000);

  // POWER CUT: Shut down all signals immediately until text input arrives
  baseServo.detach();
  shoulderServo.detach();
  elbowServo.detach();
  gripServo.detach();
}

// Re-engineered to safely process incremental motor steps 1 degree at a time
void smoothMove(Servo &servo, int &current, int target, int minLimit, int maxLimit, int pinNum) {
  target = constrain(target, minLimit, maxLimit);

  if (current != target) {
    // Dynamically attach only when active movement is required
    if (!servo.attached()) {
      servo.attach(pinNum);
    }
    
    if (current < target) {
      current += 1;
    } else if (current > target) {
      current -= 1;
    }
    servo.write(current);
  } else {
    // Immediately detach the single motor once it arrives at its target to save power
    if (servo.attached()) {
      servo.detach();
    }
  }
}

void loop() {
  if (Serial.available()) {
    String cmd = Serial.readStringUntil('\n');
    cmd.trim();

    // Small step sizes (5 degrees) protect your supply lines from dropping out
    if (cmd == "LEFT") {
      targetBase = constrain(targetBase + 5, 10, 170); 
    }
    else if (cmd == "RIGHT") {
      targetBase = constrain(targetBase - 5, 10, 170); 
    }
    else if (cmd == "UP") {
      targetShoulder = constrain(targetShoulder + 5, 40, 140); 
    }
    else if (cmd == "DOWN") {
      targetShoulder = constrain(targetShoulder - 5, 40, 140); 
    }
    else if (cmd == "OPEN") {
      targetElbow = constrain(targetElbow + 5, 30, 150); 
    }
    else if (cmd == "CLOSE") {
      targetElbow = constrain(targetElbow - 5, 30, 150); 
    }
    else if (cmd == "GRIP_OPEN") {
      targetGrip = constrain(targetGrip + 5, 40, 85); 
    }
    else if (cmd == "GRIP_CLOSE") {
      targetGrip = constrain(targetGrip - 5, 40, 85); 
    }
  }

  // LOW POWER CONCURRENCY: Removed structural nested IF locks. 
  // All 4 joints now monitor adjustments simultaneously every 45 milliseconds.
  if (millis() - lastMove > 45) {
    
    smoothMove(baseServo, basePos, targetBase, 10, 170, 9);
    smoothMove(shoulderServo, shoulderPos, targetShoulder, 40, 140, 5);
    smoothMove(elbowServo, elbowPos, targetElbow, 30, 150, 3);
    smoothMove(gripServo, gripPos, targetGrip, 40, 85, 11);
    
    lastMove = millis();
  }
}
