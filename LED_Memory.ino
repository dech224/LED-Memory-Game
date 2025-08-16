
#include <LCD-I2C.h>
#include <Wire.h>

LCD_I2C lcd(0x27, 16, 2);

const int RED_LED = 2;
const int RED_BUTTON = 15;
const int YELLOW_LED = 16;
const int YELLOW_BUTTON = 4;
const int GREEN_LED = 5;
const int GREEN_BUTTON = 17;
const int BLUE_LED = 18;
const int BLUE_BUTTON = 19;

const int BUZZER_PIN = 13;
const int RED_TONE = 440;     //A4
const int YELLOW_TONE = 523;  //C5
const int GREEN_TONE = 587;   //D5
const int BLUE_TONE = 659;    //E5

bool start_flag = false;
bool countdown_flag = false;
bool call_flag = false;
bool response_flag = true;
bool game_over_flag = false;

int r = random(0, 3);
const int MAX = 100;
int pattern[MAX];
int pattern_length = 0;

int score = 0;

//Game modes set by pressing LEDs
enum GameMode {
  NORMAL,
  INCREASE,
  HARD,
  IMPOSSIBLE
};

GameMode current_mode;

void Game_Over();
void Before_Start();
void Countdown();
void Call_Logic();
void Show_LED(int num);
void On();
void Off();
int Wait_For_Press();
void Response_Logic();

void setup() {
  //Pin setups
  pinMode(RED_LED, OUTPUT);
  pinMode(RED_BUTTON, INPUT_PULLUP);
  pinMode(YELLOW_LED, OUTPUT);
  pinMode(YELLOW_BUTTON, INPUT_PULLUP);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(GREEN_BUTTON, INPUT_PULLUP);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(BLUE_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  ledcAttach(BUZZER_PIN, 2000, 8);

  //LCD setup
  Wire.begin();
  lcd.begin(&Wire);
  lcd.display();
  lcd.backlight();
  lcd.print("Waiting...");
}

void loop() {

  //Checks for  game failure
  if (game_over_flag) { 
    Game_Over();
  } 
  
  //Checks for start flag, each button sets start flag to true and sets specific mode
  else if (!start_flag) {
    if (digitalRead(RED_BUTTON) == LOW) {
      current_mode = NORMAL;
      start_flag = true;
      pattern_length = 0;
      score = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Mode: Normal");

    } else if (digitalRead(YELLOW_BUTTON) == LOW) {
      current_mode = INCREASE;
      start_flag = true;
      pattern_length = 0;
      score = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Mode: Increase");

    } else if (digitalRead(GREEN_BUTTON) == LOW) {
      current_mode = HARD;
      start_flag = true;
      pattern_length = 0;
      score = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Mode: Hard");

    } else if (digitalRead(BLUE_BUTTON) == LOW) {
      current_mode = IMPOSSIBLE;
      start_flag = true;
      pattern_length = 0;
      score = 0;
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Mode: Impossible");

    } else {
      //No buttons pressed, Alternating LED pattern
      Before_Start();
    }
  } 
  //Start flag true (button pressed)
  else {
    //Starts countdown
    if (!countdown_flag) {
      Countdown();
      delay(800);
      countdown_flag = true;

    } 
    //Starts Call Logic, sets response false
    else if (!call_flag) {
      Call_Logic();
      call_flag = true;
      response_flag = false;

    } 
    //Response Logic starts
    else if (!response_flag) {
      Response_Logic();
      delay(500);
    }
  }
}





//Alternating Pattern while waiting for buttons to be pressed
void Before_Start() {
  //Scrolling LCD text logic
  static unsigned long last_scroll_time = 0;
  static int scroll_index = 0;
  const char* modes[] = {
    "Red: Normal Mode  ",
    "Yellow: Increase  ",
    "Green: Hard Mode  ",
    "Blue: Impossible  "
  };

  //Update scrolling line every 1.2 seconds
  if (millis() - last_scroll_time >= 1200) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Hold for Mode:");
    lcd.setCursor(0, 1);
    lcd.print(modes[scroll_index]);
    scroll_index = (scroll_index + 1) % 4;
    last_scroll_time = millis();
  }

  //Quick alternating LED pattern
  digitalWrite(RED_LED, HIGH);
  delay(150);
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  delay(150);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  delay(150);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, HIGH);
  delay(150);
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, HIGH);
  delay(150);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(YELLOW_LED, HIGH);
  delay(150);
  digitalWrite(YELLOW_LED, LOW);
}


//3 Blinks on and off
void Countdown() {
  delay(1000);
  On();
  ledcWriteTone(BUZZER_PIN, 300);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("3...");
  delay(500);
  Off();
  ledcWriteTone(BUZZER_PIN, 0);
  delay(500);
  On();
  ledcWriteTone(BUZZER_PIN, 300);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("2...");
  delay(500);
  Off();
  ledcWriteTone(BUZZER_PIN, 0);
  delay(500);
  On();
  ledcWriteTone(BUZZER_PIN, 300);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("1...");
  delay(500);
  Off();
  ledcWriteTone(BUZZER_PIN, 0);
  delay(500);
  ledcWriteTone(BUZZER_PIN, 600);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("GO!!!");
  On();
  delay(500);
  ledcWriteTone(BUZZER_PIN, 0);
  Off();

  return;
}

