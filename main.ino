#include <Wire.h>
#include <LiquidCrystal_I2C.h>

const int photoresistorPin = 2;
int currentState = HIGH;
unsigned long startTime = 0;
unsigned long endTime = 0;
unsigned long currentTime = 0;
boolean hasStarted = false;
String symbols = "";
String convertedWord = "";
boolean convertionFinished = false;


LiquidCrystal_I2C lcd(0x27, 16, 2);

const char* morseCodeDict[] = {
  ".-",      // A
  "-...",    // B
  "-.-.",    // C
  "-..",     // D
  ".",       // E
  "..-.",    // F
  "--.",     // G
  "....",    // H
  "..",      // I
  ".---",    // J
  "-.-",     // K
  ".-..",    // L
  "--",      // M
  "-.",      // N
  "---",     // O
  ".--.",    // P
  "--.-",    // Q
  ".-.",     // R
  "...",     // S
  "-",       // T
  "..-",     // U
  "...-",    // V
  ".--",     // W
  "-..-",    // X
  "-.--",    // Y
  "--..",    // Z
  ".----",   // 1
  "..---",   // 2
  "...--",   // 3
  "....-",   // 4
  ".....",   // 5
  "-....",   // 6
  "--...",   // 7
  "---..",   // 8
  "----.",   // 9
  "-----",   // 0
  ".-.-.-",  // Period (.)
  "--..--",  // Comma (,)
  "..--..",  // Question mark (?)
  ".----.",  // Apostrophe (')
  "-.-.--",  // Exclamation mark (!)
  "-..-.",   // Slash (/)
  "-.--.",   // Parenthesis open (()
  "-.--.-",  // Parenthesis closed ())
  ".-...",   // Ampersand (&)
  "---...",  // Colon (:)
  "-.-.-.",  // Semicolon (;)
  "-....-",  // Double dash (--)
  ".-.-.",   // Plus (+)
  "-....-",  // Hyphen (-)
  "-....-",  // Minus (-)
  ".-..-.",  // Quotation mark (")
  "...-..-", // Dollar sign ($)
  ".--.-.",  // At sign (@)
};


const int morseCodeDictSize = sizeof(morseCodeDict) / sizeof(morseCodeDict[0]);

char getLetter(String symbol) {
  const char* symbolChar = symbol.c_str();
  for (int i = 0; i < morseCodeDictSize; i++) {
    if (strcmp(symbolChar, morseCodeDict[i]) == 0) {
      return 'A' + i;
    }
  }
  return '\0';
}

void initializeLCD() {

  byte heartLeftPart[] = {
    0x06,
    0x0F,
    0x1F,
    0x1F,
    0x0F,
    0x07,
    0x03,
    0x01
  };

  byte heartRightPart[] = {
    0x0C,
    0x1E,
    0x1F,
    0x1F,
    0x1E,
    0x1C,
    0x18,
    0x10
  };

  lcd.init();
  lcd.backlight();
  lcd.createChar(0, heartLeftPart);
  lcd.createChar(1, heartRightPart);
  lcd.setCursor(3, 0);
  lcd.print("Morse Code");
  lcd.setCursor(2, 1);
  lcd.print("Converter  ");
  lcd.write(0);
  lcd.write(1);
  delay(3000);
  lcd.clear();
}


void setup() {
  Serial.begin(9600);
  initializeLCD();
  pinMode(photoresistorPin, INPUT);
}

void loop() {
 while (convertionFinished == false) {
   currentTime = millis();
  int photoresistorValue = digitalRead(photoresistorPin);
  if (photoresistorValue != currentState) {
    endTime = millis();
    currentState = photoresistorValue;
    int duration = calculateDuration(startTime, endTime);
    printSymbol(currentState, duration);
    hasStarted = true;
    startTime = millis();
    currentTime = 0;
  }
  else {
    finishConvertion();
  }}
}

int calculateDuration(unsigned long startTime, unsigned long endTime) {
  float interval = endTime - startTime;
  float dividedDuration = interval / 300;
  int duration = round(dividedDuration);
  return duration;
}

void printSymbol(int currentState, int duration) {
  if (currentState == HIGH) {
    if (duration == 1) {
      Serial.print(".");
      symbols += ".";
    }
    if (duration == 3) {
      Serial.print("-");
      symbols += "-";
    }
  }
  if (currentState == LOW) {
    if (duration == 7) {
      Serial.print("/ ");
      updateLcdDisplay(convertedWord, symbols, true);
      symbols = "";
    }
    if (duration == 3) {
      Serial.print("/");
      updateLcdDisplay(convertedWord, symbols, false);
      symbols = "";
    }
  }
}

void updateLcdDisplay(String word, String symbols, boolean space) {
  char letter = getLetter(symbols);
  word += letter;
  if ( space ) {
    word += " ";
  }

  lcd.print(word);

}

void finishConvertion() {
  byte smiley[] = {
    0x00, 0x00, 0x0A, 0x00, 0x11, 0x0E, 0x00, 0x00
  };
  int duration = calculateDuration(startTime, currentTime);
  if (duration > 7 && hasStarted) {
    Serial.print("/");
    updateLcdDisplay(convertedWord, symbols, false);
    lcd.setCursor(0, 1);
    lcd.print("Word completed!");
    delay(3000);
    lcd.clear();
    lcd.createChar(2, smiley);
    lcd.setCursor(0, 2);
    lcd.print("See you ");
    lcd.write(2);
    delay(2000);
    for (int i = 0; i < 16; i++) {
      lcd.scrollDisplayLeft();
      delay(200);
    }
    convertionFinished = true;
  }
}


