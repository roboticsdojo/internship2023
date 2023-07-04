#include "MobilePlatform.h"
#include "Gyro.h"

MobilePlatform::MobilePlatform() {
}

void MobilePlatform::setup() {
  gyro.setup();
  configurePins();
  configurePWMFrequency();
  // rotateMotor(0, 0);
}

void MobilePlatform::loop() {
}

void MobilePlatform::configurePins() {
  pinMode(RIGHT_SENSOR, INPUT);
  // pinMode(MIDDLE_SENSOR, INPUT);
  pinMode(LEFT_SENSOR, INPUT);

  // pinMode(FAR_RIGHT, INPUT);
  // pinMode(FAR_LEFT, INPUT);

  pinMode(ENABLE_LEFT_MOTOR, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  
  pinMode(ENABLE_RIGHT_MOTOR, OUTPUT);
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);

  pinMode(PICKPIN, OUTPUT);
  pinMode(DEBUGPIN, OUTPUT);
  pinMode(CONTINUEPIN, INPUT_PULLUP);

  // pinMode(TRIGPIN, OUTPUT);
  // pinMode(ECHOPIN, INPUT);  
}

void MobilePlatform::configurePWMFrequency() {
  // Change the frequency of PWM signal on pins D5 and D6 to 7812.5 Hz.
  // TCCR0B = TCCR0B & (B11111000 | B00000010);
  // Change the frequency of PWM signal on pins D2, D3, D5 to 7812.5 Hz.
  // TCCR3B = TCCR3B & (B11111000 | B00000010);
  // Change the frequency of PWM signal on pins D44, D45, D46 to 7812.5 Hz.
  TCCR4B = TCCR4B & (B11111000 | B00000010);
}


void MobilePlatform::rotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
  // cap max speed
  if (rightMotorSpeed > 255) rightMotorSpeed = 200;
  if (rightMotorSpeed < -255) rightMotorSpeed = -200;
  if (leftMotorSpeed > 255) leftMotorSpeed = 200;
  if (leftMotorSpeed < -255) leftMotorSpeed = -200;
  if (rightMotorSpeed > -200 && rightMotorSpeed <= 0) rightMotorSpeed = -180;
  if (rightMotorSpeed < 200 && rightMotorSpeed >= 0) rightMotorSpeed = 180;
  if (leftMotorSpeed > -200 && leftMotorSpeed <= 0) leftMotorSpeed = -180;
  if (leftMotorSpeed < 200 && leftMotorSpeed >= 0) leftMotorSpeed = 180;
  
  setMotorState(RIGHT_MOTOR_PIN1, RIGHT_MOTOR_PIN2, rightMotorSpeed);
  setMotorState(LEFT_MOTOR_PIN1, LEFT_MOTOR_PIN2, leftMotorSpeed);
  
  analogWrite(ENABLE_RIGHT_MOTOR, abs(rightMotorSpeed));
  analogWrite(ENABLE_LEFT_MOTOR, abs(leftMotorSpeed));
  
}

void MobilePlatform::fastRotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
  if (rightMotorSpeed > 255) rightMotorSpeed = 230;
  if (rightMotorSpeed < -255) rightMotorSpeed = -230;
  if (leftMotorSpeed > 255) leftMotorSpeed = 230;
  if (leftMotorSpeed < -255) leftMotorSpeed = -230;
  if (rightMotorSpeed > -200 && rightMotorSpeed <= 0) rightMotorSpeed = -230;
  if (rightMotorSpeed < 200 && rightMotorSpeed >= 0) rightMotorSpeed = 230;
  if (leftMotorSpeed > -200 && leftMotorSpeed <= 0) leftMotorSpeed = -230;
  if (leftMotorSpeed < 200 && leftMotorSpeed >= 0) leftMotorSpeed = 230;
  
  analogWrite(ENABLE_RIGHT_MOTOR, abs(rightMotorSpeed));
  analogWrite(ENABLE_LEFT_MOTOR, abs(leftMotorSpeed));
  
  setMotorState(RIGHT_MOTOR_PIN1, RIGHT_MOTOR_PIN2, rightMotorSpeed);
  setMotorState(LEFT_MOTOR_PIN1, LEFT_MOTOR_PIN2, leftMotorSpeed);
  
}

void MobilePlatform::setMotorState(int motorPin1, int motorPin2, int motorSpeed) {
  if (motorSpeed < 0) {
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
  } else if (motorSpeed > 0) {
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
  } else {
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
  }
}
