#include <Arduino.h>
#include <Servo.h>
#include <Ramp.h>


// Define the servos for the robotic arm
Servo j1Servo;
Servo j2Servo;
Servo j3Servo;
Servo j4Servo;
Servo gServo;

#define JOINT_1 2
#define JOINT_2 3
#define JOINT_3 4
#define JOINT_4 5
#define GRIPPER 6

// Initial angles
float initialTheta1 = 90.0;
float initialTheta2 = 90.0;
float initialTheta3 = 90.0;
float initialTheta4 = 0.0;

void moveServo(int angle, Servo servo);
void moveToPos(double x, double y, double z);

rampInt servo1Ramp;  
rampInt servo2Ramp;  
rampInt servo3Ramp;  
rampInt servo4Ramp;

int prevAngle1 = 0;
int prevAngle2 = 0;
int prevAngle3 = 0;
int prevAngle4 = 0;

void moveServos(int angle1, int angle2, int angle3, int angle4, uint32_t time);
void updateServo();
void rampToAngle(double s1, double s2, double s3, double s4, unsigned long t, bool init=false);
void moveToAngle(double s1, double s2, double s3, double s4);
void calculateInverseKinematics(int x_e, int y_e, int z_e, int phi_e);

void setup()
{
  Serial.begin(9600);
  j1Servo.attach(JOINT_1, 600, 2600);
  j2Servo.attach(JOINT_2, 400, 2700);
  j3Servo.attach(JOINT_3, 600, 2600);
  j4Servo.attach(JOINT_4, 600, 2600);
  gServo.attach(GRIPPER);

  j1Servo.write(90);
  j2Servo.write(90);
  j3Servo.write(90);
  j4Servo.write(90);
  rampToAngle(90, 90, 90, 90, 1000, true);


  // moveServos(90, 90, 90, 90, 50000);  // Move servos to initial position
  // rampToAngle(90, 90, 90, 90, 3000);
  delay(2000);  // Wait for movement to complete
  // moveServo(-90, j1Servo);

  // delay(3000);
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
  // gServo.write(90);
  // rampToAngle(90, 60, 180, 90, 1500);
  gServo.write(0);
  // delay(1000);
  // rampToAngle(90, 90, 0, 180, 2000);
  // gServo.write(180);
  // delay(1000);
  // moveToPos(0, 100, 0);
  // delay(1000);
  // moveToPos(0, 200, 200);
  // delay(1000);


  // int z = 0;
  // for (z = 0; z <= 200; z+=50)
  // {
    // moveToPos(0, 100, 0);
    // delay(100);
  // }
  // moveServo(-90, j3Servo);
  // moveToPos(0, 200, 0);
  // delay(3000);
  // moveToPos(0,200,(200 / 2)); // devide by 2 to correct error
  // delay(6000);
  // delay(100);
  // moveServos(90, 90, 120, 0, 60000);  // Move servos to these angles in 2000 milliseconds
  // // delay(2000);  // Wait for movement to complete
  // // moveServos(0, 0, 0, 0, 2000);  // Move servos back to initial position in 2000 milliseconds
  // // delay(2000);  // Wait for movement to complete
  // updateServo();  
  // delay(10); 
  // 
  // calculateInverseKinematics(300, 200, 0, 0);
  // calculateInverseKinematics(300, 0, 0, 0);

}

