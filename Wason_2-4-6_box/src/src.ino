#include "SevenSegmentTM1637.h"
#include "SevenSegmentExtended.h"
#include "SevenSegmentFun.h"
#include <Keypad.h>

#define R_LED  2
#define G_LED  3
#define SPKR  A4

// Initialize the TM1637 instances
// We share a common CLK for all three displays, it's hacky but it works
const byte PIN_CLK = 12,
           PIN_DIO0 = A0,
           PIN_DIO1 = A1,
           PIN_DIO2 = A2;
SevenSegmentTM1637    display0(PIN_CLK, PIN_DIO0);
SevenSegmentTM1637    display1(PIN_CLK, PIN_DIO1);
SevenSegmentTM1637    display2(PIN_CLK, PIN_DIO2);

void keypadEvent(char key) {
}

// Configure the keypad
const byte ROWS = 4, //four rows
           COLS = 4; //four columns
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[ROWS] = {4, 5, 6, 7}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 9, 10, 11}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {

  Serial.begin(9600);
  display0.begin();
  display0.setBacklight(20);
  display1.begin();
  display0.setBacklight(20);
  display2.begin();
  display2.setBacklight(20);
  keypad.addEventListener(keypadEvent);

}

long int numbers[3];

boolean playing = false,
        gameStarted = false,
        levelChosen = false;
unsigned short currentDisplay = 0,
               levelSelected = 1,
               numOneDigits = 0,
               numTwoDigits = 0,
               numThreeDigits = 0;

void buttonSpeaker() {  //piece of code that makes noise when a button is pressed
  tone(SPKR, 750);
  delay(75);
  noTone(SPKR);
}

void correctLamps() {
  digitalWrite(G_LED, HIGH);  //functions for lighting up LEDs and making noises to tell the user whether they're correct or wrong
  digitalWrite(R_LED, LOW);
  tone(SPKR, 1000);
  delay(150);
  tone(SPKR, 2000);
  delay(150);
  noTone(SPKR);
}

void wrongLamps() {
  digitalWrite(R_LED, HIGH);  //these 2 functions are going to be used pretty often so they're separate
  digitalWrite(G_LED, LOW);
  tone(SPKR, 1500);
  delay(150);
  tone(SPKR, 1000);
  delay(150);
  tone(SPKR, 500);
  delay(150);
  noTone(SPKR);
}

void displayer(unsigned short current, unsigned int value, char action) { //displaying numbers is understandably necessary very often, so it's a function
  if (action == 'p') {                                                  //(also it works with the simple display selection code without any fuss)
    if (current == 0) {
      display0.clear();
      display0.print(value);
    }
    if (current == 1) {
      display1.clear();
      display1.print(value);
    }
    if (current == 2) {
      display2.clear();
      display2.print(value);
    }
  }
  if (action == 'c') {
    if (current == 0) {
      numOneDigits = 0;
      numbers[0] = 0;
      display0.clear();
      display0.print(0);
    }
    if (current == 1) {
      numbers[1] = 0;
      numTwoDigits = 0;
      display1.clear();
      display1.print(0);
    }
    if (current == 2) {
      numbers[2] = 0;
      numThreeDigits = 0;
      display2.clear();
      display2.print(0);
    }
  }
  if (action == 'b') {
    if (current == 0) {
      display0.blink();
    }
    if (current == 1) {
      display1.blink();
    }
    if (current == 2) {
      display2.blink();
    }
  }
}
void numBuilder(unsigned short current, unsigned int digits1, unsigned int digits2, unsigned int digits3, unsigned short inputDigit) {
  if (levelChosen) {
    if (current == 0) {
      if (digits1 < 4) {
        if (digits1 == 0) {
          ++numOneDigits;
          numbers[0] = inputDigit;
        }
        else {
          ++numOneDigits;
          numbers[0] = numbers[0] * 10 + inputDigit;
        }
      }
    }
    if (current == 1) {
      if (digits2 < 4) {
        if (digits2 == 0) {
          ++numTwoDigits;
          numbers[1] = inputDigit;
        }
        else if (digits2 < 4) {
          ++numTwoDigits;
          numbers[1] = numbers[1] * 10 + inputDigit;
        }
      }
    }
    if (current == 2) {
      if (digits3 < 4) {
        if (digits3 == 0) {
          ++numThreeDigits;
          numbers[2] = inputDigit;
        }
        else if (digits3 < 4) {
          ++numThreeDigits;
          numbers[2] = numbers[2] * 10 + inputDigit;
        }
      }
    }
  }
  else {
    numbers[1] = 1;
    if (numbers[1] > 0 && numbers[1] < 15) {
      if (digits2 == 0) {
        ++numTwoDigits;
        numbers[1] = inputDigit;
      }
      else {
        ++numTwoDigits;
        numbers[1] = numbers[1] * 10 + inputDigit;
      }
    }
    else if (numbers[1] < 1) {
      numbers[1] = 1;
    }
    else if (numbers[1] > 15) {
      numbers[1] = 15;
    }
  }
}

