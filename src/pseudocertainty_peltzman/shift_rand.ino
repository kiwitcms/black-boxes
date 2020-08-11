int LATCHPIN = 5;
int CLOCKPIN = 6;
int DATAPIN = 4;

byte leds = 0;

void setup()
{
  pinMode(LATCHPIN, OUTPUT);
  pinMode(DATAPIN, OUTPUT);
  pinMode(CLOCKPIN, OUTPUT);
}


void loop()
{
  leds = 0;
  update();
  delay(500);
  for (int i = 0; i < 8; i++)
  {
    bitSet(leds, i);
    update();
    delay(500);
  }
}

void update()
{
  analogWrite(DATAPIN, random(0, 255));
  digitalWrite(LATCHPIN, LOW);
  shiftOut(DATAPIN, CLOCKPIN, LSBFIRST, leds);
  digitalWrite(LATCHPIN, HIGH);
}