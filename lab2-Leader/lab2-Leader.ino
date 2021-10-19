#include <Wire.h>

const int CALIBRATE = 1;
const int GO_TO_LETTER = 2;
const int TEST = 3;

const int numberOfFollowers = 5;
//const int followerAddresses[] = {0, 1, 2, 3, 4, 5, 6, 7};
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

// hallPin = A4 and A5 for I2C??? // ask engineering team - dont think we need this - carlos
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
      // leaderState = SendingReceiving;
      break;
    case (SendingReceiving):
      while (wordCntr < sizeof(poem) / sizeof(poem[0])) { // requires there to be at least one element in poem array
        sendMessages(/* send word as parameter? */);
        receiveMessages(); // hangs until messages complete
        delay(2000); //delay between words
        wordCntr++;
      }
      break;
  }
  delay(1000);
}
void sendMessages() {
  for (int i = 0; i < numberOfFollowers; i++) {
    Wire.beginTransmission(followerAddresses[i]);
    Wire.write(GO_TO_LETTER);
//    Wire.write(poem[wordCntr].at(i));
    Wire.endTransmission();
  }
}

void receiveMessages() {
  int finishedLetter = 0;
  while (finishedLetter < numberOfFollowers) {
    finishedLetter = 0;
    int response = 0;
    for (int i = 0; i < numberOfFollowers; i++) {
      Wire.requestFrom(followerAddresses[i], answerSize);
      while (Wire.available())	{
        response = Wire.read();
      }
      if (response == 1) {
        finishedLetter += 1;
      }
    }
  }
}

void sendCalibrate() {
  Serial.println("test");
  for (int i = 0; i < numberOfFollowers; i++) {
    followerStates[i] = Calibrating;
    Wire.beginTransmission(followerAddresses[i]);
    Wire.write(TEST);
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
  Serial.print("done");
}

bool followerStateContains(FollowerState state) {
  for (int i = 0; i < numberOfFollowers; i++) {
    if (followerStates[i] == state) {
      return true;
    }
  }
  return false;
}
