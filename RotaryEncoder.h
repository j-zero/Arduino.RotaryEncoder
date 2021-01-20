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
    int8_t Read();
    void UseInterrupts();
    void PrintEncoderPosition();
    int8_t GetEncoderValue();
    int8_t HasChanged();

    uint8_t GetButtonPressed();
    uint8_t GetButtonIsPressing();
    unsigned long GetButtonPressedTime();

  private:

    typedef struct {
      int pin;
      volatile int8_t *val;
      uint8_t *rot;
      unsigned long tick;
    } IRQHandlerParameters;

    typedef struct {
      int pin;
      volatile unsigned long *lastRising = 0, *riseTime = 0, *fallTime = 0, *dblClickTime = 0;
      uint8_t *buttonPressed = 0, *buttonIsPressing = 0;
    } BtnIRQHandlerParameters;

    uint8_t DEBOUNCE_TIME = 20;
    uint8_t DOUBLECLICK_TIME = 200;

    int _pin1;
    int _pin2;
    int _pinBtn;

    uint8_t useInterrupts = 0;
    uint8_t prevNextCode = 0;
    uint16_t store=0;
    static void IRQPIN1(void* p);
    static void IRQPIN2(void* p);
    static void IRQBTN(void* p);
    volatile int8_t encoderPos = 0;
    int8_t lastReportedPos = 1;
    uint8_t rotating;
    unsigned long lastTick;
    IRQHandlerParameters ihp1;
    IRQHandlerParameters ihp2;
    BtnIRQHandlerParameters BtnHp;

    volatile unsigned long lastRising = 0, riseTime = 0, fallTime = 0, dblClickTime = 0;
    uint8_t buttonPressed = 0, buttonIsPressing = 0;

};

#endif