void checkNum(unsigned short lvl) { //code for checking numbers is pretty chonky, so i made it into a separate function
  unsigned int num1 = numbers[0],
               num2 = numbers[1],
               num3 = numbers[2];
  switch (lvl) {
    case 1:
      if (num1 % 2 == 0 && num2 % 2 == 0 && num3 % 2 == 0) {
        correctLamps();
      }
      else {
        wrongLamps();
      }    break;

    case 2:
      if (num1 % 2 != 0 && num2 % 2 != 0 && num3 % 2 != 0) {
        correctLamps();
      }
      else {
        wrongLamps();
      }   break;

    case 3:
      if (num3 == num2 + num1) {
        correctLamps();
      }
      else {
        wrongLamps();
      } break;

    case 4:
      if (num1 < num2 && num2 < num3) {
        correctLamps();
      }
      else {
        wrongLamps();
      } break;

    case 5:
      if (num1 > num2 && num2 > num3) {
        correctLamps();
      }
      else {
        wrongLamps();
      } break;

    case 6:
      if (num1 < num2 && num2 < num3) {
        boolean powOne = false,
                powTwo = false,
                powThree = false;
        unsigned long power;
        for (unsigned short i = 0; i < 14; i ++) {
          power = pow(2, i) + 1;  //calculating it here so it's only done once per loop
          if (num1 == power) {
            powOne = true;
          }
          if (num2 == power) {  //checking if they're a power of 2
            powTwo = true;
          }
          if (num3 == power) {
            powThree = true;
          }
          if (powOne && powTwo && powThree) {
            correctLamps();
            break;
          }
          else {
            wrongLamps();
          }
        }
      }
      else {
        wrongLamps();
      }
      break;
    case 7:
      if (num1 < num2 && num2 < num3) {
        boolean div3One = false,
                div3Two = false,
                div3Three = false;
        if (num1 % 3 == 0) {
          div3One = true;
        }
        if (num2 % 3 == 0) {
          div3Two = true;
        }
        if (num3 % 3 == 0) {
          div3Three = true;
        }
        if (div3One && div3Two && div3Three) {
          correctLamps();
          break;
        }
        else {
          wrongLamps();
        }
      }
      else {
        wrongLamps();
      }
      break;
    case 8:
      if (num2 > num1 && num2 > num3) {
        boolean div5One = false,
                div5Two = false,
                div5Three = false;
        if (num1 % 5 == 0) {
          div5One = true;
        }
        if (num2 % 5 == 0) {
          div5Two = true;
        }
        if (num3 % 5 == 0) {
          div5Three = true;
        }
        if (div5One && div5Two && div5Three) {
          correctLamps();
          break;
        }
        else {
          wrongLamps();
        }
      }
      else {
        wrongLamps();
      }
      break;
    case 9:
      if (num1 > num2 && num2 > num3) {
        boolean div7One = false,
                div7Two = false,
                div7Three = false;
        if (num1 % 7 == 0) {
          div7One = true;
        }
        if (num2 % 7 == 0) {
          div7Two = true;
        }
        if (num3 % 7 == 0) {
          div7Three = true;
        }
        if (div7One && div7Two && div7Three) {
          correctLamps();
          break;
        }
        else {
          wrongLamps();
        }
      }
      else {
        wrongLamps();
      }
      break;
    case 10:
      if (num1 == num2 && num2 == num3) {
        boolean div11One = false,
                div11Two = false,
                div11Three = false;
        if (num1 % 11 == 0) {
          div11One = true;
        }
        if (num2 % 11 == 0) {
          div11Two = true;
        }
        if (num3 % 11 == 0) {
          div11Three = true;
        }
        if (div11One && div11Two && div11Three) {
          correctLamps();
          break;
        }
        else {
          wrongLamps();
        }
      }
      else {
        wrongLamps();
      }
      break;
    case 11:
      if (num1 < num2 && num2 < num3) {
        boolean isPrime = true;
        for (unsigned int i = 2; i < num3 / 2; i++) {
          if (num1 % i == (float) 0) {
            isPrime = false;
            break;
          }
          if (num2 % i == (float) 0) {
            isPrime = false;  //making sure all 3 numbers are prime, if any of them aren't the check will return negative
            break;
          }
          if (num3 % i == (float) 0) {
            isPrime = false;
            break;
          }
        }
        if (isPrime) {
          correctLamps();
        }
        else {
          wrongLamps();
        }
      }
      else {
        wrongLamps();
      }
      break;
    case 12:
      if (num1 < num2 && num2 < num3) {
        boolean squares = true;
        float static decimalParts[3];
        unsigned short static integerParts[3];
        integerParts[0] = (int)sqrt(num1);
        integerParts[1] = (int)sqrt(num2);
        integerParts[2] = (int)sqrt(num3);
        decimalParts[0] = sqrt(num1) - integerParts[0]; //checking if numbers are squares of anything, for that their roots must have 0 decimals
        decimalParts[1] = sqrt(num2) - integerParts[1];
        decimalParts[2] = sqrt(num3) - integerParts[2];
        for (unsigned short i = 0; i < 3; i++) {
          if (decimalParts[i] < 1 && decimalParts[i] > 0) {
            squares = false;
            break;
          }
        }
        if (squares) {
          correctLamps();
        }
        else {
          wrongLamps();
        }
      }
      else {
        wrongLamps();
      }
      break;
    case 13:
      if (num1 == num2 && num2 == num3) {
        for (unsigned int i = 1; i < 141; i++)
          if (num1 = (i * (i + 1)) / 2) {
            correctLamps();
            break;
          }
      }
      else {
        wrongLamps();
      }
      break;
    case 14:
      if (num1 <= num2 && num2 <= num3) {
        boolean fibonacciNum1 = false,
                fibonacciNum2 = false,
                fibonacciNum3 = false;
        unsigned int prevNum = 0,
                     currNum = 1,
                     nextSum;
        for (unsigned int i = 1; i < 22; i++) {
          if (num1 != prevNum && !fibonacciNum1) {
            fibonacciNum1 = false;
          }
          else {
            fibonacciNum1 = true;
          }
          if (num2 != prevNum && !fibonacciNum2) {  //i feel like this method is a bit dirty, but it's simple & it works with just one loop
            fibonacciNum2 = false;
          }
          else {
            fibonacciNum2 = true;
          }
          if (num3 != prevNum && !fibonacciNum3) {
            fibonacciNum3 = false;
          }
          else {
            fibonacciNum3 = true;
          }
          if (fibonacciNum1 && fibonacciNum2 && fibonacciNum3) {
            break;
          }
          nextSum = prevNum + currNum;
          prevNum = currNum;
          currNum = nextSum;
        }
        if (fibonacciNum1 && fibonacciNum2 && fibonacciNum3) {
          correctLamps();
        }
        else {
          wrongLamps();
        }
      }
      else {
        wrongLamps();
      }
      break;
    case 15:
      correctLamps(); //couldn't think of a proper level, so it's just green lamps all the way as per the specification
      break;
  }
}
void loop() {
  char key = keypad.getKey();


  if (playing) {
    switch (key) {
      case '#':
        buttonSpeaker();
        if (currentDisplay < 2) { //selecting what display to write to
          ++currentDisplay;
        }
        else {
          currentDisplay = 0;
        }
        break;
      case '*':
        if (gameStarted) {
          checkNum(levelSelected);
        }
        else {
          buttonSpeaker();
          if (levelSelected > 1) {
            if (levelSelected == 10) {
              display1.clear();
              display1.setBacklight(20);  //doing this so it doesn't print 90, 80 etc when lowering back to single digit levels
            }
            --levelSelected;
            display1.clear();
            display1.print(levelSelected);
            numbers[1] = levelSelected;
          }
        }
        break;
      case 'A':
        buttonSpeaker();
        numOneDigits = 0;
        numTwoDigits = 0;
        numThreeDigits = 0; //making sure there's no weirdness due to residual values and stuff, so it acts as both start & restart
        numbers[0] = 0;
        numbers[1] = 0;
        numbers[2] = 0;
        display0.off();
        display0.setBacklight(20);
        display0.clear();
        display0.print(0);
        display1.clear();
        display1.print(0);  //have to clear and THEN print because buggy ass displays don't wanna cooperate otherwise
        display2.clear();
        display2.print(0);
        gameStarted = true; //starting on current level
        levelChosen = true;
        currentDisplay = 0;
        break;
      case 'B':
        buttonSpeaker();
        levelChosen = false;
        playing = false;
        gameStarted = false;  //the button to stop playing and turn stuff off
        levelSelected = 1;
        numbers[0] = 0;
        numbers[1] = 0;
        numbers [2] = 0;
        numOneDigits = 0;
        numTwoDigits = 0;
        numThreeDigits = 0;
        break;
      case 'D':
        buttonSpeaker();
        if (gameStarted) {
          displayer(currentDisplay, 0, 'b');
        }
        else if (levelSelected < 15) {
          ++levelSelected;
          display1.print(levelSelected);
          numbers[1] = levelSelected;
        }
        break;
      case 'C':
        buttonSpeaker();
        if (levelChosen) {
          displayer(currentDisplay, 0, 'c');  //clears current display
        }
        else {
          displayer(1, 1, 'p');
        }
        break;
      case '0':
        buttonSpeaker();
        if (levelChosen) {
          numBuilder(currentDisplay, numOneDigits, numTwoDigits, numThreeDigits, 0);
          displayer(currentDisplay, numbers[currentDisplay], 'p');
        }
        else {
          if (levelSelected > 9) {
            numBuilder(1, 0, numTwoDigits, 0, 0);
            displayer(1, numbers[1], 'p');
            levelSelected = numbers[1];
          }
        }
        break;
      case '1':
        buttonSpeaker();
        if (levelChosen) {
          numBuilder(currentDisplay, numOneDigits, numTwoDigits, numThreeDigits, 1);
          displayer(currentDisplay, numbers[currentDisplay], 'p');
        }
        else {
          numBuilder(1, 0, numTwoDigits, 0, 1);
          displayer(1, numbers[1], 'p');
          levelSelected = numbers[1];
        }
        break;
      case '2':
        buttonSpeaker();
        if (levelChosen) {
          numBuilder(currentDisplay, numOneDigits, numTwoDigits, numThreeDigits, 2);
          displayer(currentDisplay, numbers[currentDisplay], 'p');
        }
        else {
          numBuilder(1, 0, numTwoDigits, 0, 2);
          displayer(1, numbers[1], 'p');
          levelSelected = numbers[1];
        }
        break;
      case '3':
        buttonSpeaker();
        if (levelChosen) {
          numBuilder(currentDisplay, numOneDigits, numTwoDigits, numThreeDigits, 3);
          displayer(currentDisplay, numbers[currentDisplay], 'p');
        }
        else {
          numBuilder(1, 0, numTwoDigits, 0, 3);
          displayer(1, numbers[1], 'p');
          levelSelected = numbers[1];
        }
        break;
      case '4':
        buttonSpeaker();
        if (levelChosen) {
          numBuilder(currentDisplay, numOneDigits, numTwoDigits, numThreeDigits, 4);
          displayer(currentDisplay, numbers[currentDisplay], 'p');
        }
        else {
          numBuilder(1, 0, numTwoDigits, 0, 4);
          displayer(1, numbers[1], 'p');
          levelSelected = numbers[1];
        }
        break;
      case '5':
        buttonSpeaker();
        if (levelChosen) {
          numBuilder(currentDisplay, numOneDigits, numTwoDigits, numThreeDigits, 5);
          displayer(currentDisplay, numbers[currentDisplay], 'p');
        }
        else {
          numBuilder(1, 0, numTwoDigits, 0, 5);
          displayer(1, numbers[1], 'p');
          levelSelected = numbers[1];
        }
        break;
      case '6':
        buttonSpeaker();
        if (levelChosen) {
          numBuilder(currentDisplay, numOneDigits, numTwoDigits, numThreeDigits, 6);
          displayer(currentDisplay, numbers[currentDisplay], 'p');
        }
        else {
          numBuilder(1, 0, numTwoDigits, 0, 6);
          displayer(1, numbers[1], 'p');
          if (numbers[1] > 9) {
            display1.clear();
            numbers[1] = 15;
            display1.print(15);
          }
          levelSelected = numbers[1];
        }
        break;
      case '7':
        buttonSpeaker();
        if (levelChosen) {
          numBuilder(currentDisplay, numOneDigits, numTwoDigits, numThreeDigits, 7);
          displayer(currentDisplay, numbers[currentDisplay], 'p');
        }
        else {
          numBuilder(1, 0, numTwoDigits, 0, 7);
          displayer(1, numbers[1], 'p');
          if (numbers[1] > 9) {
            display1.clear();
            numbers[1] = 15;
            display1.print(15);
          }
          levelSelected = numbers[1];
        }
        break;
      case '8':
        buttonSpeaker();
        if (levelChosen) {
          numBuilder(currentDisplay, numOneDigits, numTwoDigits, numThreeDigits, 8);
          displayer(currentDisplay, numbers[currentDisplay], 'p');
        }
        else {
          numBuilder(1, 0, numTwoDigits, 0, 8);
          displayer(1, numbers[1], 'p');
          if (numbers[1] > 9) {
            display1.clear();
            numbers[1] = 15;
            display1.print(15);
          }
          levelSelected = numbers[1];
        }
        break;
      case '9':
        buttonSpeaker();
        if (levelChosen) {
          numBuilder(currentDisplay, numOneDigits, numTwoDigits, numThreeDigits, 9);
          displayer(currentDisplay, numbers[currentDisplay], 'p');
        }
        else {
          numBuilder(1, 0, numTwoDigits, 0, 9);
          displayer(1, numbers[1], 'p');
          if (numbers[1] > 9) {
            display1.clear();
            numbers[1] = 15;
            display1.print(15);
          }
          levelSelected = numbers[1];
        }
        break;
    }
  }
  else {
    if (key == '#') {
      buttonSpeaker();
      display0.setBacklight(20);
      display1.setBacklight(20);  //starting level selection
      display2.setBacklight(20);
      display0.print("LVL?");
      display1.print(levelSelected);
      playing = true;
      currentDisplay = 1;
    }
    else {
      digitalWrite(R_LED, LOW); //turning everything off to reduce power consumption when on standby but not fully turned off
      digitalWrite(G_LED, LOW);
      display0.off();
      display1.off();
      display2.off();
    }
  }
}
