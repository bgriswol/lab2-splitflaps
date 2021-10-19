#include <Wire.h>
#include <Stepper.h>

const int CALIBRATE = 1;
const int GO_TO_LETTER = 2;
const int TEST = 3;

const int FollowerAddress = 11;
const int hallPin = 2;
volatile bool startFound = false;
String flap [] = {":)", ":(", "1", "2", "3", "4", "5", "6", "7", "8", "9", "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z", "!", "$", "%", "&", "*", "(", ")", "?", "+", "=", "/", "-"};

const int stepsPerIndex = 32; 
const int stepsPerRevolution = 400; // This is the steps per revolution
int currStep = 0; // tracks the current location of our stepper
int currFlap = 0; 
bool calibrated = false; 
bool completedCurrentState = false;

bool ledTest = LOW;
bool goCalibrate = false;


Stepper stepper(stepsPerRevolution, 3, 4);

void setup() {
    // put your setup code here, to run once:
    pinMode(hallPin,INPUT_PULLUP); // original code used INPUT as 2nd parameter
    stepper.setSpeed(31.0); // constant speed for stepper.
    Wire.begin(FollowerAddress);
    Wire.onRequest(requestEvent); 
    Wire.onReceive(receiveEvent); 

    pinMode(LED_BUILTIN, OUTPUT);

    Serial.begin(9600);
//
    Serial.println("follower");

    attachInterrupt(digitalPinToInterrupt(hallPin), hallSensorInterrupt, RISING);
}

void loop() {
  if(goCalibrate) {
    stepper.step(1);
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
    completedCurrentState = false;
    int cmd;
    String value;
    Serial.println("received event");
    while (0 < Wire.available()){
        cmd = Wire.read();
        switch(cmd) {
            case TEST:
                test();
            case CALIBRATE:
                calibrate();
                break;
            case GO_TO_LETTER:
                value = Wire.read();
                moveToTarget(value);
                break;
        }
    }
}

void test() {
  Serial.println("test");
  digitalWrite(LED_BUILTIN, ledTest);
  ledTest = !ledTest;
  completedCurrentState = true;
  Serial.print("completed test");
}

void calibrate() { 
//  Serial.println("calibrate follower");
//    while (digitalRead(hallPin) == 1) {
//        stepper.step(1); // keep moving 1 step at a time until sensor detects start position
//    }
//    calibrated = true;
//    Serial.print("completed calibrate");
  goCalibrate = true;
}

void moveToTarget(String target) { 
//    int diff = 0; // defines the number of steps we need
//    int targetFlap = 0; // defines index of target flap
//
//    for (int i = 0; i < 50; i++) {
//        if (flap[i].equals(target)) {
//          targetFlap = i; 
//        }
//    }
//
//    if (currFlap < targetFlap) {   // if current flap is earlier in array than target 
//        diff = targetFlap - currFlap; // store difference 
//
//    } else if (currFlap > targetFlap) { // if current flap is later in array than target
//        diff = 50 - currFlap + targetFlap;
//        
//    } else { // if letters are the same
//        diff = 50; 
//    }
//
//    currFlap = targetFlap; // resets currStep to our new location
//
//    stepper.step(-diff*stepsPerIndex); // moved backwards --> needed to make steps a negative 
}


void hallSensorInterrupt() {
//  detachInterrupt(digitalPinToInterrupt(hallPin));
  completedCurrentState = true;
  calibrated = true;
  goCalibrate = false;
}
