#define A 0
#define B 1
#define C 2
#define D 3
#define E 4
#define F 5
#define G 6
#define DP 7

const int pinSW = 2;
const int pinX = A0; 
const int pinY = A1;

const int pinA = 4;
const int pinB = 5;
const int pinC = 6;
const int pinD = 7;
const int pinE = 8;
const int pinF = 9;
const int pinG = 10;
const int pinDP = 11;
const int segSize = 8;

byte ledA = LOW;
byte ledB = LOW;
byte ledC = LOW;
byte ledD = LOW;
byte ledE = LOW;
byte ledF = LOW;
byte ledG = LOW;
byte ledDP = LOW;

int segments[segSize] = {
  pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP
};

int leds[segSize] = {
  ledA, ledB, ledC, ledD, ledE, ledF, ledG, ledDP
};


int currentPin = 11;
byte blinkLedState = LOW;
byte doNotBlink = LOW;


int xValue = 0;
int yValue = 0;
int minThreshold = 300;
int maxThreshold = 700;
byte joyBackToMiddleX = LOW;
byte joyBackToMiddleYstate1 = LOW;
byte joyBackToMiddleYstate2 = LOW;


byte swState = LOW;
byte lastSwState = HIGH;
byte switchState = HIGH;


const int debounceDelay = 50;
const int blinkInterval = 500;
const int longPressDuration = 2000;
const int shortPressDuration = 100;

int lastDebounceTime = 0;
int lastBlinkTime = 0;
int pressedTime;
int releasedTime;
int nrOfClicks = 0;

int state = 1;

int directions[8][4] = {
  { -1, G, F, B },
  { A, G, F, -1 },
  { G, D, E, DP },
  { G, -1, E, C },
  { G, D, -1, C },
  { A, G, -1, B },
  { A, D, -1, -1},
  { -1, -1, C, -1}
};

void setup() {
  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }
  pinMode(pinSW, INPUT_PULLUP);
  Serial.begin(9600);
}


void displayLeds() {
  for(int i = 0; i < segSize; i++)
    if(currentPin != segments[i])
      digitalWrite(segments[i], leds[i]);

  digitalWrite(currentPin, blinkLedState);
}


void resetLeds() {
  for(int i = 0; i < 8; i++)
    leds[i] = LOW;

  currentPin = pinDP;
}


void firstState() {
  //left
  if (yValue < minThreshold && joyBackToMiddleYstate1 == LOW) {
    if(directions[currentPin - 4][2] != -1)
      currentPin = segments[directions[currentPin - 4][2]];
    joyBackToMiddleYstate1 = HIGH;
  }
  //right
  else if (yValue > maxThreshold && joyBackToMiddleYstate1 == LOW) {
    if(directions[currentPin - 4][3] != -1)
      currentPin = segments[directions[currentPin - 4][3]];
    joyBackToMiddleYstate1 = HIGH; 
  }
  else if (joyBackToMiddleYstate1 == HIGH && yValue < maxThreshold && yValue > minThreshold) {
    joyBackToMiddleYstate1 = LOW;
  }

  //down
  if (xValue < minThreshold && joyBackToMiddleX == LOW) {
    if(directions[currentPin - 4][1] != -1)
      currentPin = segments[directions[currentPin - 4][1]];
    joyBackToMiddleX = HIGH;
  }
  else 
  //up
  if (xValue > maxThreshold && joyBackToMiddleX == LOW) {
    if(directions[currentPin - 4][0] != -1)
      currentPin = segments[directions[currentPin - 4][0]];
    joyBackToMiddleX = HIGH;
  }
  else if (joyBackToMiddleX == HIGH && xValue < maxThreshold && xValue > minThreshold) {
    joyBackToMiddleX = LOW;
  }
}


void secondState() {
  if (yValue < minThreshold && joyBackToMiddleYstate2 == LOW) {
    blinkLedState = LOW;
    for(int i = 0; i < segSize; i++)
      if(currentPin == segments[i])
      {
        leds[i] = LOW;
        break;
      }
    
    joyBackToMiddleYstate2 = HIGH;
  }
  else 
  if (yValue > maxThreshold && joyBackToMiddleYstate2 == LOW) {
    blinkLedState = HIGH;
    for(int i = 0; i < segSize; i++)
      if(currentPin == segments[i])
      {
        leds[i] = HIGH;
        break;
      }
    joyBackToMiddleYstate2 = HIGH; 
  }
  else if (joyBackToMiddleYstate2 == HIGH && yValue < maxThreshold && yValue > minThreshold) {
    joyBackToMiddleYstate2 = LOW;
  }
}


void buttonChangeState() {
  if (swState != lastSwState) 
    lastDebounceTime = millis();


  if ((millis() - lastDebounceTime) > debounceDelay) 
    if (swState != switchState) {
      switchState = swState;
      if (switchState == LOW) {
        pressedTime = millis();
      }
      else {
        releasedTime = millis();
      }
      long pressDuration = releasedTime - pressedTime;
      if (pressDuration > longPressDuration and state == 1) {
        resetLeds();
      }
      else if (pressDuration > shortPressDuration) {
        nrOfClicks ++;
        if (nrOfClicks % 2 != 0) {
          doNotBlink = HIGH;
          state = 2;
        }
        else {
          doNotBlink = LOW;
          state = 1;
        }
      }
    }
  
  lastSwState = swState;
}

void blink() {
  if (millis() - lastBlinkTime > blinkInterval && doNotBlink == LOW) {
    blinkLedState = !blinkLedState;
    lastBlinkTime = millis();
  }
}

void loop() {
  displayLeds();

  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  blink();

  if(state == 1)
    firstState();
  else
    if(state == 2)
      secondState();

  swState = digitalRead(pinSW);

  buttonChangeState(); 
}