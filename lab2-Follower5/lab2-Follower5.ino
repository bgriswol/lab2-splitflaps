#include <Wire.h>
#include <Stepper.h>

const byte CALIBRATE = 1;
const byte GO_TO_LETTER = 2;
const byte MOVE_STEPS = 3;
const byte SPIN = 4;
const byte TEST = 5;

const int FollowerAddress = 12;
const int hallPin = 2;
volatile bool startFound = false;
//String flap [] = {":)", ":(", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "!", "$", "%", "&", "*", "(", ")", "?", "+", "=", "/", "-"};

char flap[] = {'<', '.', '?', '!', '`', '-', '(', ')', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '^', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '>', '#', '@', '$', '%'};

// first blank is <, second is >, third is (, fourth is )
const int stepsPerIndex = 16; 
const int stepsPerRevolution = 400; // This is the steps per revolution
int currStep = 0; // tracks the current location of our stepper
int currFlap = 0; 
bool calibrated = false; 
bool completedCurrentState = false;

bool ledTest = LOW;
bool offset = false;

char target;

enum FollowerState {
  Waiting,
  Calibrating,
  OffsetAndFinishCalibration,
  SearchingForLetter,
  Spinning,
};
FollowerState state;

Stepper stepper(stepsPerRevolution, 4, 3);

void setup() {
    // put your setup code here, to run once:
    pinMode(hallPin,INPUT_PULLUP); // original code used INPUT as 2nd parameter
    stepper.setSpeed(40.0); // constant speed for stepper.
    Wire.begin(FollowerAddress);
    Wire.onRequest(requestEvent); 
    Wire.onReceive(receiveEvent); 

    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);
    Serial.println("follower");

    attachInterrupt(digitalPinToInterrupt(hallPin), hallSensorInterrupt, RISING);
}

void loop() {
  switch (state) {
    case (Calibrating):
      stepper.step(stepsPerIndex);
      break;
    case (OffsetAndFinishCalibration):
      stepper.step(stepsPerIndex*2);
      calibrated = true;
      completedCurrentState = true;
      state = Waiting;
      detachInterrupt(digitalPinToInterrupt(hallPin));
    case (SearchingForLetter):
      moveToTarget();
      completedCurrentState = true;
      state = Waiting;
    case (Spinning):
      stepper.step(stepsPerIndex);
  }
}

void requestEvent() {
    if(completedCurrentState){
        Wire.write(1);
    } else {
        Wire.write(0);
    }
}

void receiveEvent(int numBytes){
    Serial.println("received event");
    completedCurrentState = false;
    int command;
    char value;
    while (1 < Wire.available()){
      command = Wire.read();
    }
    value = Wire.read();
    Serial.println(value);
    switch(command) {
      case CALIBRATE:
          calibrate();
          break;
      case GO_TO_LETTER:
          goToLetter(value);
          break;
      case MOVE_STEPS:
          moveBySteps();
          break;
      case SPIN:
          toggleSpin();
      case TEST:
          test();
          break;
      default:
        break;
    }
}

void calibrate() { 
  state = Calibrating;
}

void goToLetter(char value) {
    state = SearchingForLetter;
    target = value;
}

void moveBySteps() {
//  stepper.step(value);
}

void toggleSpin() {
  if(state == Spinning) {
    state = Waiting;
  }
  else {
    state = Spinning;
  }
}

void test() {
  Serial.println("test");
  digitalWrite(LED_BUILTIN, ledTest);
  ledTest = !ledTest;
  completedCurrentState = true;
  Serial.print("completed test");
}

void moveToTarget() { 
  char target = Wire.read();
    int diff = 0; // defines the number of steps we need
    int targetFlap = 0; // defines index of target flap

    for (int i = 0; i < 50; i++) {
        if (flap[i] == target) {
          targetFlap = i; 
        }
    }

    if (currFlap < targetFlap) {   // if current flap is earlier in array than target 
        diff = targetFlap - currFlap; // store difference 

    } else if (currFlap > targetFlap) { // if current flap is later in array than target
        diff = 50 - currFlap + targetFlap;
        
    } else { // if letters are the same
        diff = 50; 
    }

    currFlap = targetFlap; // resets currStep to our new location

    stepper.step(diff*stepsPerIndex); // moved backwards --> needed to make steps a negative 
}

void hallSensorInterrupt() {
  state = OffsetAndFinishCalibration;
}
