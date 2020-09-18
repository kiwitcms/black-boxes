#include <FastLED.h>
#include <Encoder.h>
#include <Throttle.h>
#include "ssd1306.h"
#include "nano_gfx.h"

#define ENCODER_OPTIMIZE_INTERRUPTS
Throttle b = Throttle(3, 2);
Throttle s = Throttle(9);

#define LED_PIN     5
#define GREENBTN_PIN 7
#define REDBTN_PIN 8
#define BLUEBTN_PIN 4
#define NUM_LEDS    7
#define BRIGHTNESS  255
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

Encoder myEnc(2, 3);
#define SWITCH_PIN  9

SAppMenu menu;

const char *menuItems[] =
{
  "Level 1",
  "Level 2",
  "Level 3",
};

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

int oldPosition = 0;


void loop()
{
  b.update();

  if ( b.rose() ) {
    int newPosition = myEnc.read();
    Serial.println(newPosition);

    if (newPosition > oldPosition) {
      ssd1306_menuDown(&menu);
    } else if (newPosition < oldPosition) {
      ssd1306_menuUp(&menu);
    }
    oldPosition = newPosition;
    ssd1306_updateMenu(&menu);
  }

  s.update();

  ssd1306_showMenu(&menu);

  int greenPressed = digitalRead(7);
  int redPressed = digitalRead(8);
  int bluePressed = digitalRead(4);

  if (greenPressed == LOW) {
    Serial.println("GREEN PRESSED");
    for ( uint16_t i = 0; i < NUM_LEDS; i++) {
      delay(250);
      leds[i] = CRGB::Green;
      FastLED.show();
    }
  }
  if (redPressed == LOW) {
    Serial.println("RED PRESSED");
    for ( uint16_t i = 0; i < NUM_LEDS; i++) {
      delay(250);
      leds[i] = CRGB::Red;
      FastLED.show();
    }
  }
  if (bluePressed == LOW) {
    Serial.println("BLUE PRESSED");
    for ( uint16_t i = 0; i < NUM_LEDS; i++) {
      delay(250);
      CRGB choices[3] = { CHSV(HUE_BLUE, 255, 255), CHSV(HUE_RED, 255, 255), CHSV(HUE_GREEN, 255, 255)};
        leds[i] = choices[random(3)];;

      FastLED.show();
    }

  }

  if ( s.rose() ) {
    for ( uint16_t i = 0; i < NUM_LEDS; i++) {
      leds[i].fadeToBlackBy(255);
      delay(250);
      FastLED.show();
    }

    switch (ssd1306_menuSelection(&menu))
    {
      case 0:
        textDemo();
        break;

      case 1:
        ssd1306_printFixed(0, 32, "PLACEHOLDER", STYLE_BOLD);
        break;

      default:
        break;
    }
  }
}

static void textDemo()
{
  ssd1306_clearScreen();
  ssd1306_printFixed(0, 32, "Guess which color is next", STYLE_BOLD);
  delay(5000);
  ssd1306_clearScreen();
}
