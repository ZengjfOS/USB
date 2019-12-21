//-----------------------------------------------------------------------------
//  File:    int0.c
//  Contents:  ISR for INT0# pin, used by peripheral to issue ZERO LENGTH PACKET
//  Copyright (c) 2001 Cypress Semiconductor All rights reserved
//-----------------------------------------------------------------------------
#include "fx2.h"
#include "fx2regs.h"

extern BOOL zerolenpkt;

void int0( void ) interrupt 0
{ // processor vectors here when peripheral asserts zerolenpkt
  zerolenpkt = 1;
  EX0 = 0;                      // disable INT0# pin ISR

  // foreground clears this flag and (re)enables ISR...
  // ...an idle system will service this event in ~8usec
  // ...a busy system could take ~80usec or so...
}