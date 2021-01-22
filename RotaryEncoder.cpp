/*
  RotaryEncoder.cpp - Library for interfacing rotary encoders with STM32duino
  Created by datenpirat, January 8, 2019.
  Released into the public domain.
*/
#include "Arduino.h"
#include "RotaryEncoder.h"

static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};

uint8_t debounceTime = 1;
boolean RotaryEncoderA_set = false;            
boolean RotaryEncoderB_set = false;
//volatile uint32_t lastTick = 0;
uint32_t lastRising = 0;

RotaryEncoder::RotaryEncoder(int PIN1, int PIN2, int PINBTN)
{
  pinMode(PIN1, INPUT_PULLUP);
  pinMode(PIN2, INPUT_PULLUP);
  pinMode(PINBTN, INPUT_PULLUP);
  _pin1 = PIN1;
  _pin2 = PIN2;
  _pinBtn = PINBTN;
  rotating = false;
  buttonIsPressing = 0;
  //lastTick = micros();
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

int8_t RotaryEncoder::ReadAB(int pin1,int pin2) {
    noInterrupts();
    // A vald CW or  CCW move returns 1, invalid returns 0.
    prevNextCode <<= 2;
    if(gpio_read_bit(PIN_MAP[pin1].gpio_device, PIN_MAP[pin1].gpio_bit)) prevNextCode |= 0x02;
    if(gpio_read_bit(PIN_MAP[pin2].gpio_device, PIN_MAP[pin2].gpio_bit)) prevNextCode |= 0x01;
    prevNextCode &= 0x0f;
    // If valid then store as 16 bit data.
    if  (rot_enc_table[prevNextCode] ) {
      store <<= 4;
      store |= prevNextCode;
      if ((store&0xff)==0x2b) return 1;
      if ((store&0xff)==0x17) return -1;
    }
    return 0;
    interrupts();
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

uint8_t RotaryEncoder::GetButtonPressed(){
  int8_t result = buttonPressed;
  buttonPressed = 0;
  return result;
}

uint8_t RotaryEncoder::GetButtonIsPressing(){
  return buttonIsPressing;
}

unsigned long RotaryEncoder::GetButtonPressedTime(){
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
    ihp2.pin = _pin2;
    ihp1.val = &encoderPos;
    ihp2.val = &encoderPos;
    ihp1.rot = &rotating;
    ihp2.rot = &rotating;
    ihp1.tick = &lastTick;
    ihp2.tick = &lastTick;
    
    BtnHp.pin = _pinBtn;
    BtnHp.lastRising = &lastRising;
    BtnHp.riseTime = &riseTime;
    BtnHp.fallTime = &fallTime;
    BtnHp.dblClickTime = &dblClickTime;
    BtnHp.buttonPressed = &buttonPressed;
    BtnHp.buttonIsPressing = &buttonIsPressing;
    /*
      volatile unsigned long *lastRising = 0, *riseTime = 0, *fallTime = 0, *dblClickTime = 0;
      uint8_t *buttonPressed = 0, *buttonIsPressing = 0;
    */
    attachInterrupt(_pin1,IRQPIN1,(void*)&ihp1,CHANGE);
    attachInterrupt(_pin2,IRQPIN2,(void*)&ihp2,CHANGE);
    attachInterrupt(_pinBtn,IRQBTN,(void*)&BtnHp,CHANGE);
}


void RotaryEncoder::IRQPIN1(void *p){
  IRQHandlerParameters *_p = (IRQHandlerParameters *)p;
  if((millis() - *_p->tick) > debounceTime){
    if(gpio_read_bit(PIN_MAP[_p->pin].gpio_device, PIN_MAP[_p->pin].gpio_bit) != RotaryEncoderA_set ) {
      RotaryEncoderA_set = !RotaryEncoderA_set;
      if ( RotaryEncoderA_set && !RotaryEncoderB_set ) {
          *_p->val += 1;
      }
      *_p->rot = false;  // no more debouncing until loop() hits again
    }
    *_p->tick = millis();
  }
}

void RotaryEncoder::IRQPIN2(void *p){

  IRQHandlerParameters *_p = (IRQHandlerParameters *)p;
  if((millis() - *_p->tick) > debounceTime){
    if(gpio_read_bit(PIN_MAP[_p->pin].gpio_device, PIN_MAP[_p->pin].gpio_bit) != RotaryEncoderB_set ) {
      RotaryEncoderB_set = !RotaryEncoderB_set;
      if( RotaryEncoderB_set && !RotaryEncoderA_set ) {
          *_p->val -= 1;
      }
      *_p->rot = false;  // no more debouncing until loop() hits again
      
    }
  *_p->tick = millis();

  }
}

void RotaryEncoder::IRQBTN(void* p){

  BtnIRQHandlerParameters *_p = (BtnIRQHandlerParameters *)p;

    if((millis() - *_p->lastRising) > 20){
    /*
      int c = gpio_read_bit(PIN_MAP[_p->pin].gpio_device, PIN_MAP[_p->pin].gpio_bit);
      I HAVE NO IDEA WHY digitalRead(1) works, but gpio_bit doesn't!
    */
    int c = digitalRead(_p->pin);
    //int c = gpio_read_bit(PIN_MAP[_p->pin].gpio_device, PIN_MAP[_p->pin].gpio_bit);
    if(c == LOW){ // RISING, PULLUP!
      *_p->riseTime = millis();
      *_p->buttonIsPressing = 1;
      *_p->buttonPressed = 1;
      if(*_p->dblClickTime != 0 && *_p->riseTime > *_p->dblClickTime && (*_p->riseTime - *_p->dblClickTime) < 200)
        *_p->buttonPressed = 2;
    }
    else if(c == HIGH){  // FALLING, PULLUP!
      *_p->fallTime = millis();
      *_p->dblClickTime = millis();
      *_p->buttonPressed = 1;
      *_p->buttonIsPressing = 0;
    }
    
      *_p->lastRising=millis();
  }

}

