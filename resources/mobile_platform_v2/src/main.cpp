#include <Arduino.h>

#include <NewPing.h>
#include "MobilePlatform.h"
#include "Preferences.h"
#include <Encoder.h>
#include <Wire.h>
# include "pickPlace.h"

MobilePlatform platform;

void moveCar(int speedLeft, int speedRight);
void rotateToAngle(float target_angle, float tolerance, bool fast = false);


void moveDistance(double distance_mm, bool detectLine = false);

int minSpeed = 200; // Minimum motor speed
int maxSpeed = 255; // Maximum motor speed

// MPU 6050
const int MPU = 0x68;                                           // MPU6050 I2C address
float AccX, AccY, AccZ;                                         // linear acceleration
float GyroX, GyroY, GyroZ;                                      // angular velocity
float accAngleX, accAngleY, gyroAngleX, gyroAngleY, gyroAngleZ; // used in void loop()
float roll, pitch, yaw;
float AccErrorX, AccErrorY, GyroErrorX, GyroErrorY, GyroErrorZ;
float elapsedTime, currentTime, previousTime;
int c = 0;

float angle; // due to how I orientated my MPU6050 on my car, angle = yaw
float targetAngle = 0;

// prototypes
void calculateError();
void readAcceleration();
void readGyro();
void setGyroReadings();

// Global variables
// double Kp = 1.00, Ki = 0.001, Kd = 20.00; // Tune these values
// double Kp = 8.00, Ki = 25.00, Kd = 35.00; // Tune these values
// double Kp = 0.2, Ki = 0.000, Kd = 500.00; // Tune these values
double Kp = 0.5, Ki = 0.0002, Kd = 100.00; // Tune these values


double integral = 0, previous_error = 0;

// double Kp_line = 1.0, Ki_line = 0.0, Kd_line = 5.0; // Tune these values
double Kp_line = 6.0, Ki_line = 0.0002, Kd_line = 20.0; // Tune these values
double integral_line = 0, previous_error_line = 0;

double PIDController(double target_heading);
void rotateToAngle(double target_angle); // This is not in use // can be removed

void followLine(int stopDistance, bool fastFollow = false);

enum State
{
  PICK_TRAILER,
  PICK_ENGINE,
  PICK_WHEELS,
  BACK_TO_CHASIS,
  PICK_CABIN,
  CABIN_TO_CHASIS,
  BACK_TO_START,
  STOP
};
void customDelay(unsigned long milliseconds);
unsigned long PREVIOUS_TIME;
State state;
bool done;
float mapToMotorSpeed(float pid_output);

// Line following

double PIDControllerLine();

// Ultra sonic and distance stuff
#define MAX_DISTANCE 400 // Maximum distance we want to ping for (in centimeters).

