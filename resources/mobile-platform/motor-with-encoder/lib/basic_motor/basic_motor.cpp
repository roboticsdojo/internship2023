#include <Arduino.h>

/*
 * Author: Automatic Addison
 * Website: https://automaticaddison.com
 * Description: Controls the speed and direction of two DC motors.
 */

// Motor A connections
const int enA = 53;
const int in1 = 51;
const int in2 = 49;

// Set the speed (0 = off and 255 = max speed)
const int motorSpeed = 230;

void motor_setup()
{

    Serial.begin(9600);

    // Motor control pins are outputs
    pinMode(enA, OUTPUT);
    pinMode(in1, OUTPUT);
    pinMode(in2, OUTPUT);

    // // Turn off motors - Initial state
    // digitalWrite(in1, LOW);
    // digitalWrite(in2, LOW);

    // // Set the motor speed
    // analogWrite(enA, motorSpeed);

    // Serial.println("Motor Initialized!");
    // stop_all();
}

/*
 *  Forwards, backwards, right, left, stop.
 */
void go_forward()
{
    // Serial.println("Going forwards!");
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
    analogWrite(enA, motorSpeed);
}
void go_backwards()
{
    // Serial.println("Going backwards!");
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(enA, motorSpeed);
}
void go_right()
{
    Serial.println("Going right!");
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
}
void go_left()
{
    Serial.println("Going left!");
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
}
void stop_all()
{
    Serial.println("Stopping!");
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
    analogWrite(enA, 0);
}
