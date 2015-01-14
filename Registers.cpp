/*
 *  Registers.cpp
 *  MRF49XA
 *
 *  Created by William Dillon on 3/7/14.
 *  Copyright (c) 2014 Oregon State University (COAS). All rights reserved.
 *
 *  Modified by David Freitag 1/13/15
 *
 */

#include <Arduino.h>
#include <EEPROM.h>
#include "Registers.h"
#include "MRF49XA.h"
#include "Hamming.h"
#include "Modes.h"

Registers_t Registers = Registers_t();

void Registers_t::ApplySavedRegisters(void)
{
	MRF49XA.SetRegister(EEPROM.read(afcreg));
    MRF49XA.SetRegister(EEPROM.read(txcreg));
    MRF49XA.SetRegister(EEPROM.read(cfsreg));
    MRF49XA.SetRegister(EEPROM.read(rxcreg));
    MRF49XA.SetRegister(EEPROM.read(bbfcreg));
    MRF49XA.SetRegister(EEPROM.read(fiforstreg));
    MRF49XA.SetRegister(EEPROM.read(synbreg));
    MRF49XA.SetRegister(EEPROM.read(drsreg));
    MRF49XA.SetRegister(EEPROM.read(pllcreg));
}

const char afcregString[]     PROGMEM = "\n\r0) AFCREG:     ";
const char txcregString[]     PROGMEM = "\n\r1) TXCREG:     ";
const char cfsregString[]     PROGMEM = "\n\r2) CFSREG:     ";
const char rxcregString[]     PROGMEM = "\n\r3) RXCREG:     ";
const char bbfcregString[]    PROGMEM = "\n\r4) BBFCREG:    ";
const char fiforstregString[] PROGMEM = "\n\r5) FIFORSTREG: ";
const char synbregString[]    PROGMEM = "\n\r6) SYNBREG:    ";
const char drsregString[]     PROGMEM = "\n\r7) DRSREG:     ";
const char pllcregString[]    PROGMEM = "\n\r8) PLLCREG:    ";

void Registers_t::PrintSavedRegisters(void)
{
	if (Serial)
	{
		Serial.print(afcregString);
    	Serial.print(afcreg, HEX);
    	Serial.print(txcregString);
    	Serial.print(txcreg, HEX);
    	Serial.print(cfsregString);
    	Serial.print(cfsreg, HEX);
    	Serial.print(rxcregString);
    	Serial.print(rxcreg, HEX);
    	Serial.print(bbfcregString);
    	Serial.print(bbfcreg, HEX);
    	Serial.print(fiforstregString);
    	Serial.print(fiforstreg, HEX);
    	Serial.print(synbregString);
    	Serial.print(synbreg, HEX);
    	Serial.print(drsregString);
    	Serial.print(drsreg, HEX);
    	Serial.print(pllcregString);
    	Serial.print(pllcreg, HEX);
	}
}

void Registers_t::SetRegisterValue(uint8_t index, uint16_t value)
{
	switch (index) {
        case 0:
            EEPROM.write(afcreg, value);
            MRF49XA.SetRegister(value);
            break;
        case 1:
            EEPROM.write(txcreg, value);
            MRF49XA.SetRegister(value);
            break;
        case 2:
            EEPROM.write(cfsreg, value);
            MRF49XA.SetRegister(value);
            break;
        case 3:
            // RXCREG bit FINTDIO must be set
            EEPROM.write(rxcreg, value | MRF_FINTDIO);
            MRF49XA.SetRegister(value | MRF_FINTDIO);
            break;
        case 4:
            EEPROM.write(bbfcreg, value);
            MRF49XA.SetRegister(value);
            break;
        case 5:
            EEPROM.write(fiforstreg, value);
            MRF49XA.SetRegister(value);
            break;
        case 6:
            EEPROM.write(synbreg, value);
            MRF49XA.SetRegister(value);
            break;
        case 7:
            EEPROM.write(drsreg, value);
            MRF49XA.SetRegister(value);
            break;
        case 8:
            EEPROM.write(pllcreg, value);
            MRF49XA.SetRegister(value);
            break;
        default:
            return;
    }
    
    // The transciever should be reset after register setting
    MRF49XA.Reset();
}

enum device_mode Registers_t::GetBootState(void)
{
	uint8_t mode = Hamming.DecodeByte(EEPROM.read(bootMode));

    switch(mode) {
        case MODE_SERIAL:
        case MODE_SERIAL_ECC:
            return (enum device_mode)mode;
        default:
            SetEEPROMDefaults();
            return MODE_SERIAL;
    }
}

void Registers_t::SetBootState(enum device_mode mode)
{
	EEPROM.write(bootMode, Hamming.EncodeByte((uint8_t)mode));
}

void Registers_t::SetEEPROMDefaults(void)
{
	EEPROM.write(bootMode, 	 SERIAL);
	EEPROM.write(afcreg,     0xC4F7);
    EEPROM.write(txcreg,     0x9810);
    EEPROM.write(cfsreg,     0xA348);
    EEPROM.write(rxcreg,     0x94C0);
    EEPROM.write(bbfcreg,    0xC2AC);
    EEPROM.write(fiforstreg, 0xCA81);
    EEPROM.write(synbreg,    0xCED4);
    EEPROM.write(drsreg,     0xC623);
    EEPROM.write(pllcreg,    0xCC77);
}