/*
  RotaryEncoder.cpp - Library for interfacing rotary encoders with STM32duino
  Created by datenpirat, January 8, 2019.
  Released into the public domain.
*/

//#define MIN_DIFF 20

#include "Arduino.h"
#include "RotaryEncoder.h"

//static RotaryEncoder *instance;
static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};

boolean RotaryEncoderA_set = false;            
boolean RotaryEncoderB_set = false;

RotaryEncoder::RotaryEncoder(int PIN1, int PIN2)
{
  pinMode(PIN1, INPUT_PULLUP);
  pinMode(PIN2, INPUT_PULLUP);
  _pin1 = PIN1;
  _pin2 = PIN2;
  rotating = false;

} 


int8_t RotaryEncoder::Read() {
  if(!useInterrupts){
    // A vald CW or  CCW move returns 1, invalid returns 0.
    prevNextCode <<= 2;
    if(gpio_read_bit(PIN_MAP[_pin1].gpio_device, PIN_MAP[_pin1].gpio_bit)) prevNextCode |= 0x02;
    if(gpio_read_bit(PIN_MAP[_pin2].gpio_device, PIN_MAP[_pin2].gpio_bit)) prevNextCode |= 0x01;
    prevNextCode &= 0x0f;
    // If valid then store as 16 bit data.
    if  (rot_enc_table[prevNextCode] ) {
      store <<= 4;
      store |= prevNextCode;
      if ((store&0xff)==0x2b) return RIGHT;
      if ((store&0xff)==0x17) return LEFT;
    }
    return 0;
  }
   else{
     return encoderPos;
   }
}

void RotaryEncoder::PrintEncoderPosition(){
  Serial.println(encoderPos);
}

int8_t RotaryEncoder::HasChanged(){
  rotating = true;
  int8_t result = 0;
  if (lastReportedPos != encoderPos) {
       result = encoderPos - lastReportedPos;
       lastReportedPos = encoderPos;
  }
  return result;
}

int8_t RotaryEncoder::GetEncoderValue(){
  return encoderPos;
}

void RotaryEncoder::UseInterrupts(){
    useInterrupts = 1;
    
    ihp1.pin = _pin1;
    ihp2.pin = _pin2;
    ihp1.val = &encoderPos;
    ihp2.val = &encoderPos;
    ihp1.rot = &rotating;
    ihp2.rot = &rotating;

    attachInterrupt(_pin1,Pin1Int,(void*)&ihp1,CHANGE);
    attachInterrupt(_pin2,Pin2Int,(void*)&ihp2,CHANGE);
}

void RotaryEncoder::Pin1Int(void *p){
  IRQHandlerParameters *_p = (IRQHandlerParameters *)p;
  if ( _p->rot ) delay (1);  // wait a little until the bouncing is done
  if( gpio_read_bit(PIN_MAP[_p->pin].gpio_device, PIN_MAP[_p->pin].gpio_bit) != RotaryEncoderA_set ) {  // debounce once more
    RotaryEncoderA_set = !RotaryEncoderA_set;
    // adjust counter + if A leads B
    if ( RotaryEncoderA_set && !RotaryEncoderB_set ) {
      *_p->val += 1;
    }
    *_p->rot = false;  // no more debouncing until loop() hits again
  }
}

void RotaryEncoder::Pin2Int(void *p){
  IRQHandlerParameters *_p = (IRQHandlerParameters *)p;
  if ( _p->rot ) delay (1);
  if( gpio_read_bit(PIN_MAP[_p->pin].gpio_device, PIN_MAP[_p->pin].gpio_bit) != RotaryEncoderB_set ) {
    RotaryEncoderB_set = !RotaryEncoderB_set;
    //  adjust counter - 1 if B leads A
    if( RotaryEncoderB_set && !RotaryEncoderA_set ) {
      *_p->val -= 1;
    }
    *_p->rot = false;
  }
}