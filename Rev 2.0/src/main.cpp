#include <Arduino.h>
#include <Servo.h>
#include <MotorControl.h> 

#define MAX_BASE_SERVO 100
#define MIN_BASE_SERVO 60
#define OFFSET 5

#define IR_SENSOR_PIN A0
// #define IR_1_PIN 5
// #define IR_2_PIN 4
// #define IR_3_PIN A3
#define SPEED 150
// Servo Control Variables
Servo clawServo;
Servo baseServo1;
Servo baseServo2;

// mode to control arm state
bool controlClaw = true;

// Sonar Variables
// const int trigPin = 5;
// const int echoPin = 4;

enum State { FORWARD, RIGHT, LEFT, BACKWARD, STOP };
State currentState = STOP;

long duration;
int distance;
void handleClaw(const char command);
void handleBase(const char command);

void setup()
{
  // Initialize motor control
  initializeMotors();

  // Initialize serial communication
  Serial.begin(9600);

  // Initialize base servos
  baseServo1.attach(7);
  baseServo2.attach(6);

  baseServo1.write(60);
  baseServo2.write(125);

  // Attach servos
  clawServo.attach(13);
  clawServo.write(180);

  // Initialize sensors
  pinMode(IR_SENSOR_PIN, INPUT);
  // pinMode(trigPin, OUTPUT); 
  // pinMode(echoPin, INPUT);
}

void loop()
{
  char command;
  if (Serial.available())
  {
    command = Serial.read();
  }

  Serial.println();
  Serial.println();
  Serial.print(command);
  Serial.println();
  Serial.println();

  auto clawPos = clawServo.read();
  Serial.print("Claw Servo position: ");
  Serial.println(clawPos);

  auto b1 = baseServo1.read();
  Serial.print("Base Servo-1 position: ");
  Serial.println(b1);
  auto b2 = baseServo2.read();
  Serial.print("Base Servo-2 position: ");
  Serial.println(b2);
  Serial.print("\n\nControl Mode: ");
  Serial.println(controlClaw ? "Claw" : "Base");
  Serial.println();
  Serial.println();
  // Serial.println();

  // int distance = getSonarDistance();

  switch (command)
  {
  // case 'F':
  //   if(obstacle) moveForward(SPEED);
  case 'F':
    moveForward(SPEED);
      
    break;
  case 'B':
    moveBackward(SPEED);
    break;
  case 'L':
    turnLeft(SPEED);
    break;
  case 'R':
    turnRight(SPEED);
    break;
  case 'S':
    stopMotors();
    break;

  // case 'Q': // Toggle mode
  case 'X':
    controlClaw = false;
    break;
  case 'x':
    controlClaw = true;
    break;
  default:
    break;
  }

  if (controlClaw)
  {
    // Claw servo
    handleClaw(command);
  }
  else
  {
    handleBase(command);
  }

  delay(500);

}

void handleClaw(const char command)
{
  if ('0' <= command && command <= '9')
  {
    int cmd = command - '0';
    cmd = constrain(cmd, 0, 9);
    cmd = map(cmd, 0, 9, 105, 180);

    clawServo.write(cmd);
    delay(50);
  }
}

void handleBase(const char command)
{
  if ('0' <= command && command <= '9')
  {
    int cmd = command - '0';
    cmd = constrain(cmd, 0, 9);
    cmd = map(cmd, 0, 9, 60, 100);

    baseServo1.write(cmd);
    baseServo2.write(180 - cmd + OFFSET);
  }
}

// float getSonarDistance()
// {
//   digitalWrite(trigPin, LOW);
//   delayMicroseconds(2);
//   digitalWrite(trigPin, HIGH);
//   delayMicroseconds(8);
//   digitalWrite(trigPin, LOW);
//   duration = pulseIn(echoPin, HIGH);
//   return duration * 0.034 / 2; // Convert to distance in cm
// }

bool isObjectDetected() {
  return digitalRead(IR_SENSOR_PIN) == HIGH; // Return true if object is detected
}
// Function for manual claw control via serial
// void manualClawControl() {
//   if (Serial.available()) {
//     String cmd = Serial.readString();
//     int position = cmd.toInt();
//     clawServo.write(position); // Move claw to specified position
//     delay(500);
//   }
// }

// Function for manual base control via serial
// void manualBaseControl() {
//   if (Serial.available()) {
//     String cmd = Serial.readString();
//     int position = cmd.toInt();
//     baseServo.write(position);
//     delay(500);
//   }
// }

// #include <Arduino.h>
// #include <Servo.h>

// Servo servo1;

// void setup()
// {
//   Serial.begin(9600);

//   servo1.attach(8);
//   // servo2.attach(9);

//   servo1.write(90);
//   // servo2.write(0);
//   // Serial.println(s);
// }

// void loop()
// {

//   auto s = servo1.read();
//   Serial.print("Servo position: ");
//   Serial.println(s);
//   delay(500);

//   if(Serial.available()){
//     String cmd = Serial.readString();
//     int cm = cmd.toInt();
//     servo1.write(cm);
//     delay(500);
//   }

// }
