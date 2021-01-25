/*
  RotaryEncoder.cpp - Library for interfacing rotary encoders with STM32duino
  Created by datenpirat, January 8, 2019.
  Released into the public domain.
*/
#include "Arduino.h"
#include "RotaryEncoder.h"

static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};


boolean RotaryEncoderA_set = false;            
boolean RotaryEncoderB_set = false;

uint8_t DEBOUNCE_TIME_ENCODER = 1;
uint16_t DEBOUNCE_TIME_BUTTON  = 5;

RotaryEncoder::RotaryEncoder(int PIN1, int PIN2, int PINBTN)
{
  pinMode(PIN1, INPUT_PULLUP);
  pinMode(PIN2, INPUT_PULLUP);
  pinMode(PINBTN, INPUT_PULLUP);
  _pin1 = PIN1;
  _pin2 = PIN2;
  _pinBtn = PINBTN;
  buttonIsPressing = 0;
  fallTime = millis();
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
      if ((store&0xff)==0x2b) return 1;
      if ((store&0xff)==0x17) return -1;
    }
    return 0;
  }
   else{
     return encoderPos;
   }
}

int8_t RotaryEncoder::HasEncoderChanged(){
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

uint8_t RotaryEncoder::HasButtonChanged(){
  int8_t result = buttonChanged;
  buttonChanged = 0;
  return result;
}


uint8_t RotaryEncoder::GetButtonDown(){
  return GetButtonIsPressing();
}

uint8_t RotaryEncoder::GetButtonUp(){
  return !GetButtonIsPressing();
}

uint8_t RotaryEncoder::GetButtonIsPressing(){
  return buttonIsPressing;
}

uint32_t RotaryEncoder::GetButtonPressedTime(){
    if(fallTime > riseTime){
      return(fallTime - riseTime);
    }
    else{
      return 0;
    }
}


void RotaryEncoder::UseInterrupts(){
    useInterrupts = 1;
    
    ihp1.pin = _pin1;
    ihp1.p_val = &encoderPos;
    ihp1.p_tick = &lastTick;

    ihp2.pin = _pin2;
    ihp2.p_val = &encoderPos;
    ihp2.p_tick = &lastTick;

    btnhp.pin = _pinBtn;
    btnhp.p_lastRising = &lastRising;
    btnhp.p_riseTime = &riseTime;
    btnhp.p_fallTime = &fallTime;
    btnhp.p_buttonChanged = &buttonChanged;
    btnhp.p_buttonIsPressing = &buttonIsPressing;

    attachInterrupt(_pin1,IRQPIN1,(void*)&ihp1,CHANGE);
    attachInterrupt(_pin2,IRQPIN2,(void*)&ihp2,CHANGE);
    attachInterrupt(_pinBtn,IRQBTN,(void*)&btnhp,CHANGE);
}


void RotaryEncoder::IRQPIN1(void *p){

  IRQHandlerParameters *_p = (IRQHandlerParameters *)p;
    if(gpio_read_bit(PIN_MAP[_p->pin].gpio_device, PIN_MAP[_p->pin].gpio_bit) != RotaryEncoderA_set) {
      RotaryEncoderA_set = !RotaryEncoderA_set;
      if ( RotaryEncoderA_set && !RotaryEncoderB_set ) {
        if((millis() - *_p->p_tick) > DEBOUNCE_TIME_ENCODER){
          *_p->p_val += 1;
        }
      }
  *_p->p_tick = millis();
  }
}

void RotaryEncoder::IRQPIN2(void *p){

  IRQHandlerParameters *_p = (IRQHandlerParameters *)p;
    if(gpio_read_bit(PIN_MAP[_p->pin].gpio_device, PIN_MAP[_p->pin].gpio_bit) != RotaryEncoderB_set) {
      RotaryEncoderB_set = !RotaryEncoderB_set;
      if( RotaryEncoderB_set && !RotaryEncoderA_set ) {
        if((millis() - *_p->p_tick) > DEBOUNCE_TIME_ENCODER){
          *_p->p_val -= 1;
        }
      }
  *_p->p_tick = millis();
  }
}

void RotaryEncoder::IRQBTN(void* p){

  BtnIRQHandlerParameters *_p = (BtnIRQHandlerParameters *)p;

  if((millis() - *_p->p_lastRising) > DEBOUNCE_TIME_BUTTON){

    uint32_t c = gpio_read_bit(PIN_MAP[_p->pin].gpio_device, PIN_MAP[_p->pin].gpio_bit) ? HIGH : LOW;

    if(c == LOW){ // RISING, PULLUP!
      *_p->p_riseTime = millis();
      *_p->p_buttonIsPressing = 1;
      *_p->p_buttonChanged = 1;
    }

    else if(c == HIGH){  // FALLING, PULLUP!
      *_p->p_fallTime = millis();
      *_p->p_buttonChanged = 1;
      *_p->p_buttonIsPressing = 0;
    }
    
  }

}

