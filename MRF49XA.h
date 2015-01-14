/*
 *  MRF49XA.h
 *  MRF49XA
 *
 *  Created by William Dillon on 11/1/10.
 *  Copyright 2010 Oregon State University. All rights reserved.
 *
 *  Modified by David Freitag on 1/13/15
 *
 *  Adapted from Microchip MRF49XA sample code (for register states)
 *
 */

#ifndef MRF49XA_H
#define MRF49XA_H

#include <Arduino.h>
#include "MRF49XA_definitions.h"

// The packet structure is now a mostly blank slate.  The size feild is
// only includes the payload, not the size and type.  The type feild
// contains a hint for the internal structure of the packet.
//
// If the packet structure is changed, the payload size field must be first
// and must not be renamed.  Mark the constant number of bytes that aren't
// included in the payloadSize field in the MRF_PACKET_OVERHEAD define.
//
// These defined may be used to access the maximum payload length in the app.
#define MRF_PAYLOAD_LEN        64

#define PACKET_TYPE_SERIAL     0x01
#define PACKET_TYPE_SERIAL_ECC 0x02
#define PACKET_TYPE_PACKET     0x03
#define PACKET_TYPE_PACKET_ECC 0x04

typedef struct {
    uint8_t  payloadSize;   // Total size of the payload
    uint8_t  type;          // for now, set to 0xBD
    uint8_t  payload[MRF_PAYLOAD_LEN];
} MRF_packet_t;

// These defines are used internally to the library, they include 
// Packet overhead (length)
#define MRF_PACKET_OVERHEAD 2
// the maximum packet size for internal buffers
#define MRF_PACKET_LEN      MRF_PAYLOAD_LEN + MRF_PACKET_OVERHEAD
// Space for preamble, sync (2 bytes), length, type and dummy
#define MRF_TX_PACKET_OVERHEAD 6

// Bit position:   7  6  5  4  3  2  1  0
// Normal modes:                 <X  X  X>
// Testing modes: <X  X  X>                   

// Normal modes
#define MRF_IDLE			0x00	// Listening for packets, nothing yet
#define MRF_TRANSMIT_PACKET 0x01	// Actively transmitting a packet
#define MRF_RECEIVE_PACKET  0x02	// Actively receiving a packet

// Testing modes
#define MRF_RECEIVE_ALL		0x20	// Not yet implemented
#define MRF_TRANSMIT_ZERO	0x40	// Transmits all '0'
#define MRF_TRANSMIT_ONE	0x80	// Transmits all '1'
#define MRF_TRANSMIT_ALT	0xC0	// Transmits alternating '01'

#define MRF_TX_TEST_MASK	0xC0	// Singles out spectrum test modes

/*******************************************************************************
 * This section of the header file includes the interface used by the user
 * application.  This includes an initialization routine, and functions to set
 * desired center frequency, baud rate, deviation, etc.
 *
 * In addition to initialization an configuration functions, functions are
 * provided for sending a packet, as well as receiving one.
 *
 ******************************************************************************/
class MRF49XA_t
{
public:
	// Initialize the transciever.
	void Initialize(void);

	boolean IsIdle(void);
	boolean IsAlive(void);
	uint16_t ReadStatus(void);

	// After setting registers using this function, it's a good idea to reset the xcvr
	void SetRegister(uint16_t value);

	// Packet based functions
	void TransmitPacket(MRF_packet_t *packet);
	MRF_packet_t* ReceivePacket(void);

	void SetBaudrate(uint16_t baud);	// Sets the baud rate in kbps
	void SetFrequency(uint16_t freqb);  // Setting for the FREQB register

	// Testing functions
	void TransmitZero(void);
	void TransmitOne(void);
	void TransmitAlternating(void);
	void PacketReflect(void);
	void PacketGenerator(void);
	void Reset(void);
};

extern MRF49XA_t MRF49XA;

#endif