NewPing sonar(TRIGPIN, ECHOPIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
NewPing sonarBack(TRIGPINBACK, ECHOPINBACK, MAX_DISTANCE);

unsigned int pingSpeed = 100; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;      // Holds the next ping time.
volatile int distance = 400;
int distance_counter = 0;
unsigned long previous_time_distance;

// Prototypes
void echoCheck();
void echoCheckBack();
void fastRotate(int target_angle);
float mapToMotorSpeed2(float pid_output);

void setup()
{
  platform.setup();
  Serial.begin(9600);

  // ultrasonic
  pingTimer = millis(); // Start now.

  // gyro
  Wire.begin();                // Initialize comunication
  Wire.beginTransmission(MPU); // Start communication with MPU6050 // MPU=0x68
  Wire.write(0x6B);            // Talk to the register 6B
  Wire.write(0x00);            // Make reset - place a 0 into the 6B register
  Wire.endTransmission(true);  // end the transmission

  // Call this function if you need to get the IMU error values for your module
  calculateError();
  // delay(20);
  customDelay(1000);

  state = PICK_TRAILER;
  // state = PICK_CABIN;

  // Test the sensors
  // while (true) {
  // setGyroReadings();
  // Serial.print("yaw: ");
  // Serial.println(yaw);


  // followLine(0); Ultrasonic ok
  // moveCar(180, 180);

  // }
  // while (true) {
  //   Serial.print("LEFT_SENSOR: ");
  //   Serial.print(analogRead(LEFT_SENSOR));
  //   Serial.print("\t||\tRIGHT_SENSOR: ");
  //   Serial.println(analogRead(RIGHT_SENSOR));
  // }

  // followLine(50);
  // moveCar(254, 254);
  // moveDistance(4000);
  // moveCar(-240, -240);
  // customDelay(5000);
  // moveCar(240, -240);
  // while (true)
  // {
  //   Serial.println("stop");
  //   customDelay(1000);
  // }
  // while(true) {
  //   setGyroReadings();
  //   Serial.println(yaw);
  // }
  // while (true) {
  //   if (millis() >= pingTimer)
  //   {
  //     pingTimer += pingSpeed;      // Set the next ping time.
  //     sonarBack.ping_timer(echoCheckBack); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
  //   }
  //   Serial.println(distance);
  // }
}
void loop()
{
  setGyroReadings();
  switch (state)
  {
  case PICK_TRAILER:
    rotateToAngle(0, 10);
    customDelay(100);
    // moveDistance(185);
    moveCar(200, 200);
    customDelay(6000);
    moveCar(0, 0);
    customDelay(100);
    
    //Rotate
    for (int i = 0; i < 90; i += 5) {
      rotateToAngle(i, 2);
      customDelay(10);
    }
    customDelay(1000);
    rotateToAngle(90, 10) ;
    
    getPinValue();
    customDelay(2000); // wait for the trailer to be picked
    state = PICK_ENGINE;
    break;
  case PICK_ENGINE:
    // Rotate
    for (int i = angle; i > -150; i -= 5) {
      rotateToAngle(i, 2);
      customDelay(10);
    }
    customDelay(1000);
    rotateToAngle(-150, 10);

    customDelay(1000);
    targetAngle = -(150);
    moveDistance(1000, true); // move and detect line // first line
    customDelay(100);
    minSpeed = 100;
    maxSpeed = 100;
    moveDistance(100);        // Move past line
    moveDistance(1000, true); // move and detect second line
    customDelay(100);

    // moveCar(200, 200);
    // customDelay(100);
    // moveCar(0,0);
     // Rotate
    for (int i = angle; i < -100; i += 5) {
      rotateToAngle(i, 2);
      customDelay(10);
    }
    // customDelay(1000);
    // rotateToAngle(-90, 10);
    customDelay(100);

    followLine(50); // follow line. Stop at obstacle_distance <= 50 cm
    getPinValue();
    customDelay(2000); // wait to pick engine
    state = PICK_WHEELS;
    // state = STOP;
    break;
  case PICK_WHEELS:
    for (int i = angle; i < 0; i += 5) {
      rotateToAngle(i, 2);
      customDelay(10);
    }
    customDelay(1000);
    rotateToAngle(0, 10);
    customDelay(100);
    
    // moveDistance(50); // Move past line, just in case
    moveCar(200,200);
    customDelay(500);
    moveCar(0, 0);
    customDelay(100);
    targetAngle = 0;
    moveDistance(1500, true);

    moveCar(200, 200);
    customDelay(200);
    moveCar(0,0);
     // Rotate
    for (int i = angle; i < -90; i -= 5) {
      rotateToAngle(i, 2);
      customDelay(10);
    }
    customDelay(1000);
    rotateToAngle(-90, 10);
    customDelay(100);
    
    customDelay(2000); // Wait to pick wheels
    state = PICK_CABIN;
    // state = STOP;
    break;
  case PICK_CABIN:
    // Reverse
    moveCar(-200, -200);
    customDelay(4000);
    moveCar(0, 0);
    customDelay(100);

    // rotate to face cabin line
    for (int i = angle; i < 0; i += 5) {
      rotateToAngle(i, 2);
      customDelay(10);
    }
    customDelay(100);
    moveCar(200, 200);
    customDelay(2000);
    moveCar(0, 0);
    customDelay(100);
    targetAngle = 0;

    // detect line
    moveDistance(1000, true);    // minSpeed = 150;
    moveCar(150, 150);

    // rotate to face line
    moveCar(150, 150);
    customDelay(100);
    moveCar(0, 0);
    customDelay(100);

    customDelay(100);
    for (int i = angle; i > -80; i -= 5) {
      rotateToAngle(i, 2);
      customDelay(10);
    }
    customDelay(100);
    followLine(50);

    customDelay(2000);// wait to pick cabin
    
    state = BACK_TO_CHASIS;
    // state = STOP;
    break;
  case BACK_TO_CHASIS:
    moveCar(-100, -100);
    customDelay(6000);
    moveCar(0, 0);

    for (int i = angle; i < 140; i += 5) {
      rotateToAngle(i, 2);
      customDelay(10);
    }
    customDelay(100);
    
    moveCar(100, 100);
    customDelay(1000);
    moveCar(0, 0);
    targetAngle = 140;
    moveDistance(1000, true); // first line

    moveCar(100, 100);
    customDelay(500);
    moveCar(0, 0);


    moveDistance(1000, true); // detect second line

    moveCar(100, 100);
    customDelay(100);
    moveCar(0, 0);

    for (int i = angle; i > 100; i -= 5) {
      rotateToAngle(i, 2);
      customDelay(10);
    }
    customDelay(100);
    followLine(50);
    
    customDelay(2000); // wait to place
    state = BACK_TO_START;
  
  case BACK_TO_START:
    // Go back to starting position
    targetAngle = 170;
    for (int i = angle; i < 170; i += 5) {
      rotateToAngle(i, 2);
      customDelay(10);
    }

    moveCar(150, 150);
    customDelay(100);
    moveCar(0, 0);

    moveDistance(1000, true);

    moveCar(100, 100);
    customDelay(100);
    moveCar(0, 0);

    for (int i = angle; i > 90; i -= 5) {
      rotateToAngle(i, 2);
      customDelay(10);
    }
    customDelay(100);
    followLine(80);

    for (int i = angle; i < 150; i += 5) {
      rotateToAngle(i, 2);
      customDelay(10);
    }

    moveCar(200, 200);
    customDelay(1000);
    moveCar(0, 0);

    state = STOP;
    break;
  case STOP:
    customDelay(5000);
  }
}

void customDelay(unsigned long milliseconds)
{
  PREVIOUS_TIME = millis();
  while (abs(millis() - PREVIOUS_TIME) <= milliseconds)
    setGyroReadings();
}

void moveCar(int speedLeft, int speedRight)
{
  if (speedLeft >= 0)
  {
    digitalWrite(LEFT_MOTOR_PIN1, HIGH);
    digitalWrite(LEFT_MOTOR_PIN2, LOW);
  }
  else
  {
    digitalWrite(LEFT_MOTOR_PIN1, LOW);
    digitalWrite(LEFT_MOTOR_PIN2, HIGH);
    speedLeft = -speedLeft; // Ensure the speed is positive
  }

  if (speedRight >= 0)
  {
    digitalWrite(RIGHT_MOTOR_PIN1, HIGH);
    digitalWrite(RIGHT_MOTOR_PIN2, LOW);
  }
  else
  {
    digitalWrite(RIGHT_MOTOR_PIN1, LOW);
    digitalWrite(RIGHT_MOTOR_PIN2, HIGH);
    speedRight = -speedRight; // Ensure the speed is positive
  }

  analogWrite(ENABLE_LEFT_MOTOR, speedLeft);   // control speed of left motor
  analogWrite(ENABLE_RIGHT_MOTOR, speedRight); // control speed of right motor
}

void moveDistance(double distance_cm, bool detectLine = false)
{
  // Reset PID values
  integral = 0;
  previous_error = 0;

  setGyroReadings();
  // Reset the encoder readings

  // Initialize speeds
  int currentSpeedRight = minSpeed; // Start at minSpeed for right wheel
  int currentSpeedLeft = minSpeed;  // Start at minSpeed for left wheel

  // Start the motors
  moveCar(currentSpeedLeft, currentSpeedRight);
  int count = 0;

  sonarBack.ping_timer(echoCheckBack);
  customDelay(100);

  while (true)
  {
    // Update the current distances
    // Serial.print("Distance Left: ");
    // Serial.print(distanceLeft);
    // Serial.print("\t||\tDistance Right: ");
    // Serial.println(distanceRight);
    if (abs(angle - targetAngle) > 2 && count > 5)
    {
      count++;
      rotateToAngle(targetAngle, 10);
      integral = 0;
      previous_error = 0;
    }

    if (count >= 5)
    {
      count = 0;
      rotateToAngle(targetAngle, 10);
      integral = 0;
      previous_error = 0;
    }

    // Get the PID controller output
    double pidOutput = PIDController(targetAngle);

    // Adjust the speeds
    currentSpeedRight = constrain(currentSpeedRight + pidOutput, minSpeed, maxSpeed);
    currentSpeedLeft = constrain(currentSpeedLeft - pidOutput, minSpeed, maxSpeed);

    // Apply the new speeds
    moveCar(currentSpeedLeft, currentSpeedRight);

    // If both wheels have traveled the desired distance, stop
    // if (distanceLeft >= distance_mm && distanceRight >= distance_mm)
    // {
    //   break;
    // }

    
    // Check obstacle distance
    if (millis() >= pingTimer)
    {
      pingTimer += pingSpeed;      // Set the next ping time.
      sonarBack.ping_timer(echoCheckBack); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
    }

    if (distance_cm <= MAX_DISTANCE && distance >= distance_cm) {
      break;
    }

    // Reads black line and stop.
    // For white line, invert the values.
    // eg. if (detectLine && (analogRead(LEFT_SENSOR) < 150 || analogRead(RIGHT_SENSOR) < 150))
    if (detectLine && (analogRead(LEFT_SENSOR) > 650 || analogRead(RIGHT_SENSOR) > 650))
    {
      break;
    }
  }
  // moveCar(-250, -250);
  // customDelay(100);
  digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);

  analogWrite(ENABLE_LEFT_MOTOR, 0);  // control speed of left motor
  analogWrite(ENABLE_RIGHT_MOTOR, 0); // control speed of right motor
}

