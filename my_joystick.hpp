//
// Created by wsh on 2022/2/1.
//

#ifndef TESTJOYSTICK_MY_JOYSTICK_HPP
#define TESTJOYSTICK_MY_JOYSTICK_HPP

#include <stdint.h>

/// buttons. 16 bit ( 2 Byte )
typedef union
{
    struct
    {
        uint8_t RB: 1;     // unitree_joystick: R1
        uint8_t LB: 1;     // unitree_joystick: L1
        uint8_t start: 1;
        uint8_t select: 1;

        uint8_t RT: 1;     // unitree_joystick: R2
        uint8_t LT: 1;     // unitree_joystick: L2
        uint8_t M2: 1;     // unitree_joystick: F1
        uint8_t C: 1;      // unitree_joystick: F2

        uint8_t A: 1;
        uint8_t B: 1;
        uint8_t X: 1;
        uint8_t Y: 1;

        uint8_t up: 1;     // axis YY-
        uint8_t right: 1;  // axis XX+
        uint8_t down: 1;   // axis YY+
        uint8_t left: 1;   // axis XX-
    } components;
    uint16_t value;
} jsButtonUnion;

/// joystick data (button & axis). 40 Byte (now used 28 Byte)
typedef struct {
    uint8_t head[2];
    jsButtonUnion button;
    float LX;
    float RX;
    float RY;
    float LT;
    float LY;
    float RT;

    uint8_t idle[12];
} jsDataStruct;

#endif //TESTJOYSTICK_MY_JOYSTICK_HPP
