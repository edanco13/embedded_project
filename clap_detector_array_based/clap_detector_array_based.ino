#include "Arduino_SensorKit.h"

// define pin numbers
#define LED_PIN 6    // pin number for LED
#define MIC_PIN A2   // pin number for microphone
#define LIGHT_PIN A3 // pin number for light sensor

long longSum; //store the sum of long-term samples
long shortSum; //store the sum of short-term samples
unsigned long inClap = 0; // time stamp for during a clap
unsigned long waitingForMoreClaps = 0; // time stamp for waiting for additional claps
unsigned long previousMillisForOled = 0; // time stamp for OLED display update
int maxShort; // estimated maximum value of short-term samples
int currentclaps; // number of current claps
int data[256]; // array to store samples
int* head = &data[0]; // pointer to the current sample
int* firstNodeOfShortSum = &data[256 - 32]; // pointer to the first sample of short-term sum
int currMode = 0; // current mode
int totalClaps; // total number of claps

// function to shift the sample pointers
void shiftPointers() {
  head++;  // increment the head pointer
  if (head == &data[256]) {
    head = &data[0];  // wrap around if the end of the array is reached
  }
  firstNodeOfShortSum++;  // increment the pointer to the first sample of short-term sum
  if (firstNodeOfShortSum == &data[256]) {
    firstNodeOfShortSum = &data[0];  // wrap around if the end of the array is reached
  }
}
void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT); // set LED pin as output
  pinMode(MIC_PIN, INPUT); // set microphone pin as input
  pinMode(LIGHT_PIN, INPUT); // set light sensor pin as input

  // initialize variables with found mean from python notebook
  longSum = 77 * 256;
  shortSum = 77 * 32;
  maxShort = 77;

  for (int i = 0; i < 256; i++) {
    data[i] = 77;
  }

  Oled.begin(); // initialize the OLED display
  Oled.setFlipMode(true);
  Oled.setFont(u8x8_font_chroma48medium8_r);
}

// function to detect a clap
bool clapDetector() {
  int micValue = analogRead(MIC_PIN); // read the microphone value
  shortSum += micValue - *firstNodeOfShortSum; // update the short-term sum
  longSum += micValue - *head; // update the long-term sum

  maxShort = max(maxShort, micValue); // update the maximum short-term sample value

  // adjust the maximum short-term value if it no longer part of the short-term samples
  if (maxShort == *firstNodeOfShortSum) {
    maxShort = (shortSum >> 5);
  }

  shiftPointers();  // shift the sample pointers
  *head = micValue;  // store the new sample at the head position

  // calculate the clap likeness ratio
  float clapLikeness = float(shortSum >> 5) / float(longSum >> 8);

  // check if a clap is detected based on defined conditions
  if ((inClap <= millis()) && (clapLikeness > 1.5) && (maxShort > 600)) {
    inClap = millis() + 250;  // set the time stamp for next possible clap detection
    maxShort = (shortSum >> 5);  // update the maximum short-term value
    return true; // return true indicating a clap is detected
  }
  return false; // return false indicating no clap is detected
}

void oledPrint(char* text, int line, int value = NULL, bool needToClearFirst = false)
{
  if(needToClearFirst)
    Oled.clear();
  Oled.setCursor(0, line);
  Oled.print(text);
  if(value !=NULL)
    Oled.print(value);
}


void loop() {
  unsigned long currentMillis = millis();  // get the current time

  bool didClaped = clapDetector();  // check if a clap is detected

  if (didClaped) {
    if (currentclaps == 0) {
      waitingForMoreClaps = currentMillis + 1000;  // set the time stamp for waiting for more claps
    } else {
      waitingForMoreClaps + 500;  // add 500 milliseconds to the waiting time
    }
    currentclaps++;  // increment the number of current claps
    oledPrint("current claps:",1,currentclaps,true);
  }

  // check if waiting time is over and there are some claps
  if (waitingForMoreClaps <= currentMillis && currentclaps != 0) {
    totalClaps += currentclaps;  // update the total number of claps
    oledPrint("total claps:",1,totalClaps,true);
    // check the number of claps and perform corresponding actions
    switch (currentclaps) {
      case 0:
        break;
      case 1:
        currMode = 1;  // set the current mode to 1
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));  // toggle the LED pin
        oledPrint("mode led",2);
        break;
      case 2:
        currMode = (currMode == 2) ? 0 : 2;  // toggle between mode 0 and mode 2
        oledPrint("mode light",2);
        break;
      default:
        currMode = currentclaps;  // set the current mode to the number of claps
        break;
    }

    currentclaps = 0;  // reset the number of current claps
  }

  if ((currentMillis - previousMillisForOled >= 500)) {
    switch (currMode) {
      case 2:
        Oled.clearLine(3); //clear the line as the data dynamic
        oledPrint("light: ",3,analogRead(LIGHT_PIN));
        break;
      default:
        break;
    }
    previousMillisForOled = currentMillis;  // update the time stamp
    
  }
}
