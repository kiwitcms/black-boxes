#include <FastLED.h>
#include <Encoder.h>
#include <Throttle.h>
#include "ssd1306.h"
#include "nano_gfx.h"

#define ENCODER_OPTIMIZE_INTERRUPTS
Throttle RE = Throttle(3, 2);
Throttle BTN = Throttle(4);


#define SPKR_PIN A0
#define SWITCH_PIN  4
#define LED_PIN     7
#define GREENBTN_PIN 11
#define REDBTN_PIN 12
#define BLUEBTN_PIN 10
#define NUM_LEDS    7
#define BRIGHTNESS  255
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS],
     guesses[NUM_LEDS],
     colors[3] = {CRGB::Red, CRGB::Green, CRGB::Blue};

Encoder myEnc(2, 3);

SAppMenu menu;

const char *menuItems[] =
{
  "Level 1",
  "Level 2",
  "Level 3",
};

int oldPosition = 0;
unsigned short levelSelected = 1,
               pressCounter = 0,
               newPressCounter = 0,
               possibleOutcomes;

boolean chosenLevel = false;

void setup() {
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(  BRIGHTNESS );
  Serial.begin(9600);

  pinMode(GREENBTN_PIN, INPUT_PULLUP);
  pinMode(REDBTN_PIN, INPUT_PULLUP);
  pinMode(BLUEBTN_PIN, INPUT_PULLUP);

  pinMode(SWITCH_PIN, INPUT_PULLUP);
  FastLED.clear();

  ssd1306_128x64_i2c_init();
  ssd1306_setFixedFont(ssd1306xled_font8x16);

  ssd1306_clearScreen();
  ssd1306_createMenu( &menu, menuItems, sizeof(menuItems) / sizeof(char *) );
  ssd1306_showMenu( &menu );

}
void rollbackLeds() {
  for (int8_t i = newPressCounter; i >= 0 ; i--) {
    leds[i].fadeToBlackBy(255);
    delay(200);
    FastLED.show();
  }
}

void winningLedBlink() {  //function that makes the LEDs do a left-loop pattern upon beating the level
  uint8_t i = 0;
  for (uint8_t j = 0; j < 3; ++j) {
    for (i = 0; i < 7; ++i) {
      leds[i].fadeToBlackBy(255);
      delay(50);
      FastLED.show();
    }
    for (i = 0; i < 7; ++i) {
      leds[i] = guesses[i];
      FastLED.show();
      delay(50);
    }
  }
  FastLED.clear();
  FastLED.show();
}

void winningChime() { //function that plays a winning sound when the player beats the level
  uint8_t i = 0;
  for (i = 0; i < 2; ++i) {
    tone(SPKR_PIN, 500);
    delay(175);
    noTone(SPKR_PIN);
  }
  for(i = 0; i < 2; ++i){
  tone(SPKR_PIN, 1000);
  delay(175);
  noTone(SPKR_PIN);
  }
  for (i = 0; i < 3; ++i) {
    tone(SPKR_PIN, 1500);
    delay(175);
    noTone(SPKR_PIN);
  }
}

