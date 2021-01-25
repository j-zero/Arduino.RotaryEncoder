/*
  RotaryEncoder.h - Library for interfacing rotary encoders with STM32duino
  Created by datenpirat, January 8, 2019.
  Released into the public domain.
*/
#ifndef RotaryEncoder_h
#define RotaryEncoder_h

#include "Arduino.h"

class RotaryEncoder
{
  public:
    RotaryEncoder(int PIN1, int PIN2, int PINBTN);
    void UseInterrupts();
    int8_t Read();

    int8_t GetEncoderValue();

    uint8_t HasButtonChanged();
    int8_t  HasEncoderChanged();


    uint8_t GetButtonIsPressing();
    uint32_t GetButtonPressedTime();
    uint8_t GetButtonPressedCount();
    uint8_t GetButtonDown();
    uint8_t GetButtonUp();

  private:
    typedef struct {
      uint8_t pin;
      volatile int8_t *p_val;
      volatile uint32_t *p_tick;
    } IRQHandlerParameters;

    typedef struct {
      uint8_t pin;
      volatile uint32_t *p_lastRising = 0, *p_riseTime = 0, *p_fallTime = 0;
      uint8_t *p_buttonChanged = 0, *p_buttonIsPressing = 0;
    } BtnIRQHandlerParameters;


    uint8_t _pin1;
    uint8_t _pin2;
    uint8_t _pinBtn;

    uint8_t useInterrupts = 0;
    uint8_t prevNextCode = 0;
    uint16_t store=0;
    
    static void IRQPIN1(void* p);
    static void IRQPIN2(void* p);
    static void IRQBTN(void* p);

    volatile int8_t encoderPos = 0;
    int8_t lastReportedPos = 1;
    uint8_t rotating;
    uint32_t lastTick;

    IRQHandlerParameters ihp1;
    IRQHandlerParameters ihp2;
    BtnIRQHandlerParameters btnhp;

    volatile uint32_t lastRising = 0, riseTime = 0, fallTime = 0;
    uint8_t buttonChanged = 0, buttonIsPressing = 0;

};

#endif