void moveToPos(double x, double y, double z)
{
  // double b = atan2(x, y) * (180 / 3.1415); // base angle

  // double l = sqrt(x * x + y * y); // x and y extension

  // double h = sqrt(l * l + z * z);

  // double phi = atan(z / l) * (180 / 3.1415);

  // double theta = acos((h / 2) / 150) * (180 / 3.1415); // perfect

  // double a1 = phi + theta; // angle for first part of the arm
  // double a2 = phi - theta; // angle for second part of the arm

  // // moveToAngle(b,a1,a2,g);
  // Serial.print("b: ");
  // Serial.println(b);
  // Serial.print("a1: ");
  // Serial.println(-(a1));
  // Serial.print("a2: ");
  // Serial.println(a2);
  // Serial.println();
  // // delay(3000);
  // moveServo(b, j1Servo);
  // // if (z == 0)
  // moveServo(-(90 - a1), j2Servo);
  // // else
  //   // moveServo((-(90 - a1)), j2Servo);
  // if (z == 0)
  //   moveServo(90 - (180 - (a1 * 2)), j3Servo);
  // else
  //   moveServo(-90 + (180 - (a1 * 2)), j3Servo);
  Serial.println("Values of x , y , z :");
   
 
   Serial.print(x);
   Serial.print(" ");
   Serial.print(y);
    Serial.print(" ");
   Serial.println(z);
    Serial.print(" ");

   
    double pi = 3.141592653589793238462643383279502884197;

     // z= z  50; 
    // y = y - 80;
  double l1 = 180.00; 
  double l2 = 120.00;
  double Ls = 350;
  //cosine rule 
  double q2 = PI -  acos ( ( (l1 * l1) + (l2 * l2 ) - (x * x ) - (y * y) ) / (2 * l1 * l2) ); //* 180 / 3.142;
 double q21 = round(acos ( ((x *x ) + (y * y ) - (l1 * l1) - (l2 * l2)) / (2 * l1 * l2 ))); 

  double l = round(sqrt(x * x + y * y)); // x and y extension

  //total length of the link 
  //l = 350; 


  // angle q1 
  double q1 =  degrees(atan2(z , l )) ;//- degrees(atan( (l2 * degrees(sin (q21)) / (l1 + l2 * degrees(cos (q21))))));
  Serial.print("Value of q1 is :");
  Serial.print(q1);
  Serial.print("\n\n");
  q1 = round(q1);


  double b = atan2(x, y) * (180 / pi); // base angle



  Serial.print(" BOTH VALUES OF Q2 ARE "); 
  Serial.print(q2);
  Serial.print("  ");
  Serial.println(q21);

  double h = round(sqrt(l * l + z * z));
  /*
  Since our link lengths are in the ratio of 18 cm by 12 cm 
  we divide h by 2.5 to 
  */
  //double theta = acos((h / 2.5) / 150) * (180 / 3.1415); // perfect
  double theta = round(atan2( y, x) * 180/ pi);  // atan2 to identify coordinate frame
  theta = 180 - theta -8; 
  Serial.print("value of theta is :");
  Serial.print("value of q2 is :");
  Serial.println(q2);
  // snail(q2, j3Servo);    //////
  //moveServo(q2, j3Servo);
  Serial.println(theta);
  // snail(theta,j1Servo); ///////
 // moveServo(theta, j1Servo);
  delay(200);
  // if (z == 0)
  //moveServo(-(90 - a1), j2Servo);

  Serial.println("value of q1 is :");
  Serial.print(q1);
  Serial.println();
  q1 = q1 + 13;
  // snail(q1, j2Servo); //////
 // moveServo(q1, j2Servo);
  delay(200);
  rampToAngle(theta, q1, degrees(q2), 180, 800);

  


}

void moveServo(int angle, Servo servo)
{

  // int servoAngle = 90 - angle;
  int servoAngle = map(angle, 90, -90, 700, 2500);
  // Serial.println(servoAngle);

  // if (servoAngle > 180 || servoAngle < 0)
    // Serial.println("Angle out of range");

  // constrain the angle to the range 0-180
  // servoAngle = constrain(servoAngle, 0, 180);

  // move the servo to the specified angle
  // servo.write(servoAngle);
  servo.writeMicroseconds(servoAngle);
}

void updateServo() {
  j1Servo.write(servo1Ramp.update());
  j2Servo.write(servo2Ramp.update());
  j3Servo.write(servo3Ramp.update());
  j4Servo.write(servo4Ramp.update());
}

void moveServos(int angle1, int angle2, int angle3, int angle4, uint32_t time) {
  uint32_t duration1 = abs(angle1 - prevAngle1);
  uint32_t duration2 = abs(angle2 - prevAngle2);
  uint32_t duration3 = abs(angle3 - prevAngle3);
  uint32_t duration4 = abs(angle4 - prevAngle4);

  uint32_t maxDuration = max(max(duration1, duration2), max(duration3, duration4));

  // Calculate step for each servo
  float step1 = (maxDuration == 0) ? 0 : (float)duration1 / (time / 10);
  float step2 = (maxDuration == 0) ? 0 : (float)duration2 / (time / 10);
  float step3 = (maxDuration == 0) ? 0 : (float)duration3 / (time / 10);
  float step4 = (maxDuration == 0) ? 0 : (float)duration4 / (time / 10);

  servo1Ramp.go(angle1, step1);
  servo2Ramp.go(angle2, step2);
  servo3Ramp.go(angle3, step3);
  servo4Ramp.go(angle4, step4);

  prevAngle1 = angle1;
  prevAngle2 = angle2;
  prevAngle3 = angle3;
  prevAngle4 = angle4;
}

