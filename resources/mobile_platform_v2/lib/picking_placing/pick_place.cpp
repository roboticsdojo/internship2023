#include <Arduino.h>
#include "pickPlace.h"


// Mobile-Platform to Pi Communication Simulator
// 1. Mobile-platform stops at object
// 2. If picking, pull pin 8 high
// 3. If placing (at chassis location), pull pin 9 high
// 4. Pi executes relevant code
// 5. Pi pulls pin 11 high when done
// 6. Mobile-platform pulls pin 8 / 9 low
// 7. Mobile-platform continues on path

int pickPin = 8;
int placePin = 9;
int continuePin = 11;
int debugPin = 7;

int go;
int done = 0;


void stopAtObjectLocation();
//void getPinValue();

void setup() {
  Serial.begin(9600);
  pinMode(pickPin, OUTPUT);
  pinMode(placePin,  OUTPUT);
  pinMode(continuePin, INPUT);
  pinMode(debugPin, OUTPUT);

}

void loop() {

  if (done == 0){
    Serial.println("Cycle Starting");

  // 1. Mobile-platform stops at object
  stopAtObjectLocation();

  // 4. Pi executes relevant code

  // 5. Pi pulls pin 11 high when done
  go = digitalRead(continuePin);
  Serial.print("Waiting for go signal: ");
  Serial.println(go);

  // 7. Mobile-platform continues on path
  if (go == 1){
    // 6. Mobile-platform pulls pin 8 / 9 low
    digitalWrite(pickPin, LOW);
    digitalWrite(debugPin, LOW);
    Serial.println("Continuing on path");
    done = 1;
    delay(3000);
  }
  }
  else{
    Serial.println("Cycle Complete");
    // done = 0;
  }
}

void stopAtObjectLocation(){

  // Pick or Place

  // 2. If picking, pull pin 8 high
  digitalWrite(pickPin, HIGH);
  digitalWrite(debugPin, HIGH);
  Serial.println("Send command :> Picking up object");
}
void stopAndPlace(){
  digitalWrite(placePin, HIGH);
  digitalWrite(debugPin, HIGH);
  Serial.println("Placing object");
}