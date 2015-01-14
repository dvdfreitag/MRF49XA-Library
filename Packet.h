/*
 *  Packet.h
 *  MRF49XA
 *
 *  Created by William Dillon on 3/11/14.
 *  Copyright (c) 2014 Oregon State University (COAS). All rights reserved.
 *
 *  Modified by David Freitag 1/13/15
 *
 */

#ifndef PACKET_H
#define PACKET_H

#include "MRF49XA.h"
#include "Modes.h"

class Packet_t
{
public:
	void ByteReceived(uint8_t data, volatile uint8_t &counter, volatile enum device_mode &mode, volatile MRF_packet_t &packet);
};

extern Packet_t Packet;

#endif
