#include <LiquidCrystal.h>
#include "EEPROMMemory.h"
#include "dotGame.h"

const byte RS = 9;
const byte enable = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;

const int pinX = A4;
const int pinY = A5;
const int pinSw = 0;

const String menu[5] = { "START GAME", "HIGHSCORE", "SETTINGS", "ABOUT", "HOW TO PLAY" };
int menuPosition = 0;

int xValue = 0;
int yValue = 0;
const int minThreshold = 400;
const int maxThreshold = 800;
bool joyMovedY = false;

bool gameState = 0;

int textPosition = 0;
int lastTextPosition = -1;

int lastScore = -1;

int lastDifficulty = -1;


bool swState = LOW;
bool lastSwState = LOW;
byte switchState = HIGH;

const int shortPressDuration = 50;
const int longPressDuration = 1500;
unsigned long lastDebounceTime = 0;
unsigned int debounceDelay = 50;
unsigned long pressedTime = 0;
unsigned long releasedTime = 0;
bool displayMenuOnce = false;

int topPlayer = 0;
int lastTopPlayer = -1;

LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);
void setup() {
  pinMode(pinSw, INPUT_PULLUP);
  pinMode(pinX, INPUT);
  pinMode(pinY, INPUT);


  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  updateLCD();

  Serial.begin(9600);
}


void loop() {

  swState = digitalRead(pinSw);
  buttonChangeState();

  if(gameState == 0)
    menuMovement();

  game();

}

void game(){
  if(gameState == 0) //main menu state
    menuMovement();

  if(gameState == 1)
  {
    checkMenuPosition();
    if(menuPosition)
      menuMovement();
  }
}

void menuMovement(){
  yValue = analogRead(pinY);
  // Serial.println(gameState);
  if (yValue < minThreshold && !joyMovedY) {
      Serial.println("down");
      Serial.println(gameState);
    if(gameState == 0)
    {
      menuPosition = min(menuPosition + 1, 4);
    }
    else
    if(gameState == 1)
    {
      textPosition = min(textPosition + 1, 1);
      topPlayer = min(topPlayer + 1, 4);
      updateDifficulty(max(EEPROM.read(5) - 1, 1));
    }

    joyMovedY = true;
    if(gameState == 0)
      updateLCD();
  }
  else
  if (yValue > maxThreshold && !joyMovedY) {
    if(gameState == 0)
      menuPosition = max(menuPosition - 1, 0);
    else
    if(gameState == 1)
    {
      textPosition = max(textPosition - 1, 0);
      topPlayer = max(topPlayer - 1, 0);
      updateDifficulty(min(EEPROM.read(5) + 1, 3));
    }
    
    joyMovedY = true;
    if(gameState == 0)
      updateLCD();
  }
  else
  if (yValue >= minThreshold && yValue <= maxThreshold) {
    joyMovedY = false;
  }
}

void updateLCD(){
  lcd.clear();

  // Serial.println(menuPosition);

  if(menuPosition == 0)
  {
      lcd.setCursor(6, 0);
      lcd.print("Menu");

      lcd.setCursor(0, 1);
      lcd.print(">  ");
      lcd.print(menu[menuPosition]);
  }
  else
  if(menuPosition < 4)
  {
      lcd.setCursor(0, 0);
      lcd.print(">");

      lcd.setCursor((16 - menu[menuPosition].length()) / 2, 0);
      lcd.print(menu[menuPosition]);

      lcd.setCursor((16 - menu[menuPosition + 1].length()) / 2, 1);
      lcd.print(menu[menuPosition + 1]);
  }
  else
  {
      lcd.setCursor((16 - menu[menuPosition - 1].length()) / 2, 0);
      lcd.print(menu[menuPosition - 1]);

      lcd.setCursor(0, 1);
      lcd.print(">");
      lcd.setCursor((16 - menu[menuPosition].length()) / 2, 1);
      lcd.print(menu[menuPosition]);
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
      if (pressDuration > shortPressDuration) {

        if(pressDuration > longPressDuration)
        {
          Serial.println("long press");
          reset();
          return;
        }

        if(gameState == 0)
        {
          Serial.println("-------------");
          gameState = 1;
          return;
        }
        
        if(gameState == 1)
        {
          Serial.println(gameState);
          if(menuPosition == 0)
            if(gameOver == false)
              return;
            else
            {
              doOnce = false;
              gameOver = false;
              displayMenuOnce = false;
              checkMenuPosition();
            }

          reset();
        }

      }
    }
  }
  
  lastSwState = swState;
}

void reset(){
  doOnce = false;
  gameOver = false;
  displayMenuOnce = false;
  Serial.println("reset");
  gameState = 0;
  menuPosition = 0;
  resetMatrix();
  updateLCD();
  lastTopPlayer = -1;
  lastTextPosition = -1;
  topPlayer = 0;
  textPosition = 0;
  lastDifficulty = 0;
}

void checkMenuPosition()
{
  gameState = 1;
  switch(menuPosition)
  {
    case 0:
      startGame();
      break;

    case 1:
      highScoreMenu();
      break;

    case 2:
      settingsMenu();
      break;

    case 3:
      aboutMenu();
      break;

    case 4:
      howToPlayMenu();
      break;

    default:
      break;
  }
}

void aboutMenu(){
  if(lastTextPosition == textPosition)
    return;
  
  lcd.clear();

  if (textPosition == 0) {
    lcd.setCursor(6, 0);
    lcd.print("ABOUT");
    lcd.setCursor(0, 1);
    lcd.print("Name: Dot Game");
  }
  else
  if (textPosition == 1) {
    lcd.setCursor(5, 0);
    lcd.print("Creator:");
    lcd.setCursor(1, 1);
    lcd.print("Iojica Mattia");
  }
  lastTextPosition = textPosition;
}

void startGame(){
  if(gameOver == true)
  {
    if(!displayMenuOnce){
      GameOverMenu();
    }

    return;
  }

  gameLoop();

  lcd.setCursor(2, 1);
  lcd.print("TIME LEFT: ");
  lcd.print(6 - EEPROM.read(5) - (millis() - timeTillDeath) / 1000);

  if((millis() - timeTillDeath) / 1000 > 6 - EEPROM.read(5))
  {
    gameOver = true;
    return;
  }

  if(lastScore != score)
  {
    lcd.clear();
    lcd.setCursor(5, 0);
    lcd.print("SCORE:");
    lcd.print(score);
    
    lastScore = score;
  }
}


void settingsMenu(){
  if(lastDifficulty == EEPROM.read(5))
    return;

  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("Difficulty: ");

  lcd.print(EEPROM.read(5));
  lastDifficulty = EEPROM.read(5);
}

void highScoreMenu(){

  if(lastTopPlayer == topPlayer)
    return;

  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Name: ");

  lcd.setCursor(3, 1);
  lcd.print("Score: ");
  lcd.print(EEPROM.read(topPlayer));
  lastTopPlayer = topPlayer;
}


int getNumberOfDigits(int x)
{
  int nr = 0;

  while(x != 0)
  {
    nr++;
    x /= 10;
  }

  return nr;
}

void GameOverMenu(){

  updateHighScore(score);
  

  displayMenuOnce = true;
  lc.clearDisplay(0);
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("GAME OVER");
  lcd.setCursor(4, 1);
  lcd.print("SCORE:");
  lcd.print(score);
}

void howToPlayMenu(){
  if(displayMenuOnce)
    return;
  
  lcd.clear();
  lcd.setCursor(4, 0);
  lcd.print("EAT THE");
  lcd.setCursor(2, 1);
  lcd.print("BLINKING DOT");


  displayMenuOnce = true;
}
