/*
 *  Hamming.h
 *  MRF49XA
 *
 *  Created by William Dillon on 2/11/14.
 *  Copyright (c) 2014 Oregon State University (COAS). All rights reserved.
 *
 *  Modified by David Freitag 1/13/15
 *
 */

#ifndef HAMMING_H
#define HAMMING_H

#include <Arduino.h>

class Hamming_t
{
public:
	uint8_t EncodeNibble(uint8_t nibble);
	uint16_t EncodeByte(uint8_t data);

	uint8_t DecodeNibble(uint8_t nibble);
	uint16_t DecodeByte(uint16_t symbol);
};

extern Hamming_t Hamming;

#endif