void ledGenerator(short int lvl) {  //code is very chonky since i didn't really come up with a better solution for it, though it likely exists
  short int possibleOutcomes;       //this generates the 7 LEDs' states that the user has to guess, but it can work with more (or less) than 7
  switch (lvl) {
    case 1:
      unsigned short i;
      possibleOutcomes = random(4);
      switch (possibleOutcomes) {
        case 0:
          for ( i = 0; i < NUM_LEDS / 2; ++i) {
            guesses[i] = colors[0];
          }
          for (i = NUM_LEDS / 2; i < NUM_LEDS; ++i) {
            guesses[i] = colors[1];
          }
          break;
        case 1:
          for (i = 0; i < NUM_LEDS / 2; ++i) {
            guesses[i] = colors[1];
          }
          for (i = NUM_LEDS / 2; i < NUM_LEDS; ++i) { //we generate 2 different patterns with 2 different colors: R-R-R-G-G-G-G, G-G-G-R-R-R-R etc.
            guesses[i] = colors[0];
          }
          break;
        case 2:
          for (i = 0; i < NUM_LEDS / 2 + 1; ++i) {
            guesses[i] = colors[0];
          }
          for (i = NUM_LEDS / 2 + 1; i < NUM_LEDS; ++i) {
            guesses[i] = colors[1];
          }
          break;
        case 3:
          for (i = 0; i < NUM_LEDS / 2 + 1; ++i) {
            guesses[i] = colors[1];
          }
          for (i = NUM_LEDS / 2 + 1; i < NUM_LEDS; ++i) {
            guesses[i] = colors[0];
          }
          break;
      }
      break;
    case 2:
      possibleOutcomes = random(9);
      switch (possibleOutcomes) {
        case 0:
          for (i = 0; i < NUM_LEDS / 3; ++i) {
            guesses[i] = colors[0];
          }
          for (i = NUM_LEDS / 3; i < 2 * (NUM_LEDS / 3); ++i) {
            guesses[i] = colors[1];
          }
          for (i = 2 * (NUM_LEDS / 3); i < NUM_LEDS; ++i) {
            guesses[i] = colors[2];
          }
          break;
        case 1:
          for (i = 0; i < NUM_LEDS / 3; ++i) {
            guesses[i] = colors[1];
          }
          for (i = NUM_LEDS / 3; i < 2 * (NUM_LEDS / 3); ++i) { //this time it's 3 different patterns with 3 different colors: 2-2-3, 2-3-2, etc. with R, G & B
            guesses[i] = colors[2];
          }
          for (i = 2 * (NUM_LEDS / 3); i < NUM_LEDS; ++i) {
            guesses[i] = colors[0];
          }
          break;
        case 2:
          for (i = 0; i < NUM_LEDS / 3; ++i) {
            guesses[i] = colors[2];
          }
          for (i = NUM_LEDS / 3; i < 2 * (NUM_LEDS / 3); ++i) {
            guesses[i] = colors[0];
          }
          for (i = 2 * (NUM_LEDS / 3); i < NUM_LEDS; ++i) {
            guesses[i] = colors[1];
          }
          break;
        case 3:
          for (i = 0; i < NUM_LEDS / 3 + 1; ++i) {
            guesses[i] = colors[0];
          }
          for (i = NUM_LEDS / 3 + 1; i < 2 * (NUM_LEDS / 3) + 1; ++i) {
            guesses[i] = colors[1];
          }
          for (i = 2 * (NUM_LEDS / 3) + 1; i < NUM_LEDS; ++i) {
            guesses[i] = colors[2];
          }
          break;
        case 4:
          for (i = 0; i < NUM_LEDS / 3 + 1; ++i) {
            guesses[i] = colors[1];
          }
          for (i = NUM_LEDS / 3 + 1; i < 2 * (NUM_LEDS / 3) + 1; ++i) {
            guesses[i] = colors[2];
          }
          for (i = 2 * (NUM_LEDS / 3) + 1; i < NUM_LEDS; ++i) {
            guesses[i] = colors[0];
          }
          break;
        case 5:
          for (i = 0; i < NUM_LEDS / 3 + 1; ++i) {
            guesses[i] = colors[2];
          }
          for (i = NUM_LEDS / 3 + 1; i < 2 * (NUM_LEDS / 3) + 1; ++i) {
            guesses[i] = colors[0];
          }
          for (i = 2 * (NUM_LEDS / 3) + 1; i < NUM_LEDS; ++i) {
            guesses[i] = colors[1];
          }
          break;
        case 6:
          for (i = 0; i < NUM_LEDS / 3; ++i) {
            guesses[i] = colors[0];
          }
          for (i = NUM_LEDS / 3; i < 2 * (NUM_LEDS / 3) + 1; ++i) {
            guesses[i] = colors[1];
          }
          for (i = 2 * (NUM_LEDS / 3) + 1; i < NUM_LEDS; ++i) {
            guesses[i] = colors[2];
          }
          break;
        case 7:
          for (i = 0; i < NUM_LEDS / 3; ++i) {
            guesses[i] = colors[1];
          }
          for (i = NUM_LEDS / 3; i < 2 * (NUM_LEDS / 3) + 1; ++i) {
            guesses[i] = colors[2];
          }
          for (i = 2 * (NUM_LEDS / 3) + 1; i < NUM_LEDS; ++i) {
            guesses[i] = colors[0];
          }
          break;
        case 8:
          for (i = 0; i < NUM_LEDS / 3; ++i) {
            guesses[i] = colors[2];
          }
          for (i = NUM_LEDS / 3; i < 2 * (NUM_LEDS / 3) + 1; ++i) {
            guesses[i] = colors[0];
          }
          for (i = 2 * (NUM_LEDS / 3) + 1; i < NUM_LEDS; ++i) {
            guesses[i] = colors[1];
          }
          break;
      }
      break;
    case 3:
      for (i = 0; i < NUM_LEDS; ++i) { //this one is just fully random
        guesses[i] = colors[random(3)];
      }
      break;
  }
}

