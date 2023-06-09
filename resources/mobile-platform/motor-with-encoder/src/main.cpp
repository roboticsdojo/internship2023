#include <Arduino.h>
#include <Encoder.h>

// Encoder Pins
#define RIGHT_ENCODER_A_PIN 18
#define RIGHT_ENCODER_B_PIN 19
#define LEFT_ENCODER_A_PIN 2
#define LEFT_ENCODER_B_PIN 3

// Motor Parameters
#define PPR 360.0              // Pulses per revolution for the encoder
#define wheelDiameter 0.1      // Diameter of the wheel in meters (adjust this to match your setup)
#define wheelCircumference wheelDiameter * PI

// Encoder
Encoder rightEnc(RIGHT_ENCODER_A_PIN, RIGHT_ENCODER_B_PIN);
Encoder leftEnc(LEFT_ENCODER_A_PIN, LEFT_ENCODER_B_PIN);
long oldPositionRight  = -999;
long oldPositionLeft  = -999;


// Function Prototypes
int getDistanceRight();
int getDistanceLeft();

void setup() {
  Serial.begin(9600);
  Serial.println("Motor Encoder Test:");
}

void loop() {
  getDistanceRight();
  getDistanceLeft();
}

int getDistanceRight() {
  long newPosition = rightEnc.read();
  
  if (newPosition != oldPositionRight) {
    oldPositionRight = newPosition;
    
    double wheelRotations = newPosition / PPR;
    double distanceTravelled = wheelRotations * wheelCircumference;
    
    Serial.print("Distance Travelled Right (m): ");
    Serial.println(distanceTravelled, 3);
  }

  return (newPosition);
}

int getDistanceLeft() {
  long newPosition = leftEnc.read();
  
  if (newPosition != oldPositionLeft) {
    oldPositionLeft = newPosition;
    
    double wheelRotations = newPosition / PPR;
    double distanceTravelled = wheelRotations * wheelCircumference;
    
    Serial.print("Distance Travelled Left (m): ");
    Serial.println(distanceTravelled, 3);
  }

  return (newPosition);
}