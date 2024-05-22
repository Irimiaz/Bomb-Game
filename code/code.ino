#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
float maxTime = 100;
const int redLEDPin_buzzer = 5;       // Pin pentru LED-ul roșu
const int redLEDPin_culori = 6;             // Red LED pin
const int redLEDPin_buton = 12;     // Red LED pin
bool redBuzzer = false;
bool redCulori = false;
bool redButon = false;
bool timeRanOut = false;
bool greenBuzzer = false;
bool greenCulori = false;
bool greenButon = false;
bool greenFire = false;


void lightUPRed() {
  if(redBuzzer == false) {
      digitalWrite(redLEDPin_buzzer, HIGH);
      redBuzzer = true;
  } else if (redCulori == false) {
    digitalWrite(redLEDPin_culori, HIGH);
    redCulori = true;
  } else if (redButon == false) {
    digitalWrite(redLEDPin_buton, HIGH);
    redButon = true;
  }
}

//-----------culori_module
unsigned long startTime;
bool rgblight = true;

const int buttonPins[3] = {36, 34, 32}; // Push button pins for sequence input
const int resetButtonPin_culori = 30;        // Reset button pin
const int greenLEDPin_culori = 28;           // Green LED pin
const int rgbPins[3] = {37, 35, 33};  // RGB LED pins

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
      lightUPRed();
     

    } else {
      greenCulori = true;
      digitalWrite(greenLEDPin_culori, HIGH);
      // digitalWrite(redLEDPin_culori, HIGH);
    }

    // Reset for the next round
    userIndex = 0;
  }
}

//--------------------buton_module
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buttonPin_buton = 8;     // Push button pin
const int greenLEDPin_buton = 13;   // Green LED pin

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
  if(seconds <= 0.00) {
    timeRanOut = true;
    lcd.print("0.00");
  } else
    lcd.print(seconds);

  // Check if button is pressed
  if (buttonState == LOW && buttonPress_button == false) {
    buttonPress_button = true;
    // Check if the timer contains the number '2'
    if (String(seconds).indexOf('2') != -1) {
      greenButon = true;
      digitalWrite(greenLEDPin_buton, HIGH);
    } else {
      lightUPRed();
      // digitalWrite(redLEDPin_buton, HIGH);
    }
    delay(200);
  }
  if (buttonState == HIGH ) {
    buttonPress_button = false;
  }
}

//----------------morse module
const int buzzerPin_buzzer = A5;
const int buttonPin_buzzer = A2;        // Pin pentru butonul principal
const int resetButtonPin_buzzer = A3;   // Pin pentru butonul de resetare
const int submitButtonPin_buzzer = A4;  // Pin pentru butonul de trimitere
const int greenLEDPin_buzzer = A1;      // Pin pentru LED-ul verde
const int correctPressCount_buzzer = 5; // Numărul corect de apăsări pentru validare
int buttonPressCount_buzzer = 0;
bool playMessage = true;
bool submitPress_buzzer = true;
bool countPress_buzzer = true;
bool resetPress_buzzer = true;
enum MorseState {IDLE, DOT, DASH, LETTER_SPACE, DOT_SPACE, DASH_SPACE};
MorseState currentState = IDLE;
int messageIndex = 0;
void buzzer_module() {
  
    playMorseMessage(".... . .-.. .-.. ---");
  
  // Check the main button press
  if (digitalRead(buttonPin_buzzer) == LOW && countPress_buzzer == false) {
    countPress_buzzer = true;
    buttonPressCount_buzzer++;
    Serial.print("Button pressed. Count: ");
    Serial.println(buttonPressCount_buzzer);
    delay(200); // Debounce delay
  }
  if (digitalRead(buttonPin_buzzer) == HIGH) {
    countPress_buzzer = false;
  }

  // Check the reset button press
  if (digitalRead(resetButtonPin_buzzer) == LOW && resetPress_buzzer == false) {
    resetPress_buzzer = true;
    buttonPressCount_buzzer = 0;
    Serial.println("Counter reset.");
    currentState = IDLE;
    messageIndex = 0;
    delay(200); // Debounce delay
  }
  if (digitalRead(resetButtonPin_buzzer) == HIGH) {
    resetPress_buzzer = false;
  }

  // Check the submit button press
  if (digitalRead(submitButtonPin_buzzer) == LOW && submitPress_buzzer == false) {
    submitPress_buzzer = true;
    if (buttonPressCount_buzzer == correctPressCount_buzzer) {
      digitalWrite(greenLEDPin_buzzer, HIGH);
      greenBuzzer = true;
      Serial.println("Correct! Green LED ON.");
    } else {
      lightUPRed();
      // digitalWrite(redLEDPin_buzzer, HIGH);
      buttonPressCount_buzzer = 0;
      Serial.println("Incorrect. Red LED ON.");
    }
    delay(200); // Debounce delay
  }
  if (digitalRead(submitButtonPin_buzzer) == HIGH) {
    submitPress_buzzer = false;
  }
}

