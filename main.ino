//by Mehmet Alihan Soykal

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//SDA A4
//SCL A5
//G G
//VCC 5V

//G G
//VCC 5V
//D0 2

const int photoresistorPin = 2;  
int currentState = LOW;         
unsigned long duration = 0;    
boolean hasStarted = false;
String myString = "";
String characters = "";
String realWord = "";
boolean forTheFirstTime = true; // to avoid an error due to connecting i2c gives a dash in the beginning

LiquidCrystal_I2C lcd(0x27, 16, 2);


const char* morseCodeDict[] = {
  ".-",    // A
  "-...",  // B
  "-.-.",  // C
  "-..",   // D
  ".",     // E
  "..-.",  // F
  "--.",   // G
  "....",  // H
  "..",    // I
  ".---",  // J
  "-.-",   // K
  ".-..",  // L
  "--",    // M
  "-.",    // N
  "---",   // O
  ".--.",  // P
  "--.-",  // Q
  ".-.",   // R
  "...",   // S
  "-",     // T
  "..-",   // U
  "...-",  // V
  ".--",   // W
  "-..-",  // X
  "-.--",  // Y
  "--..",  // Z
  ".----", // 1
  "..---", // 2
  "...--", // 3
  "....-", // 4
  ".....", // 5
  "-....", // 6
  "--...", // 7
  "---..", // 8
  "----.", // 9
  "-----"  // 0
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

void setup() {
  pinMode(photoresistorPin, INPUT);
  lcd.init();
  lcd.backlight();
  Serial.begin(9600);

}

void loop() {
  int photoresistorValue = digitalRead(photoresistorPin);
  unsigned long currentTime = millis();
  static unsigned long startTime = 0; 

  // Check if no state change has occurred for more than 2100ms
  if (currentTime - startTime > 2300 && hasStarted) {
    char letter = getLetter(characters);
    realWord += letter;
    characters = "";
    Serial.print("/");
    hasStarted = false;
    lcd.setCursor(0,1);
    lcd.print(realWord);
    delay(2000);

    exit(0);

  }

  if (photoresistorValue != currentState) {
    // Photoresistor state has changed
    duration = currentTime - startTime;
    unsigned long dividedDuration = duration / 300;

    if (currentState == LOW) {
      if (forTheFirstTime == false) {
        // Transition from black to white
        if (dividedDuration >= 2.7 && dividedDuration <= 3.3) {
          Serial.print("-");
          hasStarted = true;
          characters += "-";
        }
      }

      if (dividedDuration >= 0.9 && dividedDuration <= 1.1) {
        Serial.print(".");
        hasStarted = true;
        characters += ".";
      }
forTheFirstTime = false;
    } else {
      // Transition from white to black
      if (dividedDuration >= 6.3 && dividedDuration <= 7.7) {
        Serial.print("/ ");
        char letter = getLetter(characters);
        realWord += letter;
        realWord += " ";
lcd.setCursor(0, 1);            
  lcd.print(realWord);
        characters = "";
        //Serial.println(" ");

      }
      if (dividedDuration >= 2.7 && dividedDuration <= 3.3) {
        Serial.print("/");
        char letter = getLetter(characters);
        realWord += letter;
lcd.setCursor(0, 1);            
  lcd.print(realWord);
        //Serial.print(letter);
        characters = "";
      }
    }

    currentState = photoresistorValue;
    startTime = currentTime;
  }
  delay(150);
}
