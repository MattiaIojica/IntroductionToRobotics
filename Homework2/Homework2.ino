#define STATE_CARS_CROSSING 0
#define STATE_YELLOW_LIGHT 1
#define STATE_PEDESTRIANS_CROSSING 2
#define STATE_PEDESTRIANS_BLINKING 3

#define STATE_CARS_CROSSING_DURATION 8000
#define STATE_YELLOW_LIGHT_DURATION 3000
#define STATE_PEDESTRIANS_CROSSING_DURATION 6000
#define STATE_PEDESTRIANS_BLINKING_DURATION 4000

#define STATE_BUTTON_PRESSED 1
#define STATE_BUTTON_RELEASED 0



const int buzzerPin = 3;
const int buttonPin = 4;

const int carLedGreen = 5;
const int carLedYellow = 6;
const int carLedRed = 7;

const int pedestrianLedGreen = 8;
const int pedestrianLedRed = 9;

byte buttonState = LOW;
byte lastButtonState = LOW;
byte buzzerState = LOW;
byte pedestrianGreenLedState = LOW;

int pedestrianLedBlinkingInterval = 400;
int lastPedestrianLedChangeTime = 0;
int lastBuzzerChangeTime = 0;

int currentTime = 0;
int lastChangeTime = -1;

int buzzerCrossingTone = 500;
int buzzerBlinkingTone = 700;

int buzzerCrossingInterval = 800;
int buzzerBlinkingInterval = 400;

unsigned int lastDebounceTime = 0;
unsigned int debounceDelay = 50;


int systemState = 0;


void setup() 
{
  // put your setup code here, to run once:
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  pinMode(carLedRed, OUTPUT);
  pinMode(carLedYellow, OUTPUT);
  pinMode(carLedGreen, OUTPUT);

  pinMode(pedestrianLedRed, OUTPUT);
  pinMode(pedestrianLedGreen, OUTPUT);

  changeState(STATE_CARS_CROSSING);
  Serial.begin(9600);
}

int getButtonState()
{
  int reading = digitalRead(buttonPin);
  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if (millis() - lastDebounceTime > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;
      if (buttonState == LOW) {
        return STATE_BUTTON_PRESSED;
      }
    }
  }

  lastButtonState = reading;
  return STATE_BUTTON_RELEASED;
}

void setLedValues(byte carRedValue, byte carYellowValue, byte carGreenValue, byte pedestrianRedValue, byte pedestrianGreenValue) 
{

  // cars
  digitalWrite(carLedRed, carRedValue);
  digitalWrite(carLedYellow, carYellowValue);
  digitalWrite(carLedGreen, carGreenValue);

  // pedestrians
  digitalWrite(pedestrianLedRed, pedestrianRedValue);
  digitalWrite(pedestrianLedGreen, pedestrianGreenValue);
}

void changeState(int state) 
{
  Serial.println(state);
  systemState = state;

  switch (systemState) {

    case STATE_CARS_CROSSING:
      // resets from start
      lastChangeTime = -1;
      resetBuzzer();
      setLedValues(LOW, LOW, HIGH, HIGH, LOW);
      break;
  
    case STATE_YELLOW_LIGHT:
      lastChangeTime = millis();
      setLedValues(LOW, HIGH, LOW, HIGH, LOW);
      break;
  
    case STATE_PEDESTRIANS_CROSSING:
      lastChangeTime = millis();
      resetBuzzer();
      setLedValues(HIGH, LOW, LOW, LOW, HIGH);
      break;
  
    case STATE_PEDESTRIANS_BLINKING:
      lastChangeTime = millis();
      resetBuzzer();
      setLedValues(HIGH, LOW, LOW, LOW, HIGH);
      break;
  
    default:
      break;
  }
}

void playBuzzer(int buzzerTone) 
{
  if (buzzerState == HIGH) {
    tone(buzzerPin, buzzerTone);
  } else {
    noTone(buzzerPin);
  }
}

void resetBuzzer() 
{
  noTone(buzzerPin);
  buzzerState = LOW;
  lastBuzzerChangeTime = 0;
}

void setBuzzerState(int buzzerTone, int buzzerInterval) 
{
  if (currentTime - lastBuzzerChangeTime > buzzerInterval) {
    lastBuzzerChangeTime = currentTime;
    buzzerState = !buzzerState;
  }
  playBuzzer(buzzerTone);
}


void setGreenBlinkingLedState() 
{
  if (currentTime - lastPedestrianLedChangeTime > pedestrianLedBlinkingInterval) {
    lastPedestrianLedChangeTime = currentTime;
    pedestrianGreenLedState = !pedestrianGreenLedState;
  }

  setLedValues(HIGH, LOW, LOW, LOW, pedestrianGreenLedState);
}

// depending on state, do other required actions
void doStateAction() 
{
  switch (systemState) {
    
    // buzzes
    case STATE_PEDESTRIANS_CROSSING:
      setBuzzerState(buzzerCrossingTone, buzzerCrossingInterval);
      break;
  
    // blinks and buzzes more quickly
    case STATE_PEDESTRIANS_BLINKING:
      setBuzzerState(buzzerBlinkingTone, buzzerBlinkingInterval);
      setGreenBlinkingLedState();
      break;

    default:
      break;
  }
}

// figure out and set current system state
void determineState()
{
  int buttonValue = getButtonState();
  currentTime = millis();

  // if idling and button pressed, save the millis as the starting point into the system
  if (systemState == STATE_CARS_CROSSING && lastChangeTime == -1 && buttonValue == STATE_BUTTON_PRESSED) {
    lastChangeTime = millis();
  }


  if (lastChangeTime != -1) 
  {

    if (systemState == STATE_CARS_CROSSING && currentTime - lastChangeTime > STATE_CARS_CROSSING_DURATION) 
      changeState(STATE_YELLOW_LIGHT);
    

    if (systemState == STATE_YELLOW_LIGHT && currentTime - lastChangeTime > STATE_YELLOW_LIGHT_DURATION) 
      changeState(STATE_PEDESTRIANS_CROSSING);
    

    if (systemState == STATE_PEDESTRIANS_CROSSING && currentTime - lastChangeTime > STATE_PEDESTRIANS_CROSSING_DURATION) 
      changeState(STATE_PEDESTRIANS_BLINKING);
    

    if (systemState == STATE_PEDESTRIANS_BLINKING && currentTime - lastChangeTime > STATE_PEDESTRIANS_BLINKING_DURATION) 
      changeState(STATE_CARS_CROSSING);
  }
}

void loop()
{
  determineState();
  doStateAction();
}
