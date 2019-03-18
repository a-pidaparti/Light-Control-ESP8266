#include <LiquidCrystal.h>
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int buttonPin = 5;
const int temperaturePin = 0;
void setup() {
  // put your setup code here, to run once:
  lcd.begin(16,2);
}

void loop() {
  // put your main code here, to run repeatedly:
  float degreesC = getDegrees(temperaturePin);
  float degreesF = (getDegrees(temperaturePin) * 1.8) + 32;
  lcd.setCursor(0,0);
  if(analogRead(5)<= 900){
      lcd.print("Degrees C: ");
      lcd.print(degreesC);
  }
  else{
    lcd.print("Degrees F: ");
    lcd.print(degreesF);
  }
}

float getDegrees(int pin){
  return ((analogRead(pin) * .004882814 - .5) * 10.0);
}

