#include <Arduino.h>


// Distance-related variables
#define PULSES_PER_REV 150     // this was measured manually before-hand
#define RPM_TO_RADS 0.1047198; // 1 rpm  = 0.1047198 rad/s
#define WHEEL_RADIUS 0.035     // radius of the wheel attached to the motor 0.035 m

volatile long pulse_count = 0;
bool direction = false; // true if motor is moving forward, false if motor is moving reverse

/* Sampling interval 1 second */
int sampling_interval = 1000;

/*velocity variables
  we need angular velocity in radians, in degrees and the linear velocity*/
float angular_velocity_in_rads = 0.0;
float angular_velocity_in_degrees = 0.0;
float linear_velocity = 0.0;
float distance_cm = 0.0;


// Read encoder pulses from a motor
void readEncoderRaw(int encoderA, int encoderB)
{
    int a = digitalRead(encoderA);
    int b = digitalRead(encoderB);

    Serial.print(a * 5); // scale the values for easier visualization
    Serial.print(" ");
    Serial.print(b * 5);
}


// Used to drive a motor
void setMotor(int dir, int pwmPin, int pwmVal, int in1, int in2)
{
  analogWrite(pwmPin, pwmVal);

  if (dir == -1)
  {
    // Serial.println("Forward");
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  }
  else if (dir == 1)
  {
    // Serial.println("Backward");
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
  else
  {
    // Serial.println("Stop");
    digitalWrite(in1, LOW);
    digitalWrite(in2, LOW);
  }
}


// Get distance travelled by the motor.
int getDistance(int encoderPosition, long current_millis, long previous_millis)
{

  // get the elapsed time
  current_millis = millis();

  // Take a sample
  if ((current_millis - previous_millis) > sampling_interval)
  {
    /* code */

    /* update the current time for the next iteration */
    previous_millis = current_millis;

    /* calculate Revs per minute */
    float revs_per_minute = (float)((encoderPosition * 60) / PULSES_PER_REV);

    /* calculate angular velocity in rad/s of the wheel in radians */
    angular_velocity_in_rads = revs_per_minute * RPM_TO_RADS;

    /* calculate angular velocity in degrees */
    angular_velocity_in_degrees = angular_velocity_in_rads * RAD_TO_DEG; // 1 rad = 57.2958 deg, inbuilt

    /* Calculate the linear velocity (m/s): linear velocity = radius x angular velocity  */
    linear_velocity = angular_velocity_in_rads * WHEEL_RADIUS;

    /* calculate the distance travelled (centimeters): distance = speed * time */
    if (direction == false)
    {
      /* motor moving reverse - decrement distance - linear velocity is negative here */
      distance_cm = distance_cm + (linear_velocity * (sampling_interval / 1000)) * 100;
    }
    else if (direction == true)
    {
      /* motor moving forward -> increment distance */
      distance_cm = distance_cm + (linear_velocity * (sampling_interval / 1000)) * 100;
    }

    
    return distance_cm;

    /* reset the pulse count */
    encoderPosition = 0;
  }
}


void PIDLoop(int leftEncoderPosition, int rightEncoderPosition, long current_millis, long previous_millis, long prevT, int eprev, float eintegral, int enA, int enB, int in1, int in2, int in3, int in4){
    
  int leftDistance = getDistance(leftEncoderPosition, current_millis, previous_millis);
  int rightDistance = getDistance(rightEncoderPosition, current_millis, previous_millis);

  // PID Constants
  float kp = 1;
  float ki = 0;
  float kd = 0.025;

  // compute time difference
  long currT = micros();

  float deltaT = ((float)(currT - prevT)) / 1e6; // deltaT in seconds
  prevT = currT;

  // calculate error
  // int e = target - encoderPosition;
  int e = leftDistance - rightDistance;

  // derivative
  float dedt = (e - eprev) / (deltaT);

  // integral
  eintegral = eintegral + e * deltaT;

  // calculate control signal
  float u = kp * e + ki * eintegral + kd * dedt;

  // set motor power
  float pwr = fabs(u);
  if (pwr > 255)
  {
    pwr = 255;
  }

  // Determine motor
  if (u < 0)
  {
    setMotor(1, enA, pwr, in1, in2);
  }
  else
  {
    setMotor(1, enB, pwr, in3, in4);
  }

  // store previous error
  eprev = e;


  Serial.print("Left Distance: ");
  Serial.print(leftDistance);
  Serial.print(" Right Distance: ");
  Serial.print(rightDistance);
  Serial.print(" ");
  Serial.print("Error: ");
  Serial.print(e);
  Serial.print(" ");
  Serial.print("Control Signal: ");
  Serial.print(u);
  Serial.print(" ");
  Serial.println();

}


// RPM calculations were unreliable thus redacted for now.
/*
int printRPM()
{

  // Update RPM value on every second
  currentMillis = millis();
  if (currentMillis - previousMillis > interval)
  {
    previousMillis = currentMillis;
    // Revolutions per minute (RPM) =
    // (total encoder pulse in 1s / motor encoder output) x 60s
    rpm = (float)(abs(leftEncoderPosition) * 60 / ENCODERPPR);
    rpm = fabs(rpm);

    // Only update display when there have readings
    // if (rpm > 0)
    // {
    // }
    Serial.print(leftEncoderPosition);
    Serial.print(" pulse / ");
    Serial.print(ENCODERPPR);
    Serial.print(" pulse per rotation x 60 seconds = ");
    Serial.print(rpm);
    Serial.println(" RPM");
    leftEncoderPosition = 0;
    return rpm;
  }
}

*/