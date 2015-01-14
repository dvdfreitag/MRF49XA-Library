/*
 *  Registers.h
 *  MRF49XA
 *
 *  Created by William Dillon on 3/7/14.
 *  Copyright (c) 2014 Oregon State University (COAS). All rights reserved.
 *
 *  Modified by David Freitag 1/13/15
 *
 */

#ifndef REGISTERS_H
#define REGISTERS_H

#include <Arduino.h>
#include "Modes.h"

// These registers are fully under user control
/*
uint16_t EEMEM afcreg      = 0xC4F7;
uint16_t EEMEM txcreg      = 0x9810;
uint16_t EEMEM cfsreg      = 0xA348;
uint16_t EEMEM rxcreg      = 0x94C0;
uint16_t EEMEM bbfcreg     = 0xC2AC;
uint16_t EEMEM fiforstreg  = 0xCA81;
uint16_t EEMEM synbreg     = 0xCED4;
uint16_t EEMEM drsreg      = 0xC623;
uint16_t EEMEM pllcreg     = 0xCC77;
*/

#define garbage    0x0000
#define bootMode   0x0002
#define afcreg     0x0004
#define txcreg     0x0006
#define cfsreg     0x0008
#define rxcreg     0x000A
#define bbfcreg    0x000C
#define fiforstreg 0x000E
#define synbreg    0x0010
#define drsreg     0x0012
#define pllcreg    0x0014

class Registers_t
{
public:
	void ApplySavedRegisters(void);
	void PrintSavedRegisters(void);
	void SetRegisterValue(uint8_t index, uint16_t value);

	enum device_mode GetBootState(void);
	void SetBootState(enum device_mode mode);
private:
	void SetEEPROMDefaults(void);
};

extern Registers_t Registers;

 #endif