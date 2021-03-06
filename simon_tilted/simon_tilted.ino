#include <LiquidCrystal.h>
#include <SPI.h>
#include <SoftwareSerial.h>// import the serial library
#include "pitches.h"

//bluetooth variables
SoftwareSerial ser(0, 1); // RX, TX
int BluetoothData; // the data given from Computer

//shift reg variables
//Pin connected to ST_CP of 74HC595
int latchPin = 4;
//Pin connected to SH_CP of 74HC595
int clockPin = 5;
////Pin connected to DS of 74HC595 
int dataPin = 3;

//LCD variables
const int rs = 12, en = 11, d4 = 8, d5 = 9, d6 = 10, d7 = 13;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// array to hold LEDpins
const int LEDpins[6] = {};

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

//tone varialbles
//https://www.arduino.cc/en/Tutorial/ToneMelody?from=Tutorial.Tone
const int tonePin = 6;
int melody[] = {NOTE_C8, NOTE_E6, NOTE_F6, NOTE_G6, NOTE_A6, NOTE_B4, NOTE_C5, NOTE_D5};


//move variables
int currentSideDown;
int sidePossible[4] = {};
int gameMoves[50] = {}; // to hold game moves 
int playerMoves[100] = {}; // to hold the player moves
int moveTime; // time the player has to move
int winMovesCount;
int gameDelay;
int gameLen = 0; // this lets you know home many moves the player must make, increment on loop


//game variables
int level = 1; //starting level, will increment with each success round
boolean waiting = true;
boolean fail = false;
boolean highScore = 0; // not implemented yet
int score = 0;
int moveScore = 10;
int needToBeValid = 50; // loops needs to be a valid side down, needed to debounce the analog pin read

// TODO: do not allow move across cube, as side down will not be able to be seen
// hold the next possible move for game, this array is needed as due to the
// hardware of the game we cannot make next move the side that is down (player would not see light
// also do not want to have up side to repeat move.. therefor only the 4 sides can be valid next movesp
void setup() {
  //easterEgg2();
  // shift reg  setup
  pinMode(latchPin, OUTPUT);
  pinMode(dataPin, OUTPUT);  
  pinMode(clockPin, OUTPUT);
  
  //Binary notation as comment if need single pins addressed
  //  dataArray[0] = 0xFF; //0b11111111
  //  dataArray[1] = 0xFE; //0b11111110
  //  dataArray[2] = 0xFC; //0b11111100
  //  dataArray[3] = 0xF8; //0b11111000
  //  dataArray[4] = 0xF0; //0b11110000
  //  dataArray[5] = 0xE0; //0b11100000
  //  dataArray[6] = 0xC0; //0b11000000
  
  // random seed with annalog read to make game pattern more random
  // https://www.arduino.cc/reference/en/language/functions/random-numbers/randomseed/
  randomSeed(analogRead(photoSensorPin3)); // pin3 is the one for the top side - most random reading based on light of room
  ser.begin(57600);
  lcd.begin(16,2);
  
  for (int i = 0; i < 6; i++) 
  {
    registerWrite(i, LOW);
  }  //end for loop
  showWelcome();  
} // end setup

void loop() {  
  while(waiting){
    waitForMovement();
    
  } // end while loop

  setGameStates(level);

   if(fail == true){
    ser.println("New Game");
    randomSeed(analogRead(photoSensorPin3)); // new random seed to make rando pattern fresh
    fail = false;
  } // end if for fail and restart
    
  gameLen++; // advance game one move
  gameMoves[gameLen] = random(0,6); //and rondom 0-6 -- CHANGE TO NOT ALLOW ACROSS BOX MOVES

  // do not allow duplicate moves
  if(gameMoves[gameLen] == gameMoves[gameLen-1])
   {
     //no duplicate moves
     while(gameMoves[gameLen] == gameMoves[gameLen-1]) 
     {
       gameMoves[gameLen]= random(0,6); 
     } // end while 
   } // end if 

   // use this to restart game
  if (gameLen == 0)
  {
    gameLen == 1;
    waiting = true;
  } // end if 

  startScreen();
  showGamePattern();
  playerPattern();
} // end loop

void setGameStates(int level){
  if(level == 1)
  {
    winMovesCount = 3;
    moveTime = 100;
    needToBeValid=25;
    moveScore = 10;
    gameDelay = 400;
  } // end if
  else if(level == 2)
  {
    //gameLen = 3;
    winMovesCount = 5;
    moveTime = 75;
    needToBeValid=20;
    moveScore = 20;
    gameDelay = 300;
  } // end else if
  else if(level = 3) 
  {
     //gameLen = 5;
     winMovesCount = 7;
     moveTime = 50;
     needToBeValid = 12;
     moveScore = 100;
     gameDelay = 100;
  } // end else if

  else if(level > 3) // if play gets past 3 there is an incremental increase in dificult to the point that game is unwinnable
  //or an game move possition array breaks.
  {
     //gameLen = 7;
     winMovesCount += 1;
     moveTime = 25;
     needToBeValid = 10;
     moveScore = 100;
     gameDelay -= 5; //will keep decreasing until unwinnable HAHAHAHA
  } // end else if
} // end setGameStates

