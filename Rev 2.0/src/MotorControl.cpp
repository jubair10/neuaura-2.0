#include "MotorControl.h"

void initializeMotors() {
  // Set Motor A pins as output
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  // Set Motor B pins as output
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
}

void setMotorA(int direction, int speed) {
  if (direction == 1) {  // Forward
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
  } else if (direction == -1) {  // Backward
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
  } else {  // Stop
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
  }
  analogWrite(ENA, speed);
}

void setMotorB(int direction, int speed) {
  if (direction == 1) {  // Forward
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
  } else if (direction == -1) {  // Backward
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
  } else {  // Stop
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
  }
  analogWrite(ENB, speed);
}

void moveForward(int speed) {
  setMotorA(1, speed);  // Motor A forward
  setMotorB(1, speed);  // Motor B forward
}

void moveBackward(int speed) {
  setMotorA(-1, speed);  // Motor A backward
  setMotorB(-1, speed);  // Motor B backward
}

void turnLeft(int speed) {
  setMotorA(-1, speed);  // Motor A backward
  setMotorB(1, speed);   // Motor B forward
}

void turnRight(int speed) {
  setMotorA(1, speed);   // Motor A forward
  setMotorB(-1, speed);  // Motor B backward
}

void stopMotors() {
  setMotorA(0, 0);  // Stop Motor A
  setMotorB(0, 0);  // Stop Motor B
}
