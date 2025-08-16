# LED-Memory-Game
Call and response game that generates a random sequence of LED flashes that the user has to remember. Four different "game modes" with LCD screen display. Circuit powered by ESP32 Microcontroller.

## How It Works
The ESP32 generates a random sequence of LED flashes that the player must memorize and repeat back by pressing the matching buttons. 
Each round, the sequence grows longer, increasing the difficulty. An LCD screen displays the current game mode and feedback. 
There are four different modes, including normal mode, hard mode (faster), increase mode (gradual spike in difficulty), and "Impossible Mode" 
(much faster). The game continues until the player makes an error.

## Skills Used
- Random sequence generation and memory logic in C++
- LED and button interfacing with debouncing
- LCD (I2C) display integration for modes and feedback
- Game state management with multiple difficulty modes
- Microcontroller programming (ESP32)

## Hardware
- ESP32 Dev Board
- 4x Resistors (2x 220 ohms, 1x 330 ohms, 1x 1.1k ohms for LEDS, 1x 220 ohms for Passive Buzzer)
- 4x LEDs (Red, Yellow, Green, and Blue)
- 1x Passive Buzzer
- 4x Pushbuttons
- Breadboard & Jumper Wires
