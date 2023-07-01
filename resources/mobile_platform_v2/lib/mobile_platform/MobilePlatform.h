#ifndef MobilePlatform_h
#define MobilePlatform_h

#include <Arduino.h>
#include "Preferences.h"
#include "Gyro.h"
#include "pickPlace.h"

class MobilePlatform {
  private:
    enum State {
      PICK_TRAILER,
      PICK_ENGINE,
      PICK_WHEELS,
      BACK_TO_CHASIS,
      PICK_CABIN,
      STOP
    };

    State currentState;
    
    void setMotorState(int motorPin1, int motorPin2, int motorSpeed);
    void configurePins();
    void configurePWMFrequency();


  public:
    void rotateMotor(int rightMotorSpeed, int leftMotorSpeed);
    void fastRotateMotor(int rightMotorSpeed, int leftMotorSpeed);
    MobilePlatform();
    Gyro gyro;
    void setup();
    void loop();
};

#endif
