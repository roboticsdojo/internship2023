#ifndef MobilePlatform_h
#define MobilePlatform_h

#include <Arduino.h>
#include "Preferences.h"

class MobilePlatform {
  private:
    bool turning = false;
    bool stop = false;

    enum State {
      MOVING_FORWARD,
      TURNING_RIGHT,
      TURNING_LEFT,
      STOP,
      TURNING_180
    };

    State currentState = State();
    void rotateMotor(int rightMotorSpeed, int leftMotorSpeed);
    void setMotorState(int motorPin1, int motorPin2, int motorSpeed);
    int handleFarTurnValues();
    void handleSensorValues();
    void configurePins();
    void configurePWMFrequency();
    void setState();

    //Right: 1, left: -1, ignore 0
    int turns[50] = {1, -1, -1, -1, -1, -1, -1};
    int counter = 0;

  public:
    MobilePlatform();
    void setup();
    void loop();
    int getDistance();
};

#endif
