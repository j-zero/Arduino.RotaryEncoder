/*
  RotaryEncoder.h - Library for interfacing rotary encoders with STM32duino
  Created by datenpirat, January 7, 2019.
  Released into the public domain.
*/
#ifndef RotaryEncoder_h
#define RotaryEncoder_h

#include "Arduino.h"

class RotaryEncoder
{
  public:
    RotaryEncoder(int PIN1, int PIN2);
    int8_t Read();
    void UseInterrupts();
    void PrintEncoderPosition();
    int8_t GetEncoderValue();
    int8_t HasChanged();
    static const int8_t RIGHT = -1;
    static const int8_t LEFT = 1;
    static const int8_t NONE = 0;
  private:
    typedef struct {
      int pin;
      volatile int8_t *val;
      uint8_t *rot;
    }IRQHandlerParameters;
    int _pin1;
    int _pin2;
    uint8_t useInterrupts = 0;
    uint8_t prevNextCode = 0;
    uint16_t store=0;
    static void Pin1Int(void* p);
    static void Pin2Int(void* p);
    volatile int8_t encoderPos = 0;  // a counter for the dial
    int8_t lastReportedPos = 1;   // change management
    uint8_t rotating;      // debounce management
    IRQHandlerParameters ihp1;
    IRQHandlerParameters ihp2;
};

#endif