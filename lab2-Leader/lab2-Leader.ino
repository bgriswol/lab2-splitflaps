#include <Wire.h>

const byte CALIBRATE = 1;
const byte GO_TO_LETTER = 2;
const byte MOVE_STEPS = 3;
const byte SPIN = 4;
const byte TEST = 5;

//const int numberOfFollowers = 5;
//const int followerAddresses[] = {8, 9, 10, 11, 12};
const int numberOfFollowers = 2;
const int followerAddresses[] = {10, 11};

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

//String poem [] = {"The     ", "Robots  ", "Are     ", "Coming  "};
String poem = Serial.readString();
String poemArray[sizeof(poem)] = {};
int wordNum = 0;

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
      delay(1000);
      break;
    case (SendingReceiving):
      for (int a=0; a<wordNum; a++) {
        sendCharacter(a);
        delay(2000);
      }
      leaderState = DoNothing;
//      while (wordCntr < sizeof(poem) / sizeof(poem[0])) { // requires there to be at least one element in poem array
//        sendMessages(/* send word as parameter? */);
//        receiveMessages(); // hangs until messages complete
//        delay(2000); //delay between words
//        wordCntr++;
//      }
      break;
  }
  delay(1000);
}
void sendCharacter() {
  Serial.println("sending letter");
  for (int i = 0; i < numberOfFollowers; i++) {
    Wire.beginTransmission(followerAddresses[i]);
    followerStates[i] = SearchingForLetter;
    Wire.write(GO_TO_LETTER);
    Wire.write(poemArray[wordNum].charAt(i));
    Wire.endTransmission();
  }
  int response = 0;
  while (followerStateContains(SearchingForLetter)) {
    for (int i = 0; i < numberOfFollowers; i++) {
      Wire.requestFrom(followerAddresses[i], answerSize);
      while (Wire.available()) {
        response = Wire.read();
      }
      if (response == 1) {
        followerStates[i] = Waiting;
      }
    }
  }
  Serial.println("done sending letter");
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
  while (followerStateContains(Calibrating)) { // will hang here until calibration is done for each follower
    for (int i = 0; i < numberOfFollowers; i++) {
//      if (followerStates[i] = Waiting) {
//        continue;
//      }
      Wire.requestFrom(followerAddresses[i], answerSize);
      while (Wire.available()) {
        response = Wire.read();
      }
      if (response == 1) {
        followerStates[i] = Waiting;
      }
    }
  }
  Serial.println("followers done calibrating");
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

// Splitting up array below 

String createPoemArray() {
  boolean cont = true;
  int wordLength = 0;
   
  while (cont) {
     wordLength = lengthOfWord(poem, 0);
     String newWord = poem.substring(0, wordLength-1);
     if (newWord.length()<=8) {
         int spaces = 8-newWord.length();
         for (int s=0; s<spaces; s++) {
             newWord = newWord + " ";
         }
         poemArray[wordNum] = newWord;
         wordNum++;
     } else {       
         int revolutions = ceil(wordLength/7);
         for (int b=0; b<revolutions; b++) {
             if (newWord.length() > 8) {
                 String w = newWord.substring(0, 6);
                 newWord = newWord + "-";
                 poemArray[wordNum] =  w;
                 wordNum++;
                 int wordLen = newWord.length();
                 newWord = newWord.substring(6, wordLen);
             } else {
                 int spaces = 8-newWord.length();
                 for (int s=0; s<spaces; s++) {
                    newWord = newWord + " ";
                  }
                 poemArray[wordNum] = newWord;
                 wordNum++;
               }
            }
     }
       int len = poem.length();
       poem = poem.substring(wordLength+1, len);
       if (poem.length()==0) {
           cont = false;
       }
  }
}
 
int lengthOfWord(String poem, int num) {
   if (!poem.charAt(num)==" ") {
       num++;
       lengthOfWord(poem, num);
   } else {
       return num;
   }
}
 
