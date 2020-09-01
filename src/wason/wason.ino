#include <Encoder.h>
#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"
#include "SevenSegmentFun.h"
#include <Keypad.h>
#include <Throttle.h>


Throttle b = Throttle(2, 3);
Throttle s = Throttle(4);


#define ENCODER_OPTIMIZE_INTERRUPTS
Encoder myEnc(3,2);
#define SWITCH_PIN  4

// Initialize the TM1637 instances
// We share a common CLK for all three displays, it's hacky but it works
const byte PIN_CLK = 12;
const byte PIN_DIO0 = A0;
const byte PIN_DIO1 = A1;
const byte PIN_DIO2 = A2;
SevenSegmentTM1637    display0(PIN_CLK, PIN_DIO0);
SevenSegmentTM1637    display1(PIN_CLK, PIN_DIO1);
SevenSegmentTM1637    display2(PIN_CLK, PIN_DIO2);

// Configure the keypad
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rowPins[ROWS] = {5, 6, 7, 8}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 10, 11}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  b.interval(50);


  pinMode(SWITCH_PIN, INPUT_PULLUP);
  Serial.begin(9600);
  display0.begin();
  display0.setBacklight(20);
  display1.begin();
  display0.setBacklight(20);
  display2.begin();
  display2.setBacklight(20);
  pinMode(4, INPUT_PULLUP);
}

int oldPosition = 0;
int levelSelected;

void loop() {

  b.update();
  s.update();

  if ( s.changed() ) {
    if(digitalRead(4) == HIGH) {
    } else {
      Serial.println(oldPosition);
      levelSelected = oldPosition;
      display1.print("PLAY");
      display1.blink(500);
      display0.clear();
      display1.clear();
    }
  }

  if ( b.rose() ) {
    long newPosition = myEnc.read();
    int minLevel = 1;
    int maxLevel = 15;
    if (newPosition <= minLevel) {
      myEnc.write(minLevel);
      newPosition = minLevel;
    } else if (newPosition >= maxLevel) {
      myEnc.write(maxLevel);
      newPosition = maxLevel;
    }
    if (newPosition > oldPosition) {
      newPosition = oldPosition + 1;
      // Serial.println(newPosition);
      display0.print(newPosition);

    } else if (newPosition < oldPosition) {
      newPosition = oldPosition - 1;
      display0.clear();
      // Serial.println(newPosition);
      display0.print(newPosition);
    }
    oldPosition = newPosition;
  }

  char key = keypad.getKey();
  if (key) {
    //Serial.println(key);
    display2.print(key);
  }
}
