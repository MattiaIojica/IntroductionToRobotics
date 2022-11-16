#include <EEPROM.h>


const int dataPin = 12;   //DS
const int latchPin = 11;  // STCP
const int clockPin = 10;  //SHCP

const int segD1 = 7;
const int segD2 = 6;
const int segD3 = 5;
const int segD4 = 4;

const int displayCount = 4;

int displayDigits[displayCount] = {
  segD1, segD2, segD3, segD4
};

const int encodingsNumber = 16;

int byteEncodings[encodingsNumber] = {
  //A B C D E F G DP 
  B11111100,  // 0
  B01100000,  // 1
  B11011010,  // 2
  B11110010,  // 3
  B01100110,  // 4
  B10110110,  // 5
  B10111110,  // 6
  B11100000,  // 7
  B11111110,  // 8
  B11110110,  // 9
  B11101110,  // A
  B00111110,  // B
  B10011100,  // C
  B01111010,  // D
  B10011110,  // E
  B10001110   // F
};

int digits[displayCount] = {
  0, 0, 0, 0 
};

const int firstBlinkInterval = 400;
const int secondBlinkInterval = 800;

const int pinSW = 3;
const int pinX = A4;
const int pinY = A5;

bool swState = LOW;
bool lastSwState = LOW;
byte switchState = HIGH;

int xValue = 0;
int yValue = 0;

bool joyMovedX = false;
bool joyMovedY = false;

int minThreshold = 250;
int maxThreshold = 750;

bool dpState = false;
bool selected = false;
int displayNo = 0;
unsigned long dpBlink = 0;

unsigned long lastDebounceTime = 0;
unsigned int debounceDelay = 50;
unsigned long pressedTime = 0;
unsigned long releasedTime = 0;
const int longPressDuration = 1000;
const int shortPressDuration = 50;

void setup() {
  pinMode(dataPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);

  for (int i = 0; i < displayCount; i++) {
    pinMode(displayDigits[i], OUTPUT);
    digitalWrite(displayDigits[i], LOW);
  }

  pinMode(pinSW, INPUT_PULLUP);

  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);

  readEEPROM();

  Serial.begin(9600);
}

void loop() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  if(!selected)
    state1();
  else
    state2();

  swState = digitalRead(pinSW);
  buttonChangeState();
  
  writeNumber(displayNo, selected);
}

// function that moves the current digit according to the 
// movement of the joystick on the Y axis
void state1() {
  if (yValue < minThreshold && !joyMovedY) {
    if (displayNo > 0) 
      displayNo--;
    
    joyMovedY = true;
  }
  else
  if (yValue > maxThreshold && !joyMovedY) {
    if (displayNo < 3) 
      displayNo++;
    
    joyMovedY = true;
  }
  else
  if (yValue >= minThreshold && yValue <= maxThreshold) {
    joyMovedY = false;
  }

}

void state2()
{
  if (xValue > maxThreshold && !joyMovedX) {
    digits[displayNo] = (digits[displayNo] == 0? encodingsNumber : digits[displayNo]) - 1;

    joyMovedX = true;
  }
  else
    if (xValue < minThreshold && !joyMovedX) {
        digits[displayNo] = (digits[displayNo] + 1) % encodingsNumber;
      
      joyMovedX = true;
    }
  else
    if (xValue >= minThreshold && xValue <= maxThreshold) {
      joyMovedX = false;
    }
}


// function that resets the display
void resetLeds() {
  for(int i = 0; i < displayCount; i++)
    digits[i] = 0;

  displayNo = 0;
}


void readEEPROM () {
  for (int i = 0; i < displayCount; ++i) 
    digits[i] = EEPROM.read(i);
}

void showDigit(int displayNo) {
  for (int i = 0; i < displayCount; i++) 
    digitalWrite(displayDigits[i], HIGH);
  
  digitalWrite(displayDigits[displayNo], LOW);
}

void writeReg(int digit) {
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, digit);
  digitalWrite(latchPin, HIGH);
}

void writeNumber(int displayNo, int selected) {
  for (int i = 0; i < displayCount; i++) {
    int digitToWrite = 0;
    if (i == displayNo) {
      if (selected) {
        digitToWrite = byteEncodings[digits[i]] + 1;
      }
      else {

        digitToWrite = byteEncodings[digits[i]];

        if (millis() - dpBlink > firstBlinkInterval) 
          digitToWrite = byteEncodings[digits[i]] + 1;
        
        if (millis() - dpBlink > secondBlinkInterval) {
          digitToWrite = byteEncodings[digits[i]];
          dpBlink = millis();
        }

      }
    }
    else {
      digitToWrite = byteEncodings[digits[i]];
    }


    writeReg(digitToWrite);
    showDigit(i);
    EEPROM.update(i, digits[i]);
    
    delay(5);
  }
}



void buttonChangeState() {
  if (swState != lastSwState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (swState != switchState) {
      switchState = swState;
      if (switchState == LOW) {
        pressedTime = millis();
      }
      else {
        releasedTime = millis();
      }
      long pressDuration = releasedTime - pressedTime;
      if (pressDuration > longPressDuration && selected == 0) {
        resetLeds();
      }
      else if (pressDuration > shortPressDuration) {
        selected = !selected;
      }
    }
  }
  
  lastSwState = swState;
}
