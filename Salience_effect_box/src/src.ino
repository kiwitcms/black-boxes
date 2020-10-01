#include <Encoder.h>
#include <Throttle.h>


Throttle b = Throttle(2, 3);
Encoder myEnc(2, 3);
#define ENCODER_OPTIMIZE_INTERRUPTS

int G_LED = 6;
int R_LED2 = 7;
#define R_LED1 A1
int SM_BTN = 5;
int HT_BTN = 9;
int LG_BTN = 10;
int K_BTN = 8;
long alarmLength = 5000;
long beepLength = 2500;
unsigned long alarmStart = 0;
unsigned long smBtnTime = 0;
long oldPosition = 0;
long newPosition;
#define SPKR A0
void setup() {
  Serial.begin(9600);
  b.interval(50);
  pinMode(R_LED1, OUTPUT);
  pinMode(R_LED2, OUTPUT);
  pinMode(G_LED, OUTPUT);
  pinMode(SM_BTN, INPUT);
  pinMode(HT_BTN, INPUT);
  pinMode(LG_BTN, INPUT);
  pinMode(K_BTN, INPUT);
  alarmStart = millis() - alarmLength;
}



void loop() {
  b.update();


  if ( b.rose() ) {
    newPosition = myEnc.read();
    int minLevel = 0;
    int maxLevel = 255;
    if (newPosition <= minLevel) {
      myEnc.write(minLevel);
      newPosition = minLevel;
    } else if (newPosition >= maxLevel) {
      myEnc.write(maxLevel);
      newPosition = maxLevel;
    }
    if (newPosition > oldPosition) {
      newPosition = oldPosition + 5;
      Serial.println(newPosition);

    } else if (newPosition < oldPosition) {
      newPosition = oldPosition - 5;
      Serial.println(newPosition);
    }
    oldPosition = newPosition;
  }
  if (digitalRead(HT_BTN)) {
    digitalWrite(G_LED, HIGH);
  }
  if (!digitalRead(K_BTN)) {
    alarmStart = millis();
  }
  if (millis() - alarmStart < alarmLength) {
    delay(50);
    tone(SPKR, 1000);
    delay(50);
    tone(SPKR, 5000);
  }
  if (digitalRead(SM_BTN) || digitalRead(LG_BTN)) {
    tone(SPKR, 3500, beepLength);
    smBtnTime = millis();
  }
  else if (millis() - smBtnTime > beepLength) {
    noTone(SPKR);
  }
  analogWrite(R_LED1, newPosition);
  digitalWrite(G_LED, LOW);
  digitalWrite(R_LED2, HIGH);
}