void showWelcome()
{
  
  lcd.clear();
  lcd.setCursor(0, 0);           
  lcd.print("Simon Gets Tilty");
  lcd.setCursor(0, 1);          
  lcd.print("Ready to Play");   
  for (int i = 0; i < 10; i++){
    flashAll(50);
  } //end for loop
  delay(1500);
  
  lcd.clear();
  lcd.setCursor(0, 0);           
  lcd.print("Follow pattern");
  lcd.setCursor(0, 1);          
  lcd.print("by tilting box");  
  for (int i = 0; i < 10; i++){
    flashAll(50);
  } //end for loop
  delay(1500);

   lcd.clear();
  lcd.setCursor(0, 0);           
  lcd.print("Lit LED is side");
  lcd.setCursor(0, 1);          
  lcd.print("facing UP");  
  for (int i = 0; i < 10; i++){
    flashAll(50);
  } //end for loop
  delay(1500);
  
  lcd.clear();
  lcd.setCursor(0, 0);           
  lcd.print("Tilt another");
  lcd.setCursor(0, 1);          
  lcd.print("side up to start");
} // end showWelcome

void startScreen()
{
  lcd.clear();
  lcd.setCursor(0, 0);           
  lcd.print("Follow This!");
  delay(1500);
} // end startScreen

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

   //debugging sensor reads
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
} // end readSensors

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
    } // end if
  } // end for loop
//  Serial.print("Lowest Reading: "); // debug
//  Serial.print(photoSensors[index]); // debug
//  Serial.print(" at Pin: "); //edbug
//  Serial.println(index); //debug
  lightLED(index, 70);
  return index;
} // end getSideDown

void lightLED(int pin, int delayInterval){
  for (int i = 0; i < 6; i++) 
  {    
    if (i == pin){
      //Serial.print("Turning Light On: "); //debug
      //Serial.println(LEDpins[i]); //debug
      registerWrite(i, HIGH);
      delay(delayInterval);
      registerWrite(i, LOW);
    } // end if
    else{
      registerWrite(i, LOW);
    } // end else
  } // end for 
} // end lightLED

void lightLEDCorrect(int pin){
  //int LEDpins[6] = {LEDpin1, LEDpin2, LEDpin3, LEDpin4, LEDpin5, LEDpin6};
  for (int i = 0; i < 6; i++) 
  {
    if (i == pin){
      //Serial.print("Turning Light On: ");
      //Serial.println(LEDpins[i]);
      registerWrite(i, HIGH);
      delay(500);
      registerWrite(i, LOW);
      delay(250);
      registerWrite(i, HIGH);
      delay(500);
      registerWrite(i, LOW);
      delay(250);
    } // end if
    else{
     registerWrite(i, LOW);
    } // end else
  } // end for
} // end lightLEDCorrect

void flashAll(int delay_time)
{
  for(int i = 0;i<6;i++)
  { 
    registerWrite(i, HIGH);
    delay(delay_time);
    registerWrite(i, LOW);
  } // end for
} // end flashAll

void waitForMovement(void)
{ 
  // store the current position so we can compare it later and notice a change.
  int prevSideDown = readValidPosition(); 
  //flashAll(50);
  if(prevSideDown != readValidPosition()) 
  {
    delay(500); 
    waiting = false; // get out of the waiting mode 
  } // end if
} // end waitForMovement

int readValidPosition()
{
  // the "side down" must me a min number of readings for it to fire a valid side down
  // the needsToBeValid will change as difficulty increases
  int posArray[100]; // used to store position readings in a row. 
  int validReading = 0; // used to store the single read
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
      } // end if
    } // end for
    if(j == 0) {
      validReading += 1; // the first reading is alway correct
    } // end if 
    else{
      // check new reading against previous, if it's the same, increment valid
      if(posArray[j] == posArray[j-1])
      {
        validReading += 1; 
      } // end if
      else{
        delay(100);
        j=needToBeValid; // or start over
      } // end else 
    } // end else
  }  // end for
  // all 50 readins are the same so the current side down will change 
  if(validReading == needToBeValid){
    currentSideDown = posArray[0]; 
    ser.print("Side down: ");
    ser.println(currentSideDown); 
    return currentSideDown; 
  } //end if                                               
} // end readValidPosition

