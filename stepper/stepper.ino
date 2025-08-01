#include <Stepper.h>

const int stepsPerRevolution = 100;  // change this to fit the number of steps per revolution
// for your motor

// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, 8, 9, 10, 11);

int stepCount = 0;         // number of steps the motor has taken

void setup() {
  // initialize the serial port:
  Serial.begin(9600);
  myStepper.setSpeed(60);
}

void loop() {
  // step one step:
  myStepper.step(100);
  Serial.print("steps:");
  Serial.println(stepCount);
  stepCount++;
  //delay(500);
}