unsigned long previousMillis = 0; // Store the last time an action was performed
unsigned long interval = 0; // Interval for the current action


void playMorseMessage(const char *morseMessage) {
  unsigned long currentMillis = millis();
  
  switch (currentState) {
    case IDLE:
      if (morseMessage[messageIndex] != '\0') {
        if (morseMessage[messageIndex] == '.') {
          tone(buzzerPin_buzzer, 1000); // Sound for dot
          interval = 200; // Duration of dot
          currentState = DOT;
        } else if (morseMessage[messageIndex] == '-') {
          tone(buzzerPin_buzzer, 1000); // Sound for dash
          interval = 600; // Duration of dash
          currentState = DASH;
        } else if (morseMessage[messageIndex] == ' ') {
          interval = 700; // Pause between letters
          currentState = LETTER_SPACE;
        }
        previousMillis = currentMillis;
      } else {
        // Message finished
        currentState = IDLE;
      }
      break;

    case DOT:
      if (currentMillis - previousMillis >= interval) {
        noTone(buzzerPin_buzzer);
        interval = 200; // Pause between dots/dashes
        currentState = DOT_SPACE;
        previousMillis = currentMillis;
      }
      break;

    case DASH:
      if (currentMillis - previousMillis >= interval) {
        noTone(buzzerPin_buzzer);
        interval = 200; // Pause between dots/dashes
        currentState = DASH_SPACE;
        previousMillis = currentMillis;
      }
      break;

    case DOT_SPACE:
    case DASH_SPACE:
      if (currentMillis - previousMillis >= interval) {
        messageIndex++;
        currentState = IDLE;
      }
      break;

    case LETTER_SPACE:
      if (currentMillis - previousMillis >= interval) {
        messageIndex++;
        currentState = IDLE;
      }
      break;
  }
}

//---------fire module
const int greenPIN_fire = 47; // Pinul pentru LED
const int checkPins_fire[6] = {46, 49, 50, 51, 52, 53}; // Pinii pentru verificare
const int expectedValues_fire[6] = {HIGH, HIGH, LOW, HIGH, LOW, LOW}; // Valorile așteptate când firele sunt deconectate
bool oneTimeRed = true;
int firegresite = 0;
void fire_module() {
  Serial.println(firegresite);
  int counter = 0;
  bool redLight = false;
  bool shouldLightUp = true;
  for (int i = 0; i < 6; i++) {
    int pinValue = digitalRead(checkPins_fire[i]);
    // Serial.print(expectedValues_fire[i]);
    // Serial.print(" ");
    // Serial.print(pinValue);
    // Serial.println("");
    if (pinValue != expectedValues_fire[i] && pinValue == HIGH) {
      redLight = true;
      counter ++;
      // break;
    }
    if (pinValue != expectedValues_fire[i]) {
      shouldLightUp = false;
      // break;
    }
  }
  if(shouldLightUp) {
    greenFire = true;
    digitalWrite(greenPIN_fire, HIGH);
  }
  // Serial.println("--");
  if (redLight && counter > firegresite) {
    firegresite = counter;
    lightUPRed();
    oneTimeRed = false;
    // digitalWrite(redLEDPin_buzzer, HIGH); 
    Serial.println("All connections correct. LED ON.");
  } 
}

int pos = 0;
bool steag = true;
Servo servo;

void loop() {
  if (redButon || timeRanOut || (greenButon && greenFire && greenBuzzer && greenCulori)) {
    if(steag) {
      steag = false;
      servo.write(40);
    }
    return;
  }
  colors_module();
  buton_module();
  buzzer_module();
  fire_module();
}



void setup() {
  servo.attach(9);
  servo.write(120);
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

  //morse module
  pinMode(resetButtonPin_buzzer, INPUT_PULLUP);
  pinMode(buttonPin_buzzer, INPUT_PULLUP);
  pinMode(submitButtonPin_buzzer, INPUT_PULLUP);
  pinMode(buzzerPin_buzzer, OUTPUT);
  pinMode(greenLEDPin_buzzer, OUTPUT);
  pinMode(redLEDPin_buzzer, OUTPUT);

  //fire module
  pinMode(greenPIN_fire, OUTPUT);
  for (int i = 0; i < 6; i++) {
    pinMode(checkPins_fire[i], INPUT_PULLUP); // Setează pinii de verificare ca intrări cu rezistență internă de pull-up
  }
}

