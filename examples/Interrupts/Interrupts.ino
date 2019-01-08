/*
 * Tested on BluePill Boards
 * 
 */
#include <RotaryEncoder.h>

RotaryEncoder encoder1(PA0,PA1,PA2);
RotaryEncoder encoder2(PB3,PB4,PB5);

void setup() {
    Serial.begin (115200);
    while (!Serial){ digitalWrite(PC13,!digitalRead(PC13)); delay(100); }
    Serial.println("KY-040 Test");
    encoder1.UseInterrupts();
    encoder2.UseInterrupts();
}

void loop() {
  int8_t pos1 = encoder1.HasChanged();
  int8_t pos2 = encoder2.HasChanged();
  int8_t btn1Pressed = encoder1.GetButtonPressed();
  int8_t btn2Pressed = encoder1.GetButtonPressed();
  
  if(pos1){
    Serial.print("encoder1 changed ");Serial.print(pos1);Serial.println(" steps");
  }
   if(pos2){
    Serial.print("encoder2 changed ");Serial.print(pos2);Serial.println(" steps");
  }
  if(btn1Pressed){
    Serial.print("encoder1 pressed ");Serial.print(encoder1.GetButtonPressedTime());Serial.println(" millis");
  }
  if(btn2Pressed){
    Serial.print("encoder2 pressed ");Serial.print(encoder2.GetButtonPressedTime());Serial.println(" millis");
  }
  delay(200);   
}