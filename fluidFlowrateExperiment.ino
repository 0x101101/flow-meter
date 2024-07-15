#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <FlowMeter.h>
#include <EEPROM.h>
FlowMeter Meter = FlowMeter(2);
#include <OneButton.h>
OneButton forward(10, true);
OneButton backward(11, true);
OneButton power(12, true);

int buttonPushCounter = 0;
int buttonState = 0;
int lastButtonState = 0;
const unsigned long period = 300;
const int detect = 3;
int charge = 0;
const int voltageMin = 2.8;
const int voltageMax = 5.0;
int led = 9;
char data;
byte increment;
byte incrementB;
int val2;
int x;
void(*resetFunc)(void) = 0;

void MeterISR() {
  Meter.count();
}
void setup() {
  pinMode(detect, INPUT);
  Serial.begin(9600);

  lcd.begin();
  lcd.backlight();
  attachInterrupt(INT0, MeterISR, RISING);
  Meter.reset();
  pinMode(led, OUTPUT);
  digitalWrite(led, HIGH);
  val2 = EEPROM.read(0);
  power.attachDoubleClick(resetF);
  power.attachClick(on);
  power.attachLongPressStop(off);
  forward.attachClick(forwardClick);
  backward.attachClick(backwardClick);
  forward.attachLongPressStop(forwardlongclick);
  backward.attachLongPressStop(backwardlongclick);

  pinMode(13, OUTPUT);
  welcome();
  x = 85;
}
void resetF() {
  resetFunc();
  delay(50);
}

void forwardlongclick() {
  incrementB ++;
  lcd.clear();
  delay(50);
}
void backwardlongclick() {
  incrementB --;
  lcd.clear();
  delay(50);
}
void forwardClick() {
  digitalWrite(13, HIGH);
  delay(50);
  digitalWrite(13, LOW);
  lcd.clear();
  lcd.noBacklight();
  lcd.noDisplay();
  increment ++;
}
void backwardClick() {
  digitalWrite(13, HIGH);
  delay(50);
  digitalWrite(13, LOW);
  lcd.clear();
  lcd.noBacklight();
  lcd.noDisplay();
  increment ++;
}
void welcome() {
  lcd.setCursor(4, 0);
  lcd.print("Set of");
  lcd.setCursor(2, 1);
  lcd.print("16/MEN/***");
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Flow Rate V1.0");
  lcd.setCursor(0, 1);
  lcd.print("Loading");
  lcd.setCursor(7, 1);
  for (int load = 0; load < 3 ; load ++) {
    lcd.print(".");
    delay(1000);
  }
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Poseidon");
  lcd.setCursor(5, 1);
  lcd.print("box");
  delay(1000);
  lcd.clear();
}
int position;
void readings() {
  int clear = Meter.getCurrentFlowrate();
  clear = 0;
  if (Meter.getCurrentFlowrate() > 0) {
    lcd.clear();
  }
  lcd.backlight();
  lcd.display();

  lcd.setCursor(0, 0);
  lcd.print("Rate:");
  lcd.setCursor(6, 0);
  lcd.print(Meter.getCurrentFlowrate());

  lcd.setCursor(10, 0);
  lcd.print("l/m");

  lcd.setCursor(0, 1);
  lcd.print("Volume:");
  lcd.setCursor(8, 1);
  lcd.print(Meter.getTotalVolume());
  Serial.println(Meter.getTotalVolume());
  if (Meter.getTotalVolume() > 9) {
    position = 13;
  } else {
    position = 12;
  }
  lcd.setCursor(position, 1);
  lcd.print("l");
  Meter.tick(period);
  delay(100);
  //  Meter.getCurrentFlowrate() = 0;
}
void charging() {

}
void volts() {
  int sensorValue = analogRead(A3);
  float voltage = sensorValue * (5.0 / 1023.0);
  int volts = map(voltage, voltageMin, voltageMax, 0, 2);
  charge = digitalRead(detect);
  Serial.println(voltage);
  // lcd.clear();
  lcd.backlight();
  lcd.display();
  lcd.setCursor(0, 0);
  lcd.print("Volt:");
  lcd.setCursor(6, 0);
  lcd.print(voltage);
  lcd.setCursor(10, 0);
  lcd.print("v");
  lcd.setCursor(0, 1);
  lcd.print("Status:");
  if (charge == HIGH) {
    lcd.setCursor(8, 1);
    lcd.print("charging");
  } else {
    switch (volts) {
      case 0:
        lcd.setCursor(8, 1);
        lcd.print("Low");
        break;
      case 1:
        lcd.setCursor(8, 1);
        lcd.print("Meduim");
        break;
      case 2:
        lcd.setCursor(8, 1);
        lcd.print("Full");
        break;
    }
  }
}

void brightness() {
  data = Serial.read();
  if (data == 'c') {
    incrementB ++;
  }
  if (incrementB > 2) {
    incrementB = 0;
  }
  if (data == 'd') {
    incrementB --;
  }
  if (incrementB < 0) {
    incrementB = 0;
  }
  //lcd.clear();
  lcd.backlight();
  lcd.display();
  int bright = map(incrementB, 0, 2, 0, 2);
  switch (bright) {
    case 0:
      x = 255;
      analogWrite(led, x);
      EEPROM.write(val2, x);
      Serial.println(EEPROM.read(0));
      lcd.setCursor(0, 1);
      lcd.print("High");
      break;
    case 1:
      x = 85;
      analogWrite(led, x);
      EEPROM.write(val2, x);
      Serial.println(EEPROM.read(0));
      lcd.setCursor(0, 1);
      lcd.print("Meduim");
      break;
    case 2:
      x = 1;
      analogWrite(led, x);
      EEPROM.write(val2, x);
      Serial.println(EEPROM.read(0));
      lcd.setCursor(0, 1);
      lcd.print("Low");
      break;
      delay(50);
  }
  Serial.println(incrementB);
  Serial.println(x);
  // analogWrite(led, x);
  lcd.setCursor(0, 0);
  lcd.print("Brightness:");
}
int count;
void controlPower() {
  EEPROM.write(0, x);
  data = Serial.read();
  if (data == 'a') {
    increment ++;
  }
  if (increment > 2) {
    increment = 0;
  }
  if (data == 'b') {
    increment --;
  }
  if (increment < 0) {
    increment = 0;
  }
  int menuMovement = map(increment, 0, 2, 0, 2);
  switch (menuMovement) {
    case 0:
      readings();
      break;
    case 1:
      volts();
      break;
    case 2:
      brightness();
      break;
  }

  charging();

}
// the pin that the LED is attached to

void loop() {
  forward.tick();
  backward.tick();
  power.tick();
  //count ++;
  // EEPROM.write(0, count);
  // Serial.println(EEPROM.read(0));
  //Serial.println(count);
  //analogWrite(led, x);
  controlPower();
  if (buttonState != lastButtonState) {
    // if the state has changed, increment the counter
    if (buttonState == HIGH) {
      // if the current state is HIGH then the button went from off to on:
      buttonPushCounter++;
      Serial.println("on");
      Serial.print("number of button pushes: ");
      Serial.println(buttonPushCounter);
    } else {
      // if the current state is LOW then the button went from on to off:
      Serial.println("off");
    }
    // Delay a little bit to avoid bouncing
    delay(50);
  }
  // save the current state as the last state, for next time through the loop
  lastButtonState = buttonState;
  if (buttonPushCounter % 3 == 0) {

    Serial.println("on");
  } else {
    Serial.println("off");
    lcd.noDisplay();
  }

}
