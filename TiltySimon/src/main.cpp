#include <Arduino.h>
#include <LiquidCrystal.h>

const int rs = 12, en = 11, d4 = 8, d5 = 9, d6 = 10, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//change to side numbers or colors later
const int LEDpins[6] = {7,6,5,4,3,2};

const int photoSensorPin0 = A0;
const int photoSensorPin1 = A1;
const int photoSensorPin2 = A2;
const int photoSensorPin3 = A3;
const int photoSensorPin4 = A4;
const int photoSensorPin5 = A5;

int photoSensor0Value = 0;
int photoSensor1Value = 0;
int photoSensor2Value = 0;
int photoSensor3Value = 0;
int photoSensor4Value = 0;
int photoSensor5Value = 0;

//variable to final lowest light of photosensor
int photoSensors[6];

//game play variables
int level = 1;
int moveTime;
int movesWin;

//SETTING 4 GAME MOVES TO TEST
int gameMoves[4] = {1,2,3,4}; // to hold game moves 

int playerMoves[100] = {}; // to hold the player moves
int gameLen = 4; // this lets you know home many moves the player must make

boolean waiting = true;
boolean fail = false;
int score = 0;

int currentSideDown;
                  
// hold the next possible move for game, this array is needed as due to the
// hardware of the game we cannot make next move the side that is down (player would not see light
// also do not want to have up side to repeat move.. therefor only the 4 sides can be valid next movesp
int nextMovePossible[4] = {};

void showWelcome()
{
  lcd.clear();
  lcd.setCursor(0, 0);           
  lcd.print("Simon Gets Tilty");
  lcd.setCursor(0, 1);          
  lcd.print("Ready to Play");      
  delay(5000);
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
  Serial.print("Sensor 1: ");
  Serial.print(photoSensor0Value);
  Serial.print("\t Sensor 2: ");
  Serial.print(photoSensor1Value);
  Serial.print("\t Sensor 3: ");
  Serial.print(photoSensor2Value);
  Serial.print("\t Sensor 4: ");
  Serial.print(photoSensor3Value);
  Serial.print("\t Sensor 5: ");
  Serial.print(photoSensor4Value);
  Serial.print("\t Sensor 6: ");
  Serial.println(photoSensor5Value);
  return getSideDown(photoSensors);
  
}

int getSideDown(int photoSensors[])
{
  int count = sizeof(photoSensors) / sizeof(photoSensors[0]);
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
  Serial.print("Lowest Reading: ");
  Serial.print(photoSensors[index]);
  Serial.print(" at Pin: ");
  Serial.println(index);
  lightLED(index, 50);
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
    delay(1000); 
    waiting = false; // This ultimately will get us out of the waiting mode 
  }
}

int readValidPosition()
{
  int posArray[100]; // used to store position readings in a row. 
  int validReading = 0;
  int readingCount = 0;
  //require 50 conesecutive same readings to say it is a valid reading
  for(int j = 0;j<50;j++)
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
        j=50; // or start over
      }
    }
  }  
  // all 50 readins are the same so the current side down will change 
  if(validReading == 50){
    currentSideDown = posArray[0]; 
    return currentSideDown; 
  }
     
                                                             
}

void playerPattern()
{
  Serial.print("\n\r\n\rplayer pattern:");
  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("PLAYER PATTERN: ");
  int previousSide = readValidPosition();
  for(int i=0;i<gameLen;i++) 
  {
//    while(previousSide == readValidPosition())
//    {
//     int validSide = readValidPosition();
//     lcd.setCursor(i, 1); 
//     lcd.print(validSide );
//     lightLEDCorrect(validSide);
//    }
    
    int timeout = 0;
    int wrong = 0;
    //int validSide = readValidPosition();
    while(timeout<50)
    {
        
      int validSide = readValidPosition();
      
      // (1) Are they on the correct next position?
      if(validSide == gameMoves[i]) 
        {
          // Blink current position once to indicate they did it correctly
          lightLEDCorrect(validSide);
          lcd.setCursor(i, 1);          
          lcd.print(validSide);
          break;
        }
        
      // (2) how long have they been trying (or thinking), is it time to timeout?
      else if ((timeout == 49)) 
      {
        Serial.print("Timed out on position ");
        Serial.println(i);
        waiting = true;
        gameLen = 0; // this gets us out of listen_for_pattern()
        fail = 1;
        break;
      }
      
      // (3) Are they still on the previous position? 
      // If so, do nothing and increment timeout (happens below)
      else if(validSide == gameMoves[i-1]); 
      
      // (4) Did the player rotate to the wrong position? 
      //  If so, keep track of how long by incrementing "wrong_positions"
      else if(validSide != gameMoves[i]) wrong += 1; 
                                                                            
      // (5) Has the player stayed on a wrong position for 15 counts?
      if(wrong > 15) 
      {
        Serial.print("you tumbled wrong:");
        Serial.println(validSide);
        
        lcd.clear();
        lcd.setCursor(0, 0);           
        lcd.print("Wrong Side: ");
        lcd.setCursor(0, 1);          
        lcd.print(validSide);     
        // Blink quickly to show the incorrect tilt position
        flashAll(50);
        // Blink where the player should have rolled to
        delay(10);
        gameLen = 0; // this gets us out of listen_for_pattern()
        fail = 1;
        waiting = true;
        break;
      }
      timeout += 1;   // Increment timeout
   } // End of timeout while loop
  
   if((i == gameLen) && (fail == false))
   {
     Serial.println("current gamelength complete!");
     Serial.println(currentSideDown);
     
     delay(250);
     // Check to see if the player has reached "moves_to_win"
     if(gameLen == 4)
     { 
        lcd.clear();
        lcd.setCursor(0, 0);           
        lcd.print("WINNER: ");
        lcd.setCursor(0, 1);          
        lcd.print(currentSideDown);  
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
  Serial.print("\n\r\n\rgame pattern:");
  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("GAME PATTERN: ");
  
  
  for(int i=0;i<gameLen;i++) 
  {
    Serial.print(gameMoves[i]); // debug
    Serial.print(","); // debug
    lcd.setCursor(i, 1); 
    lcd.print(gameMoves[i]);
    
    lightLED(gameMoves[i], 500);
    delay(2000); // TODO THIS NEEDS TO BE A VARIABLE TO CHANGE WHEN DIFF INCREASES
  }
  
  // print a new line, this helps keep the debug serial window more legible
  Serial.println(" "); 
}


void setup() {
  // initialize low light vars
  //int LEDpins[6] = {LEDpin1, LEDpin2, LEDpin3, LEDpin4, LEDpin5, LEDpin6};


  // random seed with annalog read to make game pattern more random
  // https://www.arduino.cc/reference/en/language/functions/random-numbers/randomseed/
  randomSeed(analogRead(photoSensorPin0));
  
  lcd.begin(16,2);
  showWelcome();
  
  delay(5000);   // to show message on screen}
  
  Serial.begin(9600);

  for (int i = 0; i < 6; i++) 
  {
    pinMode(LEDpins[i], OUTPUT);
    digitalWrite(LEDpins[i], LOW);
  }  
}

void loop() {
  
  while(waiting){
    waitForMovement();
  }

  if(fail==1){
    lcd.clear();
    lcd.setCursor(0, 0);           
    lcd.print("You Lost!");
    lcd.setCursor(0, 1);          
    lcd.print("Starting over.");     
    fail = false; 
  }
  //gameLen++;
  showGamePattern();
  startScreen();
  playerPattern();
}