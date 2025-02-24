#ifndef MOTORCONTROL_H
#define MOTORCONTROL_H

#include <Arduino.h>

// Motor A pins
#define IN1 11
#define IN2 10
#define ENA A1

// Motor B pins
#define IN3 9
#define IN4 8
#define ENB A0

// Function prototypes
void initializeMotors();
void setMotorA(int direction, int speed);
void setMotorB(int direction, int speed);
void moveForward(int speed);
void moveBackward(int speed);
void turnLeft(int speed);
void turnRight(int speed);
void stopMotors();

#endif // MOTORCONTROL_H
