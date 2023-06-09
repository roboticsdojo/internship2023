#ifndef MOTOR_ENCODER_H
#define MOTOR_ENCODER_H

void readEncoderRaw(int encoderA, int encoderB);
void setMotor(int dir, int pwmPin, int pwmVal, int in1, int in2);
int getDistance(int encoderPosition, long current_millis, long previous_millis);

void PIDLoop(int leftEncoderPosition, int rightEncoderPosition, long current_millis, long previous_millis, long prevT, int eprev, float eintegral, int enA, int enB, int in1, int in2, int in3, int in4);


#endif