void moveToAngle(double s1, double s2, double s3, double s4) {
  j1Servo.write(s1);
  j2Servo.write(s2);
  j3Servo.write(s3);
  j4Servo.write(s4);
}

void rampToAngle(double s1, double s2, double s3, double s4, unsigned long t, bool init) {
  servo1Ramp.go(s1,t, LINEAR, ONCEFORWARD);
  servo2Ramp.go(s2,t, LINEAR, ONCEFORWARD);
  servo3Ramp.go(s3,t, LINEAR, ONCEFORWARD);
  servo4Ramp.go(s4,t, LINEAR, ONCEFORWARD);
  
  while (servo1Ramp.isRunning() || servo2Ramp.isRunning() || servo3Ramp.isRunning() || servo4Ramp.isRunning()) {
    if (init) {
      servo1Ramp.update();
      servo2Ramp.update();
      servo3Ramp.update();
      servo4Ramp.update();
    }
    else
      moveToAngle(servo1Ramp.update(),servo2Ramp.update(),servo3Ramp.update(),servo4Ramp.update());
  }
}

// void calculateInverseKinematics(int x_e, int y_e, int z_e, int phi_e) {
//   double l1 = 180;
//   double l2 = 115;
//   double l3 = 170;

//   double x_w = x_e - (l3 * cos(phi_e));
//   double y_w = y_e - (l3 * sin(phi_e));


//   double theta2 = acos((sq(l1) + sq(l2) - sq(x_w) - sq(y_w)) / (2 * l1 * l2));
//   double theta1 = (atan2(y_w , x_w)) - acos((sq(x_w) + sq(y_w) + sq(l1) - sq(l2)) / (2 *l1 * sqrt(sq(x_w) + sq(y_w))));

//   // convert to degrees
//   if (y_e == 0)
//     theta1 = 90 + (theta1 * (180 / PI)); // remains
//   else
//     theta1 = 90 - (theta1 * (180 / PI)); // remains
//   theta2 = 180 - (theta2 * (180 / PI)) + 90;
  

//   double theta3 = phi_e - theta1 - theta2;

//   // theta2 = (180 - (theta2 > 90 ? theta2 - 90 : theta2 + 90));

//   // theta1 = map(theta1, -90, 90, 0, 180);

//   rampToAngle(90, abs(theta1) + 20, 180 - (abs(theta2) - 60) + 90, abs(theta3), 2000);
//   Serial.print("theta 1: "); Serial.print(theta1);
//   Serial.print("\ttheta 2: "); Serial.print(theta2);
//   Serial.print("\ttheta 3: "); Serial.print(theta3);
//   Serial.print("\tx_w: "); Serial.print(x_w);
//   Serial.print("\ty_w: "); Serial.println(y_w);

  
// }

void calculateInverseKinematics(int x_e, int y_e, int z_e, int phi_e) {
  double l1 = 180;
  double l2 = 115;
  double l3 = 170;

  double x_w = x_e - (l3 * cos(phi_e));
  double y_w = y_e - (l3 * sin(phi_e));

  double c2 = (sq(x_w) + sq(y_w) - sq(l1) - sq(l2)) / (2 * l1 * l2);
  double s2 = sqrt(1 - sq(c2)); // This will be positive for elbow up, negative for elbow down

  double theta2 = atan2(s2, c2); // Use atan2 to get the correct quadrant

  double k1 = l1 + l2 * c2;
  double k2 = l2 * s2;
  double theta1 = atan2(y_w, x_w) - atan2(k2, k1); // Use atan2 to get the correct quadrant

  // convert to degrees
  theta1 = (theta1 * (180 / PI)) + 90;
  theta2 = (theta2 * (180 / PI));
  theta1 = 180 - theta1;
  theta2 = (theta2);

  double theta3 = 180 + (phi_e - theta1 - theta2);

  rampToAngle(90, abs(theta1), theta2, abs(theta3), 2000);
  Serial.print("theta 1: "); Serial.print(theta1);
  Serial.print("\ttheta 2: "); Serial.print(theta2);
  Serial.print("\ttheta 3: "); Serial.print(theta3);
  Serial.print("\tx_w: "); Serial.print(x_w);
  Serial.print("\ty_w: "); Serial.println(y_w);
}


