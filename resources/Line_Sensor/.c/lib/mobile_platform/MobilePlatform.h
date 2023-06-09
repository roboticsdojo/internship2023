#ifndef MobilePlatform_h
#define MobilePlatform_h

#include <Arduino.h>

class MobilePlatform {
  public:
    MobilePlatform(int rightSensorPin, int leftSensorPin, int rightMotorPin1, int rightMotorPin2, int leftMotorPin1, int leftMotorPin2, int enableRightMotorPin, int enableLeftMotorPin);
    void setup();
    void loop();

  private:
    int rightSensorPin;
    int leftSensorPin;
    int rightMotorPin1;
    int rightMotorPin2;
    int leftMotorPin1;
    int leftMotorPin2;
    int enableRightMotorPin;
    int enableLeftMotorPin;
    void rotateMotor(int rightMotorSpeed, int leftMotorSpeed);
};

#endif
