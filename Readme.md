# Tilty Simon
Project to create a Simon Game with specifications for ITM 593 Embedded Systems

## Project overview
### Device connection
1. Required for projct 
- 4 LED lights
- 4 Analog Force Sensitive resistors or potentiometers as input (or any other analog type of input
device)
o Use pressure as a threshold to consider a “button push”
- An LCD 16x2 Output Screen – for state messages
- A Piezo Buzzer for audio output
  - Give each “LED” a distinctive tone

- The connection indicator is the red image towards bottom


2. Actual build components
- 6 LED lights
- 6 Photo transistors (Analog inputs)
- LCD screen
- Shift Register
- Piezo Buzzer
- Bluetooth module (HC-06)

![Scheme](images/SimonTiltLight_schem.png "Schemeatic")

3. Game Play
- Start shows 3 screens to describe the game to the user
- Player must tilt another side up to signal start game
- The game pattern is shown to the user.
    - The user must tilt box according to pattern
    - Level 1-3 have a pattern of 3 to follow
    - Level 4-5 have pattern of 4 to follow with increasing difficulty
    - Difficulty increase means game pattern is shown faster, and time is shorter for player to move.
    - Score is kept and shown if player loses. 

4. Bluetooth module is connect and currently outputs debug statements , but can be used for player controls or game views in future development. 