void playerPattern()
{
  lcd.clear();
  lcd.setCursor(0, 0);           
  lcd.print("Your Turn!");
  delay(1500);
  
  ser.print("player pattern:");
  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("PLAYER PATTERN: ");
  int previousSide = readValidPosition();
  ser.print("LEVEL=");
  ser.println(level);
  ser.print("GAMELEN=");
  ser.println(gameLen);
  ser.print("MovesWin=");
  ser.println(winMovesCount);
  for(int i=1; i<=gameLen;i++) 
  {    
    int timeout = 0;
    int wrong = 0;
    //int validSide = readValidPosition();
    while(timeout<4)
    {
      // get side down
      int validSide = readValidPosition();
      
      // correct move?
      if(validSide == gameMoves[i]) 
        {
          // lcd show
          lcd.clear();
          lcd.setCursor(0, 1);
          lcd.print(convertToSideName(validSide));
          ser.print(validSide);
          
          score += moveScore;

          //correct move tone
          tone(tonePin, melody[validSide], 500);
          delay(100);
          noTone(tonePin);

          // Blink indicate correct
          lightLEDCorrect(validSide);
          break;                    
        } // end if
        
      // timeout?
      else if ((timeout == 3)) 
      {
        //Serial.print("Timed out on position ");
        //Serial.println(i);
        lcd.clear();
        lcd.setCursor(0, 0);  
        lcd.print("TIMED OUT!");
        delay(2000);
        waiting = true;
        gameLen = 0;
        level = 1;
        fail = true;

        failState();
        
        for (int i = 0; i < 10; i++){
          flashAll(50);
        } //end for loop
        delay(2000);
        
        
        break;
      } // end else if 
       
      //did not move side yet
      else if(validSide == gameMoves[i-1]){}
      
      //wrong move
      else if(validSide != gameMoves[i]) 
      {
        wrong += 1; 
      } // end else if
                                                                            
      // on wrong for too long
      if(wrong >  2) 
      {
        ser.print("wrong: ");
        ser.println(convertToSideName(gameMoves[i]));
        
        lcd.clear();
        lcd.setCursor(0, 0);           
        lcd.print("Wrong Move: ");
        lcd.setCursor(0, 1);          
        lcd.print(convertToSideName(gameMoves[i]));     
        delay(100);
        // Blink quickly to show the incorrect tilt position
        flashAll(50);
                
        gameLen = 0; //end game
        level = 1; 
        fail = true;
        waiting = true;
       
        failState();
        break;
      } // end if
      timeout += 1;   // Increment timeout
   } // End of timeout while loop
  
   if((i == gameLen) && (fail == false))
   {
     ser.println("gamelen complete!");
     ser.println(currentSideDown);
     
     delay(250);
     // Check to see gameLen = winMoves
     if(gameLen == winMovesCount) //LEVEL complete
     {       
      
        lcd.clear();
        lcd.setCursor(0, 0);           
        lcd.print("Level Complete");
        lcd.setCursor(0, 1);          
        lcd.print("Next Level: ");  
        lcd.print(level + 1);  

        //complete level song
        tone(tonePin, NOTE_E4,800);
       delay(200);
       tone(tonePin, NOTE_F4,800);
       delay(200);
        tone(tonePin, NOTE_G4,1600);
       delay(400);
        tone(tonePin, NOTE_C5,800);
       delay(400);
       noTone(tonePin);
       delay(1000); // delay to give the player time to be ready for playback
        
       // complete level!!
       level += 1; // go to next level
       delay(2500);
     } // end if for player level complete condition 
     else{

      // lcd control
       lcd.clear();
       lcd.setCursor(0, 0);           
       lcd.print("Correct!");
       lcd.setCursor(0, 1);          
       lcd.print("Adding more");

       //correct tone
       tone(tonePin, NOTE_E4,800);
       delay(250);
       tone(tonePin, NOTE_F4,800);
       delay(250);
       noTone(tonePin);
       delay(1000); // delay to give the player time to be ready for playback
     } // end else 
    } // end gamelen complete if
  } // end of game pattern for loop
} // end playerPattern  

void failState()
{
 tone(tonePin, NOTE_F2,800);
 delay(500);
 tone(tonePin, NOTE_G2,800);
 delay(500);
 noTone(tonePin);
 delay(2000);

 if (score > 100)
 {
  easterEgg();
 }
 else {
  easterEgg2();
 }
 
 showScore(score);
 
  delay(5000);
  lcd.clear();
  lcd.setCursor(0, 0);           
  lcd.print("Tilt to");
  lcd.setCursor(0, 1);          
  lcd.print("play again"); 

  for (int i = 0; i < 10; i++){
    flashAll(50);
  } //end for loop
  delay(2000);
  score = 0;
} //end failState

