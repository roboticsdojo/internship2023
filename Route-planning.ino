#include "MobilePlatform.h"

MobilePlatform::MobilePlatform() {
  turning = false;
}

void MobilePlatform::setup() {
  configurePins();
  configurePWMFrequency();
  rotateMotor(0, 0);
}

void MobilePlatform::configurePins() {
  pinMode(RIGHT_SENSOR, INPUT);
  pinMode(MIDDLE_SENSOR, INPUT);
  pinMode(LEFT_SENSOR, INPUT);

  pinMode(FAR_RIGHT, INPUT);
  pinMode(FAR_LEFT, INPUT);

  pinMode(ENABLE_LEFT_MOTOR, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN1, OUTPUT);
  pinMode(RIGHT_MOTOR_PIN2, OUTPUT);
  
  pinMode(ENABLE_RIGHT_MOTOR, OUTPUT);
  pinMode(LEFT_MOTOR_PIN1, OUTPUT);
  pinMode(LEFT_MOTOR_PIN2, OUTPUT);
}

void MobilePlatform::configurePWMFrequency() {
  // Change the frequency of PWM signal on pins D5 and D6 to 7812.5 Hz.
  TCCR0B = TCCR0B & (B11111000 | B00000010);
}

void MobilePlatform::loop() {
  handleFarTurnValues();
  handleSensorValues();
}

void MobilePlatform::handleFarTurnValues() {
  int farTurnValues[] = {digitalRead(FAR_RIGHT), digitalRead(FAR_LEFT)};

  if (farTurnValues[0]) {//Far right
    rotateMotor(-(-254), -254);
    delay(4500);   
   
}

void MobilePlatform::handleSensorValues() {
  int sensorValues[] = {
    digitalRead(LEFT_SENSOR),
    digitalRead(MIDDLE_SENSOR),
    digitalRead(RIGHT_SENSOR)
  };
  if (!turning) {
    if (!sensorValues[0] && !sensorValues[1] && !sensorValues[2])//0 0 0
    {
      // Serial.println("All white");
      rotateMotor(MOTOR_SPEED, MOTOR_SPEED);
    }
    else if (!sensorValues[0] && !sensorValues[1] && sensorValues[2])//0 0 1 first turn from the start point
    {
      // Serial.println("Correction: turn right");
      rotateMotor(MOTOR_SPEED, -MOTOR_SPEED);

    }
    else if (sensorValues[0] && sensorValues[1] && sensorValues[2])//1 1 1 first junction
    {
      // Serial.println("Forward");
      rotateMotor(MOTOR_SPEED, MOTOR_SPEED);
    }
    else if (sensorValues[0] && sensorValues[1] && sensorValues[2])//1 1 1  second junction
    {
      // Serial.println("Forward");
      rotateMotor(MOTOR_SPEED, MOTOR_SPEED);
    } // Stop, pick the engine
      //Turn 180 degrees 

    // Return journey
    if (sensorValues[0] && sensorValues[1] && sensorValues[2])//1 1 1 first junction
    {
      // Serial.println("Forward");
      rotateMotor(MOTOR_SPEED, MOTOR_SPEED);

    }
    else if (sensorValues[0] && sensorValues[1] && sensorValues[2])//1 1 1 second junction
    {
      // Serial.println("Turn right");
      rotateMotor(MOTOR_SPEED, -MOTOR_SPEED);
    } // Move forward to the chassis. Place engine
      // After placing engine, turn 90 degrees right. Move forward until reach line(all sensors read 1)
    if (sensorValues[0] && sensorValues[1] && sensorValues[2])//1 1 1
    {
      // Serial.println("90 deg left Turn");
      rotateMotor(-MOTOR_SPEED, MOTOR_SPEED);
      // delay(3000);
    }
    else if (sensorValues[0] && sensorValues[1] && sensorValues[2])//1 1 1
    {
      // Serial.println("90 deg right turn");
      rotateMotor(MOTOR_SPEED, -MOTOR_SPEED);
    }// Move forward to the wheel rack. Stop. Pick the wheels (assuming we'll carry all of them)
    //180 degree spin turn.
    //Return journey. 
    if (sensorValues[0] && sensorValues[1] && sensorValues[2])//1 1 1 first junction
    {
      // Serial.println("forward");
      rotateMotor(MOTOR_SPEED, MOTOR_SPEED);
      // delay(3000);
  }   //Move forward to the chassis. Place all wheels where intended
}

void MobilePlatform::rotateMotor(int rightMotorSpeed, int leftMotorSpeed) {
  setMotorState(RIGHT_MOTOR_PIN1, RIGHT_MOTOR_PIN2, rightMotorSpeed);
  setMotorState(LEFT_MOTOR_PIN1, LEFT_MOTOR_PIN2, leftMotorSpeed);
  
  analogWrite(ENABLE_RIGHT_MOTOR, abs(rightMotorSpeed));
  analogWrite(ENABLE_LEFT_MOTOR, abs(leftMotorSpeed));
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