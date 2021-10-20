#include <Wire.h>

const byte CALIBRATE = 1;
const byte GO_TO_LETTER = 2;
const byte MOVE_STEPS = 3;
const byte SPIN = 4;
const byte TEST = 5;

const int numberOfFollowers = 5;
const int followerAddresses[] = {8, 9, 10, 11, 12};

enum LeaderState {
  Initializing,
  CalibrateFollowers,
  SendingReceiving,
  DoNothing,
};
LeaderState leaderState = Initializing;
enum FollowerState {
  Waiting,
  Calibrating,
  SearchingForLetter,
  Spinning,
};
FollowerState followerStates[numberOfFollowers];

bool calibrated = false;
const int answerSize = 1;
int wordCntr = 0;

String poem [] = {"The     ", "Robots  ", "Are     ", "Coming  "};

void setup() {
  for (int i = 0; i < sizeof(followerStates) / sizeof(followerStates[0]); i++) {
    followerStates[i] = Waiting;
  }
  leaderState = CalibrateFollowers;
  Wire.begin();
  Serial.begin(9600);
}

void loop() {
  switch (leaderState) {
    case (CalibrateFollowers):
      sendCalibrate();
      leaderState = SendingReceiving;
      break;
    case (SendingReceiving):
      sendCharacter();
      leaderState = DoNothing;
//      while (wordCntr < sizeof(poem) / sizeof(poem[0])) { // requires there to be at least one element in poem array
//        sendMessages(/* send word as parameter? */);
//        receiveMessages(); // hangs until messages complete
//        delay(2000); //delay between words
//        wordCntr++;
//      }
//      break;
  }
  delay(1000);
}
void sendCharacter() {
  for (int i = 0; i < numberOfFollowers; i++) {
    Wire.beginTransmission(followerAddresses[i]);
    followerStates[i] = Calibrating;
    Wire.write(GO_TO_LETTER);
    Wire.write('q');
//    Wire.write(poem[wordCntr].at(i));
    Wire.endTransmission();
  }
  int response = 0;
  while (!followerStateContains(SearchingForLetter)) {
    for (int i = 0; i < numberOfFollowers; i++) {
      if (followerStates[i] = Waiting) {
        continue;
      }
      Wire.requestFrom(followerAddresses[i], answerSize);
      while (Wire.available()) {
        response = Wire.read();
      }
      if (response == 1) {
        followerStates[i] = Waiting;
      }
    }
  }
}

void sendCalibrate() {
  Serial.println("sending calibrate to followers");
  for (int i = 0; i < numberOfFollowers; i++) {
    followerStates[i] = Calibrating;
    Wire.beginTransmission(followerAddresses[i]);
    Wire.write(CALIBRATE);
    Wire.write(CALIBRATE);
    Wire.endTransmission();
  }
  int response = 0;
  while (!followerStateContains(Calibrating)) {
    for (int i = 0; i < numberOfFollowers; i++) {
      if (followerStates[i] = Waiting) {
        continue;
      }
      Wire.requestFrom(followerAddresses[i], answerSize);
      while (Wire.available()) {
        response = Wire.read();
      }
      if (response == 1) {
        followerStates[i] = Waiting;
      }
    }
  }
  Serial.print("followers done calibrating");
}

void sendSpin() {

}

bool followerStateContains(FollowerState state) {
  for (int i = 0; i < numberOfFollowers; i++) {
    if (followerStates[i] == state) {
      return true;
    }
  }
  return false;
}
