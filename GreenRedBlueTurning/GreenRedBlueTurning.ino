//Uses modified code from Joop Brooking

#include<Wire.h>
#include <FastLED.h>
#define LED_PIN     5
#define NUM_LEDS    60

const int MPU_addr=0x68;
CRGB leds[NUM_LEDS];
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ,piezoVal;
int pulse = 30;
int count = 0;

void setup(){
  setupWire();
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);
  Serial.begin(9600);
}

void loop(){
  updateWire();
  //The Faster(larger AcY) you go the more LEDS turn on
  int y = map(AcY, -4000, 4000, 0, NUM_LEDS/2);
  for(int i = 0; i < NUM_LEDS/2; i++) {
    int k = i + 30;
    if(i <= y) {
      //Blue if going straight, Green on inside of turn, Red on outside of turn
      if(AcX >= 750)  {
        leds[i] = CRGB ( 255, 0, 0);
        leds[k] = CRGB ( 0, 255, 0);
      }
      else if(AcX <= -750) {
        leds[i] = CRGB ( 0, 255, 0);
        leds[k] = CRGB ( 255, 0, 0);
      }
      else  {
        leds[i] = CRGB ( 0, 0, 255);
        leds[k] = CRGB ( 0, 0, 255);
      }
    }
    if(i == pulse && count < 3)  {
      leds[i] = CRGB ( 255, 0, 255);
      leds[k] = CRGB ( 255, 0, 255);
      pulse++;
      count++;
    }
    if(i > y) {
      leds[i] = CRGB ( 0, 0, 0);
      leds[k] = CRGB ( 0, 0, 0);
    }
  }
  count = 0;
  if(pulse < 30)  {
    pulse--;
  }
  FastLED.show();
  delay(50);
}

void setupWire()  {
  Wire.begin();
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);
  Wire.write(0x00);
  Wire.endTransmission(true);

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1B);
  Wire.write(0x08);
  Wire.endTransmission();

  Wire.beginTransmission(MPU_addr);
  Wire.write(0x1C);
  Wire.write(0x10);
  Wire.endTransmission();
}

void updateWire() {
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);
  AcX=Wire.read()<<8|Wire.read();
  AcY=Wire.read()<<8|Wire.read();
  AcZ=Wire.read()<<8|Wire.read();
  Tmp=Wire.read()<<8|Wire.read();
  GyX=Wire.read()<<8|Wire.read();
  GyY=Wire.read()<<8|Wire.read();
  GyZ=Wire.read()<<8|Wire.read();
  piezoVal = analogRead(A2);
  Serial.print("AcX = "); Serial.print(AcX);
  Serial.print(" | AcY = "); Serial.print(AcY);
  Serial.print(" | AcZ = "); Serial.print(AcZ);
  Serial.print(" | GyX = "); Serial.print(GyX);
  Serial.print(" | Piezo = "); Serial.println(piezoVal);
  if(piezoVal > 3) {
    pulse = 0;
  }
}

