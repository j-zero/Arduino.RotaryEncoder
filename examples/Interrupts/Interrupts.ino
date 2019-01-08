/*
 * Tested on BluePill Boards
 * 
 */
#include <RotaryEncoder.h>

RotaryEncoder encoder1(0,1);
RotaryEncoder encoder2(PB3,PB4);

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
    if(pos1){
      Serial.print("encoder1 changed ");Serial.print(pos1);Serial.println(" steps");
    }
     if(pos2){
      Serial.print("encoder2 changed ");Serial.print(pos2);Serial.println(" steps");
    }

    delay(200);

    
}