void showScore(int score){
    lcd.clear();
    lcd.setCursor(0, 0);           
    lcd.print("Your Score:");
    lcd.setCursor(0, 1);          
    lcd.print(score);
    delay(5000);
} // end showScore

void showGamePattern()
{
  ser.print("game pattern:");
  lcd.clear();
  lcd.setCursor(0, 0);  
  lcd.print("GAME PATTERN: ");
  
  for(int i=1;i<=gameLen;i++) 
  {
    lcd.clear();
    lcd.setCursor(0, 1);
    lcd.print(convertToSideName(gameMoves[i]));
    
    ser.print(gameMoves[i]);
    ser.print(",");
    
    lightLED(gameMoves[i], 500);
    tone(tonePin, melody[gameMoves[i]], 500);
    delay(100);
    noTone(tonePin);
    
    delay(gameDelay); // change game delay with difficulty increases
  } // end for loop
    
  // print a new line, this helps keep the debug serial window more legible
  ser.println(" "); 
} // end showGamePattern

String convertToSideName(int sideNum){
  // convert the side number to a readable side, this could also be used to convert to colors if I had all diff colored LEDS
  switch(sideNum)
  {
    case 0:
    {
      return "Top";
      break;
    } // end case 0
    case 1:
    {
      return "Left";
      break;
    } // end case 1
    case 2:
    {
      return "Back";
      break;
    } // end case 2
    case 3:
    {
      return "Bottom";
      break;
    } // end case 3
    case 4:
    {
      return "Front";
      break;
    } // end case 4
    case 5:
    {
      return "Right";
      break;
    } // end case 5    
  }// end switch
} // end convertToSideNam

// This method sends bits to the shift register:
// https://www.arduino.cc/en/Tutorial/ShftOut12
void registerWrite(int whichPin, int whichState) {
// the bits you want to send
  byte bitsToSend = 0;
  // ASCII '0' through '9' characters are
  // represented by the values 48 through 57.
  // so if the user types a number from 0 through 9 in ASCII, 
  // you can subtract 48 to get the actual value:
  int bitToSet = whichPin - 48;

  // turn off the output so the pins don't light up
  // while you're shifting bits:
  digitalWrite(latchPin, LOW);

  // turn on the next highest bit in bitsToSend:
  bitWrite(bitsToSend, whichPin, whichState);

  // shift the bits out:
  shiftOut(dataPin, clockPin, MSBFIRST, bitsToSend);

    // turn on the output so the LEDs can light up:
  digitalWrite(latchPin, HIGH);

} // end registerWrite

void easterEgg(){
  // plays indiana jones theme song 
  //EFGC DEF GABF AABCDE EFGC DEF GABF AABCDE EFGC DEF GGED GED GED GFEDC
  tone(tonePin, NOTE_E4,800);
  delay(200);
  tone(tonePin, NOTE_F4,800);
  delay(200);
  tone(tonePin, NOTE_G4,800);
  delay(400);
  tone(tonePin, NOTE_C5,1600);
  delay(600);
  tone(tonePin, NOTE_D4,800);
  delay(200);
  tone(tonePin, NOTE_E4,800);
  delay(200);
  tone(tonePin, NOTE_F4,1600);
  delay(1500);

  tone(tonePin, NOTE_G4,800);
  delay(200);
  tone(tonePin, NOTE_A4,800);
  delay(200);
  tone(tonePin, NOTE_B4,800);
  delay(400);
  tone(tonePin, NOTE_F5,1600);
  delay(1500);

  tone(tonePin, NOTE_A4,800);
  delay(200);
  tone(tonePin, NOTE_B4,800);
  delay(400);
  tone(tonePin, NOTE_C4,800);
  delay(200);
  tone(tonePin, NOTE_D5,800);
  delay(400);
  tone(tonePin, NOTE_E5,1200);
  delay(1000);
  
}

void easterEgg2()
{
  // imperial march
  tone(tonePin, NOTE_A3, 500);
  delay(500);
  tone(tonePin, NOTE_A3, 500);
  delay(500);
  tone(tonePin, NOTE_A3, 500);
  delay(500);
  tone(tonePin, NOTE_F3, 350);
  delay(350);
  tone(tonePin, NOTE_CS3, 150);
  delay(150);
  tone(tonePin, NOTE_A3, 500);
  delay(500);
  tone(tonePin, NOTE_F3, 350);
  delay(350);
  tone(tonePin, NOTE_CS3, 150);
  delay(150);
  tone(tonePin, NOTE_A3, 650);
  delay(650);
}