//Generates and displays the LED sequence for the current round
void Call_Logic() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Watch Closely...");

  //Append a new random LED to the sequence (up to MAX)
  if (pattern_length < MAX) {
    pattern[pattern_length] = random(0, 4); //Random number 0–3 maps to LEDs
    pattern_length++;
  }

  int Time; //LED on/off duration for sequence display

  //Set display speed based on current game mode
  switch (current_mode) {
    case NORMAL:
      Time = 300; //Constant speed
      break;
    case INCREASE:
      Time = max(50, 300 - pattern_length * 15); //Faster as sequence grows
      break;
    case HARD:
      Time = 150; //Constant, faster speed
      break;
    case IMPOSSIBLE:
      Time = 75; //Super fast
      break;
  }

  //Loop through and display the stored sequence
  for (int i = 0; i < pattern_length; i++) {
    Show_LED(pattern[i]); 
    delay(Time);          
    Off();                
    delay(Time);          
  }
}

//Asigns LED Color to 0-3
void Show_LED(int num) {
  if (num == 0) {
    digitalWrite(RED_LED, HIGH);
    ledcWriteTone(BUZZER_PIN, RED_TONE);
  } else if (num == 1) {
    digitalWrite(YELLOW_LED, HIGH);
    ledcWriteTone(BUZZER_PIN, YELLOW_TONE);
  } else if (num == 2) {
    digitalWrite(GREEN_LED, HIGH);
    ledcWriteTone(BUZZER_PIN, GREEN_TONE);
  } else if (num == 3) {
    digitalWrite(BLUE_LED, HIGH);
    ledcWriteTone(BUZZER_PIN, BLUE_TONE);
  }
}

//All LEDs On
void On() {
  digitalWrite(RED_LED, HIGH);
  digitalWrite(YELLOW_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, HIGH);
}

//All LEDs Off
void Off() {
  digitalWrite(RED_LED, LOW);
  digitalWrite(YELLOW_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  digitalWrite(BLUE_LED, LOW);
  ledcWriteTone(BUZZER_PIN, 0);
}

//Waits until the user gives an input, and returns value for specific LED
int Wait_For_Press() {
  while (true) {
    if (digitalRead(RED_BUTTON) == LOW) {
      while (digitalRead(RED_BUTTON) == LOW);
      return 0;
      //Each of these numbers gets compared to Show_LED()
    }
    if (digitalRead(YELLOW_BUTTON) == LOW) {
      while (digitalRead(YELLOW_BUTTON) == LOW);
      return 1;
    }
    if (digitalRead(GREEN_BUTTON) == LOW) {
      while (digitalRead(GREEN_BUTTON) == LOW);
      return 2;
    }
    if (digitalRead(BLUE_BUTTON) == LOW) {
      while (digitalRead(BLUE_BUTTON) == LOW);
      return 3;
    }
  }
}

//Checks player's input against the sequence
void Response_Logic() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Repeat!");

  //Loop through the sequence, waiting for button presses
  for (int i = 0; i < pattern_length; i++) {
    int input = Wait_For_Press(); //Wait until a button is pressed

    int Time; //LED/tone display time for feedback

    //Set response feedback speed based on mode
    switch (current_mode) {
      case NORMAL:
        Time = 300;
        break;
      case INCREASE:
        Time = max(75, 300 - pattern_length * 15);
        break;
      case HARD:
        Time = 150;
        break;
      case IMPOSSIBLE:
        Time = 75;
        break;
    }

    //Briefly flash the pressed LED with tone feedback
    Show_LED(input);
    delay(Time);
    Off();

    //If player presses wrong LED, trigger Game Over
    if (input != pattern[i]) {
      game_over_flag = true;
      return;
    }
  }

  //If correct, prepare for next round
  call_flag = false;      
  response_flag = true;   
  score++;                //Increase score for correct sequence
}


void Game_Over() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over!");
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);

  //Flashing LED after loss
  On();
  ledcWriteTone(BUZZER_PIN, 300);
  delay(500);
  Off();
  ledcWriteTone(BUZZER_PIN, 0);
  delay(500);
  On();
  ledcWriteTone(BUZZER_PIN, 300);
  delay(500);
  Off();
  ledcWriteTone(BUZZER_PIN, 0);
  delay(500);
  On();
  ledcWriteTone(BUZZER_PIN, 300);
  delay(500);
  Off();
  ledcWriteTone(BUZZER_PIN, 0);
  delay(500);


  //Reset
  start_flag = false;
  countdown_flag = false;
  call_flag = false;
  response_flag = true;
  game_over_flag = false;
  pattern_length = 0;
}
