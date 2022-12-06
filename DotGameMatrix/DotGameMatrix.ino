#include "LedControl.h" // need the library
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;

const int xPin = A4;
const int yPin = A5;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); // DIN, CLK,LOAD, No. DRIVER

byte matrixBrightness = 2;

byte xPos = 0;
byte yPos = 0;
byte xLastPos = 0;
byte yLastPos = 0;
byte lastFoodPosX;
byte lastFoodPosY;
byte foodPosX = 4;
byte foodPosY = 6;

const int minThreshold = 200;
const int maxThreshold = 600;

const byte moveInterval = 100;
unsigned long long lastMoved = 0;

const byte matrixSize = 8;
bool matrixChanged = true;


const int blinkInterval = 250;
unsigned long lastBlink;
int score = 0;


byte matrix[matrixSize][matrixSize] = {
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0}
};


void setup() {
  Serial.begin(9600);

  // the zero refers to the MAX7219 number, it is zero for 1 chip
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixBrightness); // sets brightness (0~15 possible values)
  lc.clearDisplay(0);// clear screen

  matrix[xPos][yPos] = 1;
}


void loop() {
  // updateByteMatrix();
  if (millis() - lastMoved > moveInterval) {
  // game logic
  updatePositions();
  lastMoved = millis();
  }

  if (millis() - lastBlink > blinkInterval) {
    foodBlink();
    lastBlink = millis();
  }

  //check if the player is on the food position
  if (foodPosX == xPos && foodPosY == yPos) {
    generateFood();
    score -= -(!0);
    Serial.println(score);
  }

  if (matrixChanged == true) {
  // matrix display logi
  updateMatrix();
  matrixChanged = false;
  }
}

void foodBlink(){
  matrix[foodPosX][foodPosY] = !matrix[foodPosX][foodPosY];
  matrixChanged = true;
}

void generateFood() {
  matrix[foodPosX][foodPosY] = 0;

  lastFoodPosX = foodPosX;
  lastFoodPosY = foodPosY;

  foodPosX = rand() % matrixSize; 
  foodPosY = rand() % matrixSize; 

  //the new food is generated on the same position
  while (foodPosX == xPos && foodPosY == yPos){
    foodPosX = rand() % matrixSize; 
    foodPosY = rand() % matrixSize; 
  }

  matrix[foodPosX][foodPosY] = 1;
  matrix[xPos][yPos] = 1;


  matrixChanged = true;
}


void updateMatrix() {
  for (int row = 0; row < matrixSize; row++) {
    for (int col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}


void updatePositions() {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);
  xLastPos = xPos;
  yLastPos = yPos;
  if (xValue < minThreshold) {
    if (xPos < matrixSize - 1) {
      xPos++;
    }
    else {
      xPos = 0;
    }
  }

  if (xValue > maxThreshold) {
    if (xPos > 0) {
      xPos--;
    }
    else {
      xPos = matrixSize - 1;
    }
  }
  if (yValue > maxThreshold) {
    if (yPos < matrixSize - 1) {
      yPos++;
    }
    else {
      yPos = 0;
    }
  }

  if (yValue < minThreshold) {
    if (yPos > 0) {
      yPos--;
    }
    else {
      yPos = matrixSize - 1;
    }
  }

  if (xPos != xLastPos || yPos != yLastPos) {
    matrixChanged = true;
    matrix[xLastPos][yLastPos] = 0;
    matrix[xPos][yPos] = 1;
  }
}