void loop() {
  BTN.update();
  RE.update();
  if (!chosenLevel) {
    if ( RE.rose() ) {
      int newPosition = myEnc.read();
      if (newPosition > oldPosition) {  //bit of code that reads the encoder and interacts with the level selection
        ssd1306_menuDown(&menu);
        tone(SPKR_PIN, 500);
        delay(50);
        noTone(SPKR_PIN);
        if (levelSelected < 3) {
          ++levelSelected;
        }
        else {
          levelSelected = 1;
        }
      } else if (newPosition < oldPosition) {
        ssd1306_menuUp(&menu);
        tone(SPKR_PIN, 500);
        delay(50);
        noTone(SPKR_PIN);
        if (levelSelected > 1) {
          --levelSelected;
        }
        else {
          levelSelected = 3;
        }
      }
      oldPosition = newPosition;
      ssd1306_updateMenu(&menu);
    }
    ssd1306_showMenu(&menu);
    if (!digitalRead(SWITCH_PIN)) { //starts the game on the selected level
      pressCounter = 0;
      chosenLevel = true;
      ledGenerator(levelSelected);
      ssd1306_clearScreen();
      FastLED.clear();
      FastLED.show();
      tone(SPKR_PIN, 750);
      delay(75);
      noTone(SPKR_PIN);
      delay(300);
    }
  }
  else {
    ssd1306_printFixed(0, 32, "Guess which color is next", STYLE_BOLD);
    if (digitalRead(GREENBTN_PIN) || digitalRead(REDBTN_PIN) || digitalRead(BLUEBTN_PIN)) { //triggers if there's a new button press
      ++newPressCounter;
    }
    if (!digitalRead(SWITCH_PIN)) { //if you press the rotary encoder button while in-game, it brings you back to level selection
      chosenLevel = false;
      newPressCounter = 0;
      pressCounter = 0;
      ssd1306_clearScreen();
      FastLED.clear();
      FastLED.show();
      tone(SPKR_PIN, 750);
      delay(75);
      noTone(SPKR_PIN);
      delay(300);
    }
    if (newPressCounter != pressCounter) {
      if (digitalRead(GREENBTN_PIN) && guesses[pressCounter] == colors[1] && pressCounter != newPressCounter) { //if green button was correct, it lights up and keeps going
        leds[pressCounter] = guesses[pressCounter];                                                             //we check if the button was already pressed or not so it doesn't trigger the rest
        ssd1306_clearScreen();
        ssd1306_printFixed(0, 32, "CORRECT", STYLE_BOLD);
        ++pressCounter;
        FastLED.show();
        tone(SPKR_PIN, 1000);
        delay(150);
        tone(SPKR_PIN, 2000);
        delay(150);
        noTone(SPKR_PIN);
        delay(300);
        ssd1306_clearScreen();
      }
      else if (digitalRead(GREENBTN_PIN) && pressCounter != newPressCounter) {  //if green button was wrong, it shows the correct light and returns to the start
        leds[pressCounter] = guesses[pressCounter];
        FastLED.show();
        chosenLevel = false;
        pressCounter = 0;
        ssd1306_clearScreen();
        ssd1306_printFixed(0, 32, "Wrong!", STYLE_BOLD);
        tone(SPKR_PIN, 1500);
        delay(150);
        tone(SPKR_PIN, 1000);
        delay(150);
        tone(SPKR_PIN, 500);
        delay(150);
        noTone(SPKR_PIN);
        delay(500);
        rollbackLeds();
        ssd1306_clearScreen();
        newPressCounter = 0;
      }
      if (digitalRead(REDBTN_PIN) && guesses[pressCounter] == colors[0] && pressCounter != newPressCounter) { //if the red button was correct, it lights up and keeps going
        leds[pressCounter] = guesses[pressCounter];
        ssd1306_clearScreen();
        ssd1306_printFixed(0, 32, "CORRECT", STYLE_BOLD);
        ++pressCounter;
        FastLED.show();
        tone(SPKR_PIN, 1000);
        delay(150);
        tone(SPKR_PIN, 2000);
        delay(150);
        noTone(SPKR_PIN);
        delay(300);
        ssd1306_clearScreen();
      }
      else if (digitalRead(REDBTN_PIN) && pressCounter != newPressCounter) {  //if the red button was wrong, it shows the correct light and then returns to the start
        leds[pressCounter] = guesses[pressCounter];
        FastLED.show();
        chosenLevel = false;
        pressCounter = 0;
        ssd1306_clearScreen();
        ssd1306_printFixed(0, 32, "Wrong!", STYLE_BOLD);
        tone(SPKR_PIN, 1500);
        delay(150);
        tone(SPKR_PIN, 1000);
        delay(150);
        tone(SPKR_PIN, 500);
        delay(150);
        noTone(SPKR_PIN);
        delay(500);
        rollbackLeds();
        ssd1306_clearScreen();
        newPressCounter = 0;
      }

      if (digitalRead(BLUEBTN_PIN) && guesses[pressCounter] == colors[2] && pressCounter != newPressCounter) {  //if the blue button was correct, it lights up and keeps going
        leds[pressCounter] = guesses[pressCounter];
        ssd1306_clearScreen();
        ssd1306_printFixed(0, 32, "CORRECT", STYLE_BOLD);
        ++pressCounter;
        FastLED.show();
        tone(SPKR_PIN, 1000);
        delay(150);
        tone(SPKR_PIN, 2000);
        delay(150);
        noTone(SPKR_PIN);
        delay(300);
        ssd1306_clearScreen();
      }
      else if (digitalRead(BLUEBTN_PIN) && pressCounter != newPressCounter) { //if the blue button was wrong, it shows the correct light and returns to the start
        leds[pressCounter] = guesses[pressCounter];
        FastLED.show();
        chosenLevel = false;
        pressCounter = 0;
        ssd1306_clearScreen();
        ssd1306_printFixed(0, 32, "Wrong!", STYLE_BOLD);
        tone(SPKR_PIN, 1500);
        delay(150);
        tone(SPKR_PIN, 1000);
        delay(150);
        tone(SPKR_PIN, 500);
        delay(150);
        noTone(SPKR_PIN);
        delay(500);
        rollbackLeds();
        ssd1306_clearScreen();
        newPressCounter = 0;
      }
      if (pressCounter == NUM_LEDS) {
        ssd1306_clearScreen();
        ssd1306_printFixed(0, 32, "YOU WIN!", STYLE_BOLD);
        winningChime();
        winningLedBlink();
        ssd1306_clearScreen();
        chosenLevel = false;
        pressCounter = 0;
        newPressCounter = 0;
      }
    }
  }
}