double PIDController(double target_heading)
{
  setGyroReadings();

  // Calculate the error
  double error = target_heading - angle;
  // double distance_error = distanceLeft - distanceRight;
  // error = error * 0.7 + distance_error * 0.3;

  // Proportional term
  double P = Kp * error;

  // Integral term
  integral += error;
  double I = Ki * integral;

  // Derivative term
  double derivative = error - previous_error;
  double D = Kd * derivative;

  // Remember the error for the next time
  previous_error = error;

  // The PID controller output
  return P + I + D;
}

double PIDControllerLine()
{
  setGyroReadings();

  // Read the sensor values
  int sensorValues[] = {
      analogRead(LEFT_SENSOR),
      analogRead(MIDDLE_SENSOR),
      analogRead(RIGHT_SENSOR)};

  // Calculate the error
  double error = (sensorValues[0]) - ((sensorValues[2]));

  if (abs(error) < 20)
    return 0.0;
  // Proportional term
  double P = Kp_line * error;

  // Integral term
  integral_line += error;
  double I = Ki_line * integral_line;

  // Derivative term
  double D = Kd_line * (error - previous_error_line);

  // Remember the error for the next time
  previous_error_line = error;

  // The PID controller output
  return P + I + D;
}

void setGyroReadings()
{
  // === Read accelerometer (on the MPU6050) data === //
  readAcceleration();
  // Calculating Roll and Pitch from the accelerometer data
  accAngleX = (atan(AccY / sqrt(pow(AccX, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorX; // AccErrorX is calculated in the calculateError() function
  accAngleY = (atan(-1 * AccX / sqrt(pow(AccY, 2) + pow(AccZ, 2))) * 180 / PI) - AccErrorY;

  // === Read gyroscope (on the MPU6050) data === //
  previousTime = currentTime;
  currentTime = micros();
  elapsedTime = (currentTime - previousTime) / 1000000; // Divide by 1000 to get seconds
  readGyro();
  // Correct the outputs with the calculated error values
  GyroX -= GyroErrorX; // GyroErrorX is calculated in the calculateError() function
  GyroY -= GyroErrorY;
  GyroZ -= GyroErrorZ;
  // Currently the raw values are in degrees per seconds, deg/s, so we need to multiply by sendonds (s) to get the angle in degrees
  gyroAngleX += GyroX * elapsedTime; // deg/s * s = deg
  gyroAngleY += GyroY * elapsedTime;
  yaw += GyroZ * elapsedTime;
  // combine accelerometer- and gyro-estimated angle values. 0.96 and 0.04 values are determined through trial and error by other people
  roll = 0.96 * gyroAngleX + 0.04 * accAngleX;
  pitch = 0.96 * gyroAngleY + 0.04 * accAngleY;
  angle = yaw;
  // if (angle < -180) angle = 0;
  // if (angle > 180) angle = 0;
  while (angle > 180)
    angle -= 360;
  while (angle < -180)
    angle += 360;
}

void calculateError()
{
  // When this function is called, ensure the car is stationary.

  // Read accelerometer values 200 times
  c = 0;
  while (c < 200)
  {
    readAcceleration();
    // Sum all readings
    AccErrorX += (atan((AccY) / sqrt(pow((AccX), 2) + pow((AccZ), 2))) * 180 / PI);
    AccErrorY += (atan(-1 * (AccX) / sqrt(pow((AccY), 2) + pow((AccZ), 2))) * 180 / PI);
    c++;
  }
  // Divide the sum by 200 to get the error value, since expected value of reading is zero
  AccErrorX = AccErrorX / 200;
  AccErrorY = AccErrorY / 200;
  c = 0;

  // Read gyro values 200 times
  while (c < 200)
  {
    readGyro();
    // Sum all readings
    GyroErrorX += GyroX;
    GyroErrorY += GyroY;
    GyroErrorZ += GyroZ;
    c++;
  }
  // Divide the sum by 200 to get the error value
  GyroErrorX = GyroErrorX / 200;
  GyroErrorY = GyroErrorY / 200;
  GyroErrorZ = GyroErrorZ / 200;
  Serial.println("The the gryoscope setting in MPU6050 has been calibrated");
}

void readAcceleration()
{
  Wire.beginTransmission(MPU);
  Wire.write(0x3B); // Start with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  // For a range of +-2g, we need to divide the raw values by 16384, according to the MPU6050 datasheet
  AccX = (Wire.read() << 8 | Wire.read()) / 16384.0; // X-axis value
  AccY = (Wire.read() << 8 | Wire.read()) / 16384.0; // Y-axis value
  AccZ = (Wire.read() << 8 | Wire.read()) / 16384.0; // Z-axis value
}

void readGyro()
{
  Wire.beginTransmission(MPU);
  Wire.write(0x43);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU, 6, true);
  GyroX = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroY = (Wire.read() << 8 | Wire.read()) / 131.0;
  GyroZ = (Wire.read() << 8 | Wire.read()) / 131.0;
}

void rotateToAngle(double target_angle)
{
  // Reset the PID variables
  integral = 0;
  previous_error = 0;

  // Initialize speeds
  int currentSpeedRight = minSpeed; // Start at minSpeed for right wheel
  int currentSpeedLeft = minSpeed;  // Start at minSpeed for left wheel // 220 instead of minspeed

  // Start the motors
  moveCar(currentSpeedLeft, currentSpeedRight);

  while (true)
  {
    // Get the PID controller output
    double pidOutput = PIDController(target_angle);

    // Adjust the speeds
    currentSpeedRight = constrain(currentSpeedRight + pidOutput, minSpeed, maxSpeed); // 220 instead of
    currentSpeedLeft = constrain(currentSpeedLeft - pidOutput, minSpeed, maxSpeed);   // minspeed

    // Apply the new speeds
    moveCar(currentSpeedLeft, currentSpeedRight);

    // If the error is below a small threshold, stop
    if (abs(target_angle - angle) <= 1.0)
    { // Adjust the threshold as needed
      break;
    }
  }
  digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);

  analogWrite(ENABLE_LEFT_MOTOR, 0);  // control speed of left motor
  analogWrite(ENABLE_RIGHT_MOTOR, 0); // control speed of right motor
}

void rotateToAngle(float target_angle, float tolerance, bool fast = false)
{ // tolerance = 2.0
  // float current_angle = 0.0; // initialize current angle
  float current_angle = angle;
  float error = target_angle - current_angle; // initialize error

  // float min_motor_speed = 220.0; // minimum speed required to move the car

  do
  {
    // calculate the current orientation of the car
    setGyroReadings();
    current_angle = angle;

    // calculate the error and PID output
    error = target_angle - current_angle;
    float pid_output = PIDController(target_angle);
    float motor_speed;
    if (!fast) {
      motor_speed = mapToMotorSpeed(pid_output);
    }
    else {
      motor_speed = mapToMotorSpeed2(pid_output);
    }

    // // apply the PID output to the motors (turn in place)
    // // check if pid_output is below minimum motor speed, and if so, set it to minimum motor speed
    // if (pid_output < min_motor_speed) {
    //   pid_output = min_motor_speed;
    // }

    // // note: we might need to adjust the sign of pid_output depending on setup
    // moveCar(pid_output, -pid_output);
    // if (pid_output > 0) { // If PID output is negative, rotate in the opposite direction
    //   moveCar(-motor_speed, motor_speed);
    // } else {
    //   moveCar(motor_speed, -motor_speed);
    // }
    // if (target_angle > angle)
    if (target_angle > angle)
    { // If PID output is negative, rotate in the opposite direction
      moveCar(-motor_speed, motor_speed);
    }
    else
    {
      moveCar(motor_speed, -motor_speed);
    }

  } while (abs(error) > tolerance); // continue rotating until we are close to the target angle

  // once we are close enough to the target angle, stop the car
  moveCar(0, 0);
}

void echoCheck()
{ // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonar.check_timer())
  { // This is how you check to see if the ping was received.
    // // Here's where you can add code.
    Serial.print("Ping: ");
    Serial.print(sonar.ping_result / US_ROUNDTRIP_CM); // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
    Serial.println("cm");
    distance = sonar.ping_result / US_ROUNDTRIP_CM;
  }
  // Don't do anything here!
}

void echoCheckBack()
{ // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  if (sonarBack.check_timer())
  { // This is how you check to see if the ping was received.
    // // Here's where you can add code.
    // Serial.print("Ping: ");
    // Serial.print(sonarBack.ping_result / US_ROUNDTRIP_CM); // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
    // Serial.println("cm");
    distance = sonarBack.ping_result / US_ROUNDTRIP_CM;
  }
  // Don't do anything here!
}

float mapToMotorSpeed(float pid_output)
{
  // Ensure pid_output is between 0 and 1
  pid_output = constrain(pid_output, 0.0, 1.0);
  // Map from PID output to motor speed
  return pid_output * (100 - 80) + 80;
}

float mapToMotorSpeed2(float pid_output)
{
  // Ensure pid_output is between 0 and 1
  pid_output = constrain(pid_output, 0.0, 1.0);
  // Map from PID output to motor speed
  return pid_output * (180 - 150) + 150;
}

void followLine(int stopDistance, bool fastFollow)
{
  sonar.ping_timer(echoCheck);
  customDelay(100);
  // Initialize speeds
  int currentSpeedRight = minSpeed; // Start at minSpeed for right wheel
  int currentSpeedLeft = minSpeed; // Start at minSpeed for left wheel

  previous_time_distance = millis();
  distance = 500;

  while (true)
  {
    // Check obstacle distance
    if (millis() >= pingTimer)
    {
      pingTimer += pingSpeed;      // Set the next ping time.
      sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
    }

    // Keep updating angles: improves accuracy
    setGyroReadings();

    // Follow Line
    // Get the PID controller output
    double pidOutput = PIDControllerLine();

    // Adjust the speeds
    currentSpeedRight = constrain(minSpeed + pidOutput, 0 , 254);
    currentSpeedLeft = constrain(minSpeed - pidOutput, 0, 254);

    // if (currentSpeedLeft <= 0)
    //   currentSpeedLeft = constrain(currentSpeedLeft, -180, -150);
    // if (currentSpeedLeft > 0)
    //   currentSpeedLeft = constrain(currentSpeedLeft, 150, 180);

    // if (currentSpeedRight <= 0)
    //   currentSpeedRight = constrain(currentSpeedRight, -180, -150);
    // if (currentSpeedRight > 0)
    //   currentSpeedRight = constrain(currentSpeedRight, 150, 180);

    // // Apply the new speeds
    moveCar(currentSpeedLeft, currentSpeedRight);
    // Serial.println(currentSpeedLeft);
    // Serial.println(currentSpeedRight);
    // if (fastFollow)
    // {
    //   platform.fastRotateMotor(MOTOR_SPEED + pidOutput, MOTOR_SPEED - pidOutput);
    // }
    // else
    // {
    //   platform.rotateMotor(MOTOR_SPEED + pidOutput, MOTOR_SPEED - pidOutput);
    // }

    if (distance <= stopDistance && ((millis() - PREVIOUS_TIME) > 2000))
    {
      break;
    }
    // Serial.println(distance);
  }

  // Brake
  // moveCar(-220, -220);
  customDelay(100);
  // Stop the car
  digitalWrite(LEFT_MOTOR_PIN1, LOW);
  digitalWrite(LEFT_MOTOR_PIN2, LOW);
  digitalWrite(RIGHT_MOTOR_PIN1, LOW);
  digitalWrite(RIGHT_MOTOR_PIN2, LOW);

  analogWrite(ENABLE_LEFT_MOTOR, 0);  // control speed of left motor
  analogWrite(ENABLE_RIGHT_MOTOR, 0); // control speed of right motor
}

void fastRotate(int target_angle)
{
  int left_speed;
  int right_speed;

  if (target_angle >= 0)
  {
    left_speed = -250;
    right_speed = 250;
  }
  else
  {
    left_speed = 250;
    right_speed = -250;
  }

  // while (abs(target_angle - angle) > 10) {
  // moveCar(left_speed, right_speed);
  platform.rotateMotor(right_speed, left_speed);
  // }
}