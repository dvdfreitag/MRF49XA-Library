/*
 *  Packet.pp
 *  MRF49XA
 *
 *  Created by William Dillon on 3/11/14.
 *  Copyright (c) 2014 Oregon State University (COAS). All rights reserved.
 *
 *  Modified by David Freitag 1/13/15
 *
 */

#include <Arduino.h>
#include "Packet.h"
#include "MRF49XA.h"
#include "Modes.h"

Packet_t Packet = Packet_t();

void Packet_t::ByteReceived(uint8_t data, volatile uint8_t &counter, volatile enum device_mode &mode, volatile MRF_packet_t &packet)
{
	// Fill out the packet contents
	switch (counter) {
		case 0:
			// Sanity checking on the length byte
			if (data <= MRF_PAYLOAD_LEN) 
			{
				packet.payloadSize = data;
				counter++;
			}

			break;
		case 1:
			if (mode == MODE_SERIAL_ECC) packet.type = PACKET_TYPE_SERIAL_ECC;
			else packet.type = PACKET_TYPE_SERIAL;
			
			counter++;
			break;
		default:
			packet.payload[counter - 2] = data;
			counter++;
			break;
	}
	
	// If the counter equals the packet size, transmit
	if (counter >= packet.payloadSize + MRF_PACKET_OVERHEAD) {
		// Disable new serial data during this routine
		MRF49XA.TransmitPacket((MRF_packet_t *)(&packet));
		counter = 0;
	}
}