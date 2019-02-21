#include <LiquidCrystal.h>
#include <SPI.h>
#include <SoftwareSerial.h>// import the serial library
#include <Wire.h>//I2C library

//bluetooth variables
SoftwareSerial ser(0, 1); // RX, TX
int BluetoothData; // the data given from Computer

//LCD variables
const int rs = 12, en = 11, d4 = 8, d5 = 9, d6 = 10, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//change to side numbers or colors later
const int LEDpins[6] = {7,6,5,4,3,2};

// sensor pins
const int photoSensorPin0 = A0;
const int photoSensorPin1 = A1;
const int photoSensorPin2 = A2;
const int photoSensorPin3 = A3;
const int photoSensorPin4 = A4;
const int photoSensorPin5 = A5;

// sensor values
int photoSensor0Value = 0;
int photoSensor1Value = 0;
int photoSensor2Value = 0;
int photoSensor3Value = 0;
int photoSensor4Value = 0;
int photoSensor5Value = 0;
int photoSensors[6];



//move variables
int currentSideDown;
int sidePossible[4] = {};
int gameMoves[23] = {}; // to hold game moves 
int playerMoves[100] = {}; // to hold the player moves

//game variables
int level = 1; //starting level, will increment with each success round
int moveTime;
int winMovesCount;
int gameLen = 0; // this lets you know home many moves the player must make, increment on loop

//game states
boolean waiting = true;
boolean fail = false;
boolean highScore = 0;
int score = 0;
int needToBeValid = 50;
                  
// hold the next possible move for game, this array is needed as due to the
// hardware of the game we cannot make next move the side that is down (player would not see light
// also do not want to have up side to repeat move.. therefor only the 4 sides can be valid next movesp
void setup() {
  
  // random seed with annalog read to make game pattern more random
  // https://www.arduino.cc/reference/en/language/functions/random-numbers/randomseed/
  randomSeed(analogRead(photoSensorPin0));
  //Wire.begin();
  ser.begin(57600);
  lcd.begin(16,2);
  
  for (int i = 0; i < 6; i++) 
  {
    pinMode(LEDpins[i], OUTPUT);
    digitalWrite(LEDpins[i], LOW);
  }  
  showWelcome();  
}

void loop() {
  
  while(waiting){
    waitForMovement();
  }

  setGameStates(level);

  if(fail == true ){
    lcd.clear();
    lcd.setCursor(0, 0);           
    lcd.print("You Lost!");
    lcd.setCursor(0, 1);          
    lcd.print("Starting over.");     
    fail = false; 
  }
  gameLen++;
  gameMoves[gameLen] = random(0,6); //and rondom 0-6 
  if(gameMoves[gameLen] == gameMoves[gameLen-1])
   {
     //no duplicate moves
     while(gameMoves[gameLen] == gameMoves[gameLen-1]) 
     {
       gameMoves[gameLen]= random(0,6); 
     }
   }

   // use this to restart game
  if (gameMoves == 0)
  {
    gameMoves == 1;
  }
  showGamePattern();
  playerPattern();
}

void setGameStates(int level){
  if(level == 1)
  {
    winMovesCount = 5;
    moveTime = 400;
  }
  else if(level == 2)
  {
    winMovesCount = 7;
    moveTime = 100;
  }
  else if(level >= 3) // if play gets past 3 there is an incremental increase in dificult to the point that game is unwinnable
  //or an game move possition array breaks.
  {
     winMovesCount += 2;
     moveTime = 50;
  }
}

void showWelcome()
{
  lcd.clear();
  lcd.setCursor(0, 0);           
  lcd.print("Simon Gets Tilty");
  lcd.setCursor(0, 1);          
  lcd.print("Ready to Play");      
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);           
  lcd.print("Tilt another");
  lcd.setCursor(0, 1);          
  lcd.print("side up to start");
}

void startScreen()
{
  lcd.clear();
  lcd.setCursor(0, 0);           
  lcd.print("Ready, GO!");
  delay(2500);
}

