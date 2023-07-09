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
int moveToPos(double x, double y, double z);

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

// Communication stuff
String getValue(String data, char separator, int index);

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

  // delay (2000);

  // while (true) {
  //   moveToPos(0, 100, 0);
  //   gServo.write(0);
  //   delay(1000);
  //   gServo.write(90);
  //   delay(1000);
  //   // delay(1000);
  //   moveToPos(0, 200, 200);
  //   delay(1000);
  // }

  // delay(2000);  // Wait for movement to complete
}

void loop()
{
   // 1. Poll Serial for information
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    // String data = "0|10|20|30\n";
    // Serial.print("[Arduino Rx]> ");
    // Serial.print(data);
    // Serial.println();

    // 2. Determine whether to pick or place from message

    // Message Format
    // "action|x|y|z" -> "0|10|20|30"

    // 0 - pick , 1 - place
    String action = getValue(data,'|',0);
    // Get - Coordinates
    String x_coord = getValue(data,'|',1);
    String y_coord = getValue(data,'|',2);
    String z_coord = getValue(data,'|',3);

    // Serial.print("Parsed Message: ");
    // Serial.print(action);
    // Serial.print(" ");
    // Serial.print(x_coord);
    // Serial.print(" ");
    // Serial.print(y_coord);
    // Serial.print(" ");
    // Serial.print(z_coord);
    // Serial.println();

  

  // typecast action to int
  int a = action.toInt();

  // 3. If picking, get coordinate info from message
  if (a == 0){
    int x = x_coord.toInt();
    int y = y_coord.toInt();
    int z = z_coord.toInt();

    // 4. Send coordinates to IK function
    // pickStatus = pick(x, y, z);
    moveToPos(x, y, z);
    delay(1000);
    gServo.write(90);
    delay(1000);
    gServo.write(0);
    delay(1000);
    rampToAngle(90, 90, 0, 180, 1000);

    // pickStatus = 1;
    // 6. Send message to Pi
    // if (pickStatus == 1) {
    Serial.print(String(x));
    Serial.print(String(y));
    Serial.println(String(z));

    // } else {
      // Serial.println("FAILURE");
    // }
    // pickStatus = 0;
  } else{
      Serial.println("PLACING");
  }
  }

  // delay(3000);
  // Serial.println("Waiting for message...");
}

int moveToPos(double x, double y, double z)
{
  // Serial.println("Values of x , y , z :");
   
 
  //  Serial.print(x);
  //  Serial.print(" ");
  //  Serial.print(y);
  //   Serial.print(" ");
  //  Serial.println(z);
  //   Serial.print(" ");

   
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
  // Serial.print("Value of q1 is :");
  // Serial.print(q1);
  // Serial.print("\n\n");
  q1 = round(q1);


  double b = atan2(x, y) * (180 / pi); // base angle



  // Serial.print(" BOTH VALUES OF Q2 ARE "); 
  // Serial.print(q2);
  // Serial.print("  ");
  // Serial.println(q21);

  double h = round(sqrt(l * l + z * z));
  /*
  Since our link lengths are in the ratio of 18 cm by 12 cm 
  we divide h by 2.5 to 
  */
  //double theta = acos((h / 2.5) / 150) * (180 / 3.1415); // perfect
  double theta = round(atan2( y, x) * 180/ pi);  // atan2 to identify coordinate frame
  theta = 180 - theta -8; 
  // Serial.print("value of theta is :");
  // Serial.print("value of q2 is :");
  // Serial.println(q2);
  // snail(q2, j3Servo);    //////
  //moveServo(q2, j3Servo);
  // Serial.println(theta);
  // snail(theta,j1Servo); ///////
 // moveServo(theta, j1Servo);
  delay(200);
  // if (z == 0)
  //moveServo(-(90 - a1), j2Servo);

  // Serial.println("value of q1 is :");
  // Serial.print(q1);
  // Serial.println();
  q1 = q1 + 13;
  // snail(q1, j2Servo); //////
 // moveServo(q1, j2Servo);
  delay(200);
  rampToAngle(theta, q1, degrees(q2), 180, 800);

  return 1;
}

void moveServo(int angle, Servo servo)
{
  int servoAngle = map(angle, 90, -90, 700, 2500);
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

String getValue(String data, char separator, int index)
{
  int found = 0;
  int strIndex[] = {0, -1};
  int maxIndex = data.length()-1;

  for(int i=0; i<=maxIndex && found<=index; i++){
    if(data.charAt(i)==separator || i==maxIndex){
        found++;
        strIndex[0] = strIndex[1]+1;
        strIndex[1] = (i == maxIndex) ? i+1 : i;
    }
  }

  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}