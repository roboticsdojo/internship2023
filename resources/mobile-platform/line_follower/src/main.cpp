#include <Arduino.h>
#define MOTOR_SPEED 150

// IR Sensor. Normally LOW, HIGH when detects black line
#define right_sensor 19
#define left_sensor 20

// Motor Driver
#define enableLeftMotor 4
#define enableRightMotor 5

//Left motor
int leftMotorPin1=8;
int leftMotorPin2=9;

//Right motor
int rightMotorPin1=10;
int rightMotorPin2=11;

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed);

//******* alt code
// #include "MobilePlatform.h"

// MobilePlatform platform(19, 20, 10, 11, 8, 9, 5, 4);


void setup() 
{
  //******* alt code
  // platform.setup();

  Serial.begin(9600);
  pinMode(right_sensor, INPUT);
  pinMode(left_sensor, INPUT);

  //The problem with TT gear motoLOWrs is that, at very low pwm value it does not even rotate.
  //If we increase the PWM value then it rotates faster and our robot is not controlled in that speed and goes out of line.
  //For that we need to increase the frequency of analogWrite.
  //Below line is important to change the frequency of PWM signal on pin D5 and D6
  //Because of this, motor runs in controlled manner (lower speed) at high PWM value.
  //This sets frequecny as 7812.5 hz.
  TCCR0B = TCCR0B & (B11111000 | B00000010) ;
  
  // put your setup code here, to run once:
  pinMode(enableLeftMotor, OUTPUT);
  pinMode(rightMotorPin1, OUTPUT);
  pinMode(rightMotorPin2, OUTPUT);
  
  pinMode(enableRightMotor, OUTPUT);
  pinMode(leftMotorPin1, OUTPUT);
  pinMode(leftMotorPin2, OUTPUT);

  rotateMotor(0,0);  
  // Start motor
  // analogWrite(enableLeftMotor, MOTOR_SPEED);
  // analogWrite(enableRightMotor, MOTOR_SPEED);

}

void loop() 
{
  // platform.loop();
  //******* alt code
  
  // Forward
  // digitalWrite(rightMotorPin1,HIGH);
  // digitalWrite(rightMotorPin2,LOW);
  // digitalWrite(leftMotorPin1,HIGH);
  // digitalWrite(leftMotorPin2,LOW);
  // IR Sensor. Normally LOW, HIGH when detects black line
  if (digitalRead(right_sensor) == 0 && digitalRead(left_sensor) == 0)
  {
    // Serial.println("Forward");
    rotateMotor(MOTOR_SPEED,MOTOR_SPEED);
    
  }
  else if (digitalRead(right_sensor) == 0 && digitalRead(left_sensor) == 1)
  {
    // Serial.println("Right");
    rotateMotor(MOTOR_SPEED, -MOTOR_SPEED);
  }
  else if (digitalRead(right_sensor) == 1 && digitalRead(left_sensor) == 0)
  {
    // Serial.println("Left");
    rotateMotor(-MOTOR_SPEED, MOTOR_SPEED);
  }
  else
  {
    // Serial.println("Stop");
    rotateMotor(0,0);
  }
  // Serial.println("Right: " + String(digitalRead(right_sensor)));
  // Serial.println("Left: " + String(digitalRead(left_sensor)));
  // // Serial.println("Left: " + String(digitalRead(left_sensor)));
  // Serial.println(" ");
}

void rotateMotor(int rightMotorSpeed, int leftMotorSpeed)
{
  
  if (rightMotorSpeed < 0)
  {
    digitalWrite(rightMotorPin1,LOW);
    digitalWrite(rightMotorPin2,HIGH);    
  }
  else if (rightMotorSpeed > 0)
  {
    digitalWrite(rightMotorPin1,HIGH);
    digitalWrite(rightMotorPin2,LOW);      
  }
  else
  {
    digitalWrite(rightMotorPin1,LOW);
    digitalWrite(rightMotorPin2,LOW);      
  }

  if (leftMotorSpeed < 0)
  {
    digitalWrite(leftMotorPin1,LOW);
    digitalWrite(leftMotorPin2,HIGH);    
  }
  else if (leftMotorSpeed > 0)
  {
    digitalWrite(leftMotorPin1,HIGH);
    digitalWrite(leftMotorPin2,LOW);      
  }
  else 
  {
    digitalWrite(leftMotorPin1,LOW);
    digitalWrite(leftMotorPin2,LOW);      
  }
  analogWrite(enableRightMotor, abs(rightMotorSpeed));
  analogWrite(enableLeftMotor, abs(leftMotorSpeed));    
}