int readSensors(){

  int photoSensors[6] = {photoSensor0Value, photoSensor1Value, photoSensor2Value, photoSensor3Value, photoSensor4Value, photoSensor5Value};

  photoSensor0Value = analogRead(photoSensorPin0);
  delay(5);
  photoSensor1Value = analogRead(photoSensorPin1);
  delay(5);
  photoSensor2Value = analogRead(photoSensorPin2);
  delay(5);
  photoSensor3Value = analogRead(photoSensorPin3);
  delay(5);
  photoSensor4Value = analogRead(photoSensorPin4);
  delay(5);
  photoSensor5Value = analogRead(photoSensorPin5);

  //Serial.print(photoSensor0Value);
//  Serial.print("Sensor 1: ");
//  Serial.print(photoSensor0Value);
//  Serial.print("\t Sensor 2: ");
//  Serial.print(photoSensor1Value);
//  Serial.print("\t Sensor 3: ");
//  Serial.print(photoSensor2Value);
//  Serial.print("\t Sensor 4: ");
//  Serial.print(photoSensor3Value);
//  Serial.print("\t Sensor 5: ");
//  Serial.print(photoSensor4Value);
//  Serial.print("\t Sensor 6: ");
//  Serial.println(photoSensor5Value);
  return getSideDown(photoSensors);
  
}

int getSideDown(int photoSensors[])
{
  int index = 0 ;
  int currentPhotoSensorValue;
  int prevPhotoSensorValue;

  for (int i = 0; i < 6; i++) 
  {
    currentPhotoSensorValue = photoSensors[i];
    prevPhotoSensorValue = photoSensors[index];
    if (currentPhotoSensorValue < prevPhotoSensorValue) {
      index = i;
    }
  }
//  Serial.print("Lowest Reading: ");
//  Serial.print(photoSensors[index]);
//  Serial.print(" at Pin: ");
//  Serial.println(index);
  lightLED(index, 70);
  return index;
}

void lightLED(int pin, int delayInterval){
  //int LEDpins[6] = {LEDpin1, LEDpin2, LEDpin3, LEDpin4, LEDpin5, LEDpin6};
  for (int i = 0; i < 6; i++) 
  {
    if (i == pin){
      //Serial.print("Turning Light On: ");
      //Serial.println(LEDpins[i]);
      digitalWrite(LEDpins[i], HIGH);
      delay(delayInterval);
      digitalWrite(LEDpins[i], LOW);
    }
    else{
      digitalWrite(LEDpins[i], LOW);
    }
  } 
}

void lightLEDCorrect(int pin){
  //int LEDpins[6] = {LEDpin1, LEDpin2, LEDpin3, LEDpin4, LEDpin5, LEDpin6};
  for (int i = 0; i < 6; i++) 
  {
    if (i == pin){
      //Serial.print("Turning Light On: ");
      //Serial.println(LEDpins[i]);
      digitalWrite(LEDpins[i], HIGH);
      delay(1000);
      digitalWrite(LEDpins[i], LOW);
      delay(500);
      digitalWrite(LEDpins[i], HIGH);
      delay(1000);
      digitalWrite(LEDpins[i], LOW);
      delay(500);
    }
    else{
      digitalWrite(LEDpins[i], LOW);
    }
  } 
}


void flashAll(int delay_time)
{
  for(int i = 0;i<6;i++)
  { 
    digitalWrite(LEDpins[i], HIGH);
    delay(delay_time);
    digitalWrite(LEDpins[i], LOW);
  }
}

void waitForMovement(void)
{ 
  // store the current position so we can compare it later and notice a change.
  int prevSideDown = readValidPosition(); 
  flashAll(50);
  if(prevSideDown != readValidPosition()) 
  {
    delay(500); 
    waiting = false; // get out of the waiting mode 
  }
}

int readValidPosition()
{
  int posArray[100]; // used to store position readings in a row. 
  int validReading = 0;
  int readingCount = 0;
  //require 50 conesecutive same readings to say it is a valid reading
  for(int j = 0;j<needToBeValid;j++)
  {
    int firstRead = readSensors(); 
    for(int i = 0;i<6;i++) 
    {
      if(firstRead == i) 
      {
        posArray[j] = i; 
        ///Serial.print(pos_array[j]);
        break;
      }
    }
    if(j == 0) validReading += 1; // the first reading is alway correct
    else{
      // check new reading against previous, if it's the same, increment valid
      if(posArray[j] == posArray[j-1])
      {
        validReading += 1; 
      }
      else{
        delay(100);
        j=needToBeValid; // or start over
      }
    }
  }  
  // all 50 readins are the same so the current side down will change 
  if(validReading == needToBeValid){
    currentSideDown = posArray[0]; 
    ser.print("Side down: ");
    ser.println(currentSideDown); 
    return currentSideDown; 
  }
     
                                                             
}

