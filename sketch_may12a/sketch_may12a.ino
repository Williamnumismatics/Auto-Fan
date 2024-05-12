#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <IRremote.h>

int ledPin = A5;

int poPin = A4;
int poPinv = 0;

int buttonBpin = 9;
int buttonApin = 8;

int buttonCpin = 6;

int pirPin = 7;
int pirValue;

String thing;

int receiver = 11;

byte leds = 0;

bool led = false;

const int MPU_addr=0x68;  // I2C address of the MPU-6050
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;

IRrecv irrecv(receiver);
uint32_t last_decodedRawData = 0;
/*-----( Function )-----*/
void translateIR() // takes action based on IR code received
{
  // Check if it is a repeat IR code
  if (irrecv.decodedIRData.flags)
  {
    //set the current decodedRawData to the last decodedRawData
    irrecv.decodedIRData.decodedRawData = last_decodedRawData;
    Serial.println("REPEAT!");
  } else
  {
    //output the IR code on the serial monitor
    Serial.print("IR code:0x");
    Serial.println(irrecv.decodedIRData.decodedRawData, HEX);
  }
  //map the IR code to the remote key
  switch (irrecv.decodedIRData.decodedRawData)
  {
    case 0x5DA2FF00: analogWrite(ledPin, 255); break;
    case 0x1DE2FF00: analogWrite(ledPin, 0); break;
    case 0xCF30FF00: analogWrite(ledPin, 50); break;
    case 0xE718FF00: analogWrite(ledPin, 100);    break;
    case 0x857AFF00: analogWrite(ledPin, 150);    break;
    case 0xEF10FF00: analogWrite(ledPin, 200);   break;
    case 0xC738FF00: analogWrite(ledPin, 255);    break;
    default:
      Serial.println(" other button   ");
  }// End Case
  //store the last decodedRawData
  last_decodedRawData = irrecv.decodedIRData.decodedRawData;
  delay(500); // Do not get immediate repeat
} //END translateIR

  LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{

  lcd.init();
  lcd.backlight();

  pinMode(ledPin, OUTPUT);
  pinMode(pirPin, INPUT);
  pinMode(buttonApin, INPUT_PULLUP);
  pinMode(buttonBpin, INPUT_PULLUP);
  pinMode(buttonCpin, INPUT_PULLUP);
  pinMode(poPin, INPUT);

  Serial.begin(9600);
  Serial.println("Hehe");
  irrecv.enableIRIn();


  poPinv = analogRead(poPin);

  // lcd.print("  Welcome to your ");
  // lcd.setCursor(0, 2);
  // lcd.print("   personal fan");
  // delay(5000);
  // lcd.clear();
  // lcd.print(" A quick tutorial:");
  // delay(5000);
  // lcd.clear();
  // lcd.print(" The Button on the    left turns on");
  // delay(5000);
  // lcd.clear();
  // lcd.print(" The button on the    right turns off");
  // delay(5000);
  // lcd.clear();
  // lcd.print("    There is also ");
  // lcd.setCursor(0, 1);
  // lcd.print("      a sensor");
  // lcd.setCursor(0, 2);
  // lcd.print("    To sense your");
  // lcd.setCursor(0, 3);
  // lcd.print("      presence");
  // delay(5000);
  // lcd.clear();
  // lcd.print("Thats it! Enjoy!");
  // delay(10000);
  // lcd.clear();
  // lcd.noBacklight();


}

void loop()
{
  if (digitalRead(buttonApin) == LOW)
  {
    analogWrite(ledPin, 255);
  }
  if (digitalRead(buttonBpin) == LOW)
  {
    analogWrite(ledPin, 0);
  }

  pirValue = digitalRead(pirPin);
  if (pirValue == 1 ) {
    analogWrite(ledPin, 255);
  }
  delay(1);

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // request a total of 14 registers


  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  Serial.println(Tmp);


  if (Serial.available()) {
    thing = Serial.readStringUntil('\n');
    thing.trim();
    lcd.print("")
  }
  if (thing.equals("On")) {
    analogWrite(ledPin, 255);
    Serial.println("Command:");
    Serial.print(thing);
  } else if ( thing.equals ("Off")) {
    analogWrite(ledPin, 0);
    Serial.println("Command:");
    Serial.print(thing);
  }
  delay(1);

  if (irrecv.decode()) // have we received an IR signal?
  {
    translateIR();
    irrecv.resume(); //  receive the next value
  }
  delay(1);

  // if (digitalRead(buttonBpin) == LOW){
  //   analogWrite(ledPin, 0);
  // }
  if (Tmp >= 1180 && digitalRead(buttonBpin) != LOW){
    analogWrite(ledPin, 255);
  }
   else if (digitalRead(buttonBpin) == LOW)
  {
    analogWrite(ledPin, 0);
  }


  
  // else{
  //   digitalWrite(ledPin, 0);
  // }
  delay(1);
  
  
}
// button to no backlight or backlight the lcd screen. 
// when you press a button, you display it on the screen e.g.
//  if you press on, it says on of motion edetected,
//   it says motion detected. turn off with the exess button.
//    use input_pullup