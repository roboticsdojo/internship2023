#include <Arduino.h>
#include <Servo.h>

// Define the servos for the robotic arm
Servo j1Servo;
Servo j2Servo;
Servo j3Servo;
Servo j4Servo;

#define JOINT_1 2
#define JOINT_2 3
#define JOINT_3 4
#define JOINT_4 51

// Initial angles
float initialTheta1 = 90.0;
float initialTheta2 = 90.0;
float initialTheta3 = 90.0;
float initialTheta4 = 0.0;

void moveServo(int angle, Servo servo);
void moveToPos(double x, double y, double z);

void setup()
{
  Serial.begin(9600);
  j1Servo.attach(JOINT_1, 750, 2600);
  j2Servo.attach(JOINT_2, 750, 2600);
  j3Servo.attach(JOINT_3, 750, 2600);
  j4Servo.attach(JOINT_4, 750, 2600);

  j1Servo.write(initialTheta1);
  j2Servo.write(initialTheta2);
  j3Servo.write(initialTheta3);

  delay(3000);
  // //Sequence 1
  // moveServo(-80, j2Servo);
  // moveServo(20, j3Servo);
  // delay(3000);
  // moveServo(0, j2Servo);
  // moveServo(0, j3Servo);
  // j2Servo.writeMicroseconds(750);
  // delay(5000);
  // j2Servo.writeMicroseconds(2600);
  
}
void loop()
{
  int z = 0;
  // for (z = 0; z <= 200; z+=50)
  // {
    // moveToPos(0, 100, 0);
    // delay(100);
  // }
  // moveServo(-90, j3Servo);
  moveToPos(0, 200, 0);
  delay(3000);
  moveToPos(0,200,(200 / 2)); // devide by 2 to correct error
  delay(6000);
  // delay(100);
}

void moveToPos(double x, double y, double z)
{
  double b = atan2(x, y) * (180 / 3.1415); // base angle

  double l = sqrt(x * x + y * y); // x and y extension

  double h = sqrt(l * l + z * z);

  double phi = atan(z / l) * (180 / 3.1415);

  double theta = acos((h / 2) / 150) * (180 / 3.1415); // perfect

  double a1 = phi + theta; // angle for first part of the arm
  double a2 = phi - theta; // angle for second part of the arm

  // moveToAngle(b,a1,a2,g);
  Serial.print("b: ");
  Serial.println(b);
  Serial.print("a1: ");
  Serial.println(-(a1));
  Serial.print("a2: ");
  Serial.println(a2);
  Serial.println();
  // delay(3000);
  moveServo(b, j1Servo);
  // if (z == 0)
  moveServo(-(90 - a1), j2Servo);
  // else
    // moveServo((-(90 - a1)), j2Servo);
  if (z == 0)
    moveServo(90 - (180 - (a1 * 2)), j3Servo);
  else
    moveServo(-90 + (180 - (a1 * 2)), j3Servo);

}

void moveServo(int angle, Servo servo)
{

  // int servoAngle = 90 - angle;
  int servoAngle = map(angle, 90, -90, 750, 2600);
  // Serial.println(servoAngle);

  // if (servoAngle > 180 || servoAngle < 0)
    // Serial.println("Angle out of range");

  // constrain the angle to the range 0-180
  // servoAngle = constrain(servoAngle, 0, 180);

  // move the servo to the specified angle
  // servo.write(servoAngle);
  servo.writeMicroseconds(servoAngle);
}