void playerPattern()
{
  ser.print("player pattern:");
  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("PLAYER PATTERN: ");
  int previousSide = readValidPosition();
  for(int i=1; i<=gameLen;i++) 
  {    
    int timeout = 0;
    int wrong = 0;
    //int validSide = readValidPosition();
    while(timeout<5)
    {
      // get side down
      int validSide = readValidPosition();
      
      // correct move?
      if(validSide == gameMoves[i]) 
        {
          // Blink current position once to indicate they did it correctly
          lightLEDCorrect(validSide);
          lcd.setCursor(i, 1);          
          lcd.print(validSide);
          ser.print(validSide);
          break;
        }
        
      //how long have they been trying (or thinking), is it time to timeout?
      else if ((timeout == 4)) 
      {
        //Serial.print("Timed out on position ");
        //Serial.println(i);
        waiting = true;
        gameLen = 0; // this gets us out of listen_for_pattern()
        level = 1;
        fail = true;
        break;
      }
       
      //did not move side yet
      else if(validSide == gameMoves[i-1]); 
      
      //wrong move
      else if(validSide != gameMoves[i]) 
      {
        wrong += 1; 
      }
                                                                            
      // on wrong for too long
      if(wrong > 5) 
      {
        ser.print("wrong: ");
        ser.println(validSide);
        
        lcd.clear();
        lcd.setCursor(0, 0);           
        lcd.print("Wrong Move: ");
        lcd.setCursor(0, 1);          
        lcd.print(validSide);     
        // Blink quickly to show the incorrect tilt position
        flashAll(50);
        // Blink where the player should have rolled to
        delay(10);
        
        gameLen = 0; //end game
        level = 1; 
        fail = true;
        waiting = true;
        break;
      }
      timeout += 1;   // Increment timeout
   } // End of timeout while loop
  
   if((i == gameLen) && (fail == false))
   {
     ser.println("current gamelength complete!");
     ser.println(currentSideDown);
     
     delay(250);
     // Check to see if the player has reached "moves_to_win"
     if(gameLen == winMovesCount)
     { 
        lcd.clear();
        lcd.setCursor(0, 0);           
        lcd.print("Level Complete");
        lcd.setCursor(0, 1);          
        lcd.print("Next Level: ");  
        lcd.print(level += 1);  
        
       // you've won!!
       gameLen = 0; // this gets us out of listen_for_pattern()
       fail = 1;
       level += 1; // go to next level
     }
     else{
       delay(1000); // delay to give the player time to be ready for playback
     }
    }
  } // end of game pattern for loop
}  

void showGamePattern()
{
  ser.print("game pattern:");
  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("GAME PATTERN: ");
  
  
  for(int i=1;i<=gameLen;i++) 
  {
    //Serial.print(gameMoves[i]); // debug
    //Serial.print(","); // debug
    lcd.setCursor(i-1, 1); 
    lcd.print(gameMoves[i]);
    ser.print(gameMoves[i]);
    ser.print(",");
    
    lightLED(gameMoves[i], 500);
    delay(2000); // TODO THIS NEEDS TO BE A VARIABLE TO CHANGE WHEN DIFF INCREASES
  }

  startScreen();
  
  // print a new line, this helps keep the debug serial window more legible
  ser.println(" "); 
}

//BLUETOOTH COMS
//void writeTo(byte device, byte toAddress, byte val) {
//  Wire.beginTransmission(device);
//  Wire.write(toAddress);
//  Wire.write(val);
//  Wire.endTransmission();
//}
//
//void readFrom(byte device, byte fromAddress, int num, byte result[]) {
//  Wire.beginTransmission(device);
//  Wire.write(fromAddress);
//  Wire.endTransmission();
//  Wire.requestFrom((int)device, num);
//  int i = 0;
//  while (Wire.available()) {
//    result[i] = Wire.read();
//    i++;
//  }
//}
