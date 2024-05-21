#include <Wire.h>
#include <LiquidCrystal_I2C.h>
float maxTime = 100;


//-----------culori_module
unsigned long startTime;
bool rgblight = true;

const int buttonPins[3] = {2, 3, 4}; // Push button pins for sequence input
const int resetButtonPin_culori = 7;        // Reset button pin
const int redLEDPin_culori = 5;             // Red LED pin
const int greenLEDPin_culori = 6;           // Green LED pin
const int rgbPins[3] = {9, 10, 11};  // RGB LED pins

int sequence[8];       // Stores the random sequence of 8 colors
int userSequence[8];   // Stores the user's input sequence
int userIndex = 0;     // Tracks the user's current input position
bool userTurn = false;


void setColor(int color) {
  switch (color) {
    case 0: // Red
      digitalWrite(rgbPins[0], HIGH);
      digitalWrite(rgbPins[1], LOW);
      digitalWrite(rgbPins[2], LOW);
      break;
    case 1: // Green
      digitalWrite(rgbPins[0], LOW);
      digitalWrite(rgbPins[1], HIGH);
      digitalWrite(rgbPins[2], LOW);
      break;
    case 2: // Blue
      digitalWrite(rgbPins[0], LOW);
      digitalWrite(rgbPins[1], LOW);
      digitalWrite(rgbPins[2], HIGH);
      break;
    default: // Off
      digitalWrite(rgbPins[0], LOW);
      digitalWrite(rgbPins[1], LOW);
      digitalWrite(rgbPins[2], LOW);
      break;
  }
}


int brightness = 1;
int index = 0;
unsigned long time_now = 0;
void light() {
  int period = 500;
  if (index < 8) {
    // Serial.println(index);
    if (brightness == 1) {
      setColor(sequence[index]);
    } 
    else {
      setColor(-1);
    }
    if(millis() >= time_now + period){
      if (brightness == 1)
          index ++;
      time_now += period;
      brightness = 1 - brightness;
    }
  }
  else {
    setColor(-1);
    index = 0;
    brightness = 1;
    rgblight = false;
  }
}




bool buttonPressReset = false;
bool buttonPress[3] = {false, false, false};
int buttonTimeNow = 0;

void colors_module() {
  int period = 100;
  if (rgblight) {
    light();
  }  
  
  if (userIndex < 8) {
    //reset button
    if (digitalRead(resetButtonPin_culori) == LOW && buttonPressReset == false && millis() >= buttonTimeNow + period) {
      buttonPressReset = true;
      if(buttonPressReset) {
        Serial.println("Reset button pressed");
        rgblight = true;
        userIndex = 0;
        buttonTimeNow += period;
        delay(200);
        time_now = millis();
      }
    }
    if (digitalRead(resetButtonPin_culori) == HIGH ) {
      buttonPressReset = false;
    }
  

    //normal buttons
    for (int i = 0; i < 3; i++) {
      if (digitalRead(buttonPins[i]) == LOW && buttonPress[i] == false && millis() >= buttonTimeNow + period) {
        buttonPress[i] = true;
        if(buttonPress[i]) {
          Serial.print("User pressed button: ");
          Serial.println(i);
          userSequence[userIndex] = i;
          userIndex++;
          buttonTimeNow += period;
          delay(200);
        }
      }
      if (digitalRead(buttonPins[i]) == HIGH ) {
        buttonPress[i] = false;
      
      } 
    }
  }

  // Check if the user's sequence matches the generated sequence
  if (userIndex == 8) {
    bool correct = true;
    for (int i = 0; i < 8; i++) {
      Serial.println(userSequence[i]);
      Serial.println(sequence[i]);
      Serial.println();
      if (userSequence[i] != sequence[i]) {
        correct = false;
        break;
      }
    }

    // Light up the corresponding LED based on the result
    if (!correct) {
      digitalWrite(greenLEDPin_culori, HIGH);

    } else {
      digitalWrite(redLEDPin_culori, HIGH);
    }

    // Reset for the next round
    userIndex = 0;
  }
}

//--------------------buton_module
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buttonPin_buton = 8;     // Push button pin
const int greenLEDPin_buton = 13;   // Green LED pin
const int redLEDPin_buton = 12;     // Red LED pin

unsigned long startTime_buton;     // Start time of the timer
bool timerRunning = false;   // Timer state
int seconds = 0;             // Seconds counter
bool buttonPress_button = false;
void buton_module() {

  int buttonState = digitalRead(buttonPin_buton);
  float currentTime = millis() - startTime_buton;  // Calculate elapsed time
  float seconds = maxTime - currentTime / 1000;
  // Display the timer on the LCD
  lcd.setCursor(8, 0);
  lcd.print(seconds);

  // Check if button is pressed
  if (buttonState == LOW && buttonPress_button == false) {
    buttonPress_button = true;
    // Check if the timer contains the number '2'
    if (String(seconds).indexOf('2') != -1) {
      digitalWrite(greenLEDPin_buton, HIGH);
    } else {
      digitalWrite(redLEDPin_buton, HIGH);
    }
    delay(200);
  }
  if (buttonState == HIGH ) {
    buttonPress_button = false;
  }
}

void loop() {
  colors_module();
  buton_module();
}



void setup() {
  Serial.begin(9600);
  digitalWrite(greenLEDPin_buton, LOW);
  for (int i = 0; i < 3; i++) {
    pinMode(buttonPins[i], INPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(resetButtonPin_culori, INPUT);
  pinMode(resetButtonPin_culori, INPUT_PULLUP);
  pinMode(redLEDPin_culori, OUTPUT);
  pinMode(greenLEDPin_culori, OUTPUT);
  for (int i = 0; i < 3; i++) {
    pinMode(rgbPins[i], OUTPUT);
  }
  
  randomSeed(analogRead(0)); // Seed the random generator

  // Generate a random sequence of 8 colors
  for (int i = 0; i < 8; i++) {
    sequence[i] = random(0, 3); // Randomly choose 0 (red), 1 (green), or 2 (blue)
  }


  //buton module
  pinMode(buttonPin_buton, INPUT);
  pinMode(greenLEDPin_buton, OUTPUT);
  pinMode(redLEDPin_buton, OUTPUT);
  pinMode(buttonPin_buton, INPUT_PULLUP);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(1, 0);
  lcd.print("Time: ");
  if (!timerRunning) {
      startTime_buton = millis();  // Start the timer
      timerRunning = true;
    }
}

