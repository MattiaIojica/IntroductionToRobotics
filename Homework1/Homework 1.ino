// Analog-In 
const int redPotPin = A2;
const int greenPotPin = A1;
const int bluePotPin = A0;

// Analog-Out
const int redPin = 3;
const int greenPin = 5;
const int bluePin = 6;

// Bounds
const int potLowerBound = 0,
const int potUpperBound = 1023,
const int ledLowerBound = 0,
const int ledUpperBound = 255;
         
int redPotValue = 0;
int greenPotValue = 0;
int bluePotValue = 0;
    
int redPinValue = 0;
int greenPinValue = 0;
int bluePinValue = 0;

void setup() {
  pinMode(redPotPin, INPUT);
  pinMode(greenPotPin, INPUT);
  pinMode(bluePotPin, INPUT);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  // Read potentiometer value
  // get potentiometer data
  redPotValue = analogRead(redPotPin);
  greenPotValue = analogRead(greenPotPin);
  bluePotValue = analogRead(bluePotPin);

  // return the equivalent into the led interval
  redPinValue = map(redPotValue, potLowerBound, potUpperBound, ledLowerBound, ledUpperBound);
  greenPinValue = map(greenPotValue, potLowerBound, potUpperBound, ledLowerBound, ledUpperBound);
  bluePinValue = map(bluePotValue, potLowerBound, potUpperBound, ledLowerBound, ledUpperBound);

  // Write the values into the leds  
  analogWrite(redPin, redPinValue);
  analogWrite(greenPin, greenPinValue);
  analogWrite(bluePin, bluePinValue);

  delay(1);
}