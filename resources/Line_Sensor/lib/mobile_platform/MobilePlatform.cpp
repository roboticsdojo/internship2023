#include "MobilePlatform.h"

#define MOTOR_SPEED 150

MobilePlatform::MobilePlatform(int rightSensorPin, int middleSensorPin, int leftSensorPin, int rightMotorPin1, int rightMotorPin2, int leftMotorPin1, int leftMotorPin2, int enableRightMotorPin, int enableLeftMotorPin) {
  this->rightSensorPin = rightSensorPin;
  this->middleSensorPin = middleSensorPin;
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
  int sensorValues[] = {
    digitalRead(leftSensorPin),
    digitalRead(middleSensorPin),
    digitalRead(rightSensorPin)
  };

  if (!sensorValues[0] && !sensorValues[1] && !sensorValues[2])//0 0 0
  {
    Serial.println("All white");
    rotateMotor(0, 0);
  }
  else if (!sensorValues[0] && !sensorValues[1] && sensorValues[2])//0 0 1
  {
    // Serial.println("Correction: turn right");
    rotateMotor(-MOTOR_SPEED, MOTOR_SPEED);
  }
  else if (!sensorValues[0] && sensorValues[1] && !sensorValues[2])//0 1 0
  {
    // Serial.println("Forward");
    rotateMotor(MOTOR_SPEED, MOTOR_SPEED);// Right  Left
  }
  else if (!sensorValues[0] && sensorValues[1] && sensorValues[2])//0 1 1
  {
    Serial.println("90 deg right Turn");
    rotateMotor(-MOTOR_SPEED, MOTOR_SPEED);
    // delay(3000);
  }
  else if (sensorValues[0] && !sensorValues[1] && !sensorValues[2])//1 0 0
  {
    // Serial.println("Correction: turn left");
    rotateMotor(MOTOR_SPEED, -MOTOR_SPEED);
  }
  else if (sensorValues[0] && !sensorValues[1] && sensorValues[2])//1 0 1
  {
    Serial.println("Impossible");
    rotateMotor(0, 0);
  }
  else if (sensorValues[0] && sensorValues[1] && !sensorValues[2])//1 1 0
  {
    Serial.println("90 deg left Turn");
    rotateMotor(MOTOR_SPEED, -MOTOR_SPEED);
    // delay(3000);
  }
  else if (sensorValues[0] && sensorValues[1] && sensorValues[2])//1 1 1
  {
    // Serial.println("Sharp Left/Sharp Right");
    rotateMotor(0, 0);
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
