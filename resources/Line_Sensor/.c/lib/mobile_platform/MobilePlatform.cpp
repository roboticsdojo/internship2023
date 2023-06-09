#include "MobilePlatform.h"

#define MOTOR_SPEED 150

MobilePlatform::MobilePlatform(int rightSensorPin, int leftSensorPin, int rightMotorPin1, int rightMotorPin2, int leftMotorPin1, int leftMotorPin2, int enableRightMotorPin, int enableLeftMotorPin) {
  this->rightSensorPin = rightSensorPin;
  this->leftSensorPin = leftSensorPin;
  this->rightMotorPin1 = rightMotorPin1;
  this->rightMotorPin2 = rightMotorPin2;
  this->leftMotorPin1 = leftMotorPin1;
  this->leftMotorPin2 = leftMotorPin2;
  this->enableRightMotorPin = enableRightMotorPin;
  this->enableLeftMotorPin = enableLeftMotorPin;
}

void MobilePlatform::setup() {
  pinMode(rightSensorPin, INPUT);
  pinMode(leftSensorPin, INPUT);
  pinMode(enableRightMotorPin, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  pinMode(enableLeftMotorPin, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);
  rotateMotor(0,0);
}

void MobilePlatform::loop() {
  if (digitalRead(rightSensorPin) == 0 && digitalRead(leftSensorPin) == 0) {
    rotateMotor(MOTOR_SPEED,MOTOR_SPEED);
  }
  else if (digitalRead(rightSensorPin) == 0 && digitalRead(leftSensorPin) == 1) {
    rotateMotor(MOTOR_SPEED, -MOTOR_SPEED);
  }
  else if (digitalRead(rightSensorPin) == 1 && digitalRead(leftSensorPin) == 0) {
    rotateMotor(-MOTOR_SPEED, MOTOR_SPEED);
  }
  else {
    rotateMotor(0,0);
  }
}

void MobilePlatform::rotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
  digitalWrite(rightMotorPin1, rightMotorSpeed > 0 ? HIGH : LOW);
  digitalWrite(rightMotorPin2, rightMotorSpeed < 0 ? HIGH : LOW);
  digitalWrite(leftMotorPin1, leftMotorSpeed > 0 ? HIGH : LOW);
  digitalWrite(leftMotorPin2, leftMotorSpeed < 0 ? HIGH : LOW);
  analogWrite(enableRightMotorPin, abs(rightMotorSpeed));
  analogWrite(enableLeftMotorPin, abs(leftMotorSpeed));
}
