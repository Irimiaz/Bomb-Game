
//culori_module
unsigned long startTime;
bool rgblight = true;

const int buttonPins[3] = {2, 3, 4}; // Push button pins for sequence input
const int resetButtonPin = 7;        // Reset button pin
const int redLEDPin = 5;             // Red LED pin
const int greenLEDPin = 6;           // Green LED pin
const int rgbPins[3] = {9, 10, 11};  // RGB LED pins

int sequence[8];       // Stores the random sequence of 8 colors
int userSequence[8];   // Stores the user's input sequence
int userIndex = 0;     // Tracks the user's current input position
bool userTurn = false;



void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 3; i++) {
    pinMode(buttonPins[i], INPUT);
    pinMode(buttonPins[i], INPUT_PULLUP);
  }
  pinMode(resetButtonPin, INPUT);
  pinMode(resetButtonPin, INPUT_PULLUP);
  pinMode(redLEDPin, OUTPUT);
  pinMode(greenLEDPin, OUTPUT);
  for (int i = 0; i < 3; i++) {
    pinMode(rgbPins[i], OUTPUT);
  }
  
  randomSeed(analogRead(0)); // Seed the random generator

  // Generate a random sequence of 8 colors
  for (int i = 0; i < 8; i++) {
    sequence[i] = random(0, 3); // Randomly choose 0 (red), 1 (green), or 2 (blue)
  }

}

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
    if (digitalRead(resetButtonPin) == LOW && buttonPressReset == false && millis() >= buttonTimeNow + period) {
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
    if (digitalRead(resetButtonPin) == HIGH ) {
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
      digitalWrite(greenLEDPin, HIGH);

    } else {
      digitalWrite(redLEDPin, HIGH);
    }

    // Reset for the next round
    userIndex = 0;
  }
}

void loop() {
  colors_module();
}


