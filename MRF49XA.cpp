/*
 *  MRF49XA.cpp
 *  MRF49XA
 *
 *  Created by William Dillon on 11/1/10.
 *  Copyright 2010 Oregon State University. All rights reserved.
 *
 *  Modified by David Freitag 1/13/15
 *
 */

#include <Arduino.h>
#include <SPI.h>
#include "MRF49XA.h"
#include "MRF49XA_definitions.h"
#include "Hamming.h"

MRF49XA_t MRF49XA = MRF49XA_t();

static volatile uint8_t mrf_state;	// Defaults to idle
static volatile uint8_t mrf_alive;	// Set to '1' by ISR

volatile uint8_t packetCounter;

// There are 2 Rx_Packet_t instances, one for reading off the air
// and one for processing back in main. (double buffering)
MRF_packet_t Rx_Packet_a;
MRF_packet_t Rx_Packet_b;
MRF_packet_t Tx_packet;

// The hasPacket flag means that the finished_packet variable contains
// a fresh data packet.  The receiving_packet always contains space for
// received data.  If finished_packet isn't read before the a second packet
// comes the contents are lost
volatile uint8_t	hasPacket;		// Initialized to 0 by default
volatile MRF_packet_t *finished_packet;
volatile MRF_packet_t *receiving_packet;

volatile uint16_t	mrf_status;

static volatile uint8_t fiforstregUser;

void RegisterSet(uint16_t setting)
{
	SPI.transfer((uint8_t)(setting & 0x00FF));
	SPI.transfer((uint8_t)(setting & 0xFF00) >> 8);
}

static inline uint8_t ReadFifo(void)
{
	RegisterSet(MRF_RXFIFOREG);
	return SPI.transfer(0xFF);
}

static inline void IdleISR(void)
{
    uint8_t bl = ReadFifo();

    // The first byte is the packet payload length, make sure it's sensical
    if (bl <= MRF_PAYLOAD_LEN && bl > 0) 
    {
        mrf_state  = MRF_RECEIVE_PACKET;

        receiving_packet->payloadSize = bl;
        
        for (int i = 0; i < bl; i++) receiving_packet->payload[i] = 0;   // Clean the previous payload
        
        // We've received 1 byte
        packetCounter = 1;
    }
    // The length doesn't make sense, so reset
    else 
    {
        packetCounter = 0;
        MRF49XA.Reset();
        return;
    }
}

static inline void TransmitISR(void)
{
    uint8_t maxPacketCounter = 0;
    
    // ECC payloads are twice as large as advertised
    if (Tx_packet.type == PACKET_TYPE_SERIAL_ECC || Tx_packet.type == PACKET_TYPE_PACKET_ECC) 
    {
        maxPacketCounter = (Tx_packet.payloadSize * 2) + MRF_TX_PACKET_OVERHEAD;
    } 
    else 
    {
        maxPacketCounter = Tx_packet.payloadSize + MRF_TX_PACKET_OVERHEAD;
    }
    
    // Test whether we're done transmitting
    if (packetCounter >= maxPacketCounter) 
    {
        // Disable transmitter, enable receiver
        RegisterSet(MRF_PMCREG | MRF_RXCEN);
        RegisterSet(MRF_GENCREG_SET | MRF_FIFOEN);
        RegisterSet(MRF_FIFOSTREG_SET | fiforstregUser);
        RegisterSet(MRF_FIFOSTREG_SET | fiforstregUser | MRF_FSCF);
        
        // Return the state
        mrf_state = MRF_IDLE;
        packetCounter = 0;
    }
    
    switch (packetCounter) 
    {
        case 0:         // First byte is 'AA' for a alternating tone
            RegisterSet(MRF_TXBREG | 0x00AA);
            break;
        case 1:         // First of two synchronization bytes
            RegisterSet(MRF_TXBREG | 0x002D);
            break;
        case 2:         // Second of two synchronization bytes
            RegisterSet(MRF_TXBREG | 0x00D4);
            break;
        case 3:         // Size byte
            RegisterSet(MRF_TXBREG | Tx_packet.payloadSize);
            break;
        case 4:         // Type byte
            RegisterSet(MRF_TXBREG | Tx_packet.type);
            break;
            
        default:        // Payload
            // It matters which mode we're in.
            // If we're in an ECC mode, we transmit hamming-coded
            // high-nibbles on high-packet
            if (Tx_packet.type == PACKET_TYPE_SERIAL_ECC || Tx_packet.type == PACKET_TYPE_PACKET_ECC) 
            {
                // Calculate the payload byte we're using (divide by 2)
                uint8_t payloadByte = Tx_packet.payload[(packetCounter - 5) >> 1];

                // If the payload index is odd, we're transmitting the high nibble
                if ((packetCounter - 5) & 0x01) 
                {
                    RegisterSet(MRF_TXBREG | Hamming.EncodeNibble(payloadByte >> 4));
                }
                // Otherwise, it's the low nibble
                else 
                {
                    RegisterSet(MRF_TXBREG | Hamming.EncodeNibble(payloadByte & 0x0F));
                }
                
            } 
            else 
            {
                // The 5 is from the preamble, 2 sync bytes, size and type bytes.
                // Later, we'll need to include ECC calculation here.
                RegisterSet(MRF_TXBREG | Tx_packet.payload[packetCounter - 5]);
            }
            
            break;
    }
    
    packetCounter += 1;
}

// If this ISR function is called, we've recieved the payload length and nothing else
static inline void ReceiveISR(void)
{
	uint8_t bl = ReadFifo();
    
    if (packetCounter == 1) 
    {
        // We're recieving the type field
        receiving_packet->type = bl;
        packetCounter++;
        return;
    }
    
    // We've got the type field, so we know whether it's ECC, and 2x the size
    uint8_t maxPacketCounter = receiving_packet->payloadSize + MRF_PACKET_OVERHEAD;

    if (receiving_packet->type == PACKET_TYPE_SERIAL_ECC || receiving_packet->type == PACKET_TYPE_PACKET_ECC) 
    {
        maxPacketCounter = (receiving_packet->payloadSize * 2) + MRF_PACKET_OVERHEAD;
        
        // Get the location into the payload field
        uint8_t index = (packetCounter - MRF_PACKET_OVERHEAD) >> 1;
        
        // If the packet counter is odd, we're recieving the high nibble
        // The packet is cleared out beforehand, so we can just or-in the new info
        if ((packetCounter - MRF_PACKET_OVERHEAD) & 0x01) 
        {
            receiving_packet->payload[index] |= Hamming.DecodeNibble(bl) << 4;
        }
        // Otherwise, we're receiving the low nibble
        else 
        {
            receiving_packet->payload[index] |= Hamming.DecodeNibble(bl) & 0x0F;
        }
    } 
    else 
    {
        receiving_packet->payload[packetCounter - MRF_PACKET_OVERHEAD] = bl;
    }

    packetCounter++;
    
    // End of packet?
    if (packetCounter >= maxPacketCounter) 
    {
        // Reset the FIFO
        RegisterSet(MRF_FIFOSTREG_SET | fiforstregUser);
        RegisterSet(MRF_FIFOSTREG_SET | fiforstregUser | MRF_FSCF);
        
        // Swap packet structures
        finished_packet = receiving_packet;
        hasPacket = 1;

        if (receiving_packet == &Rx_Packet_a) receiving_packet =  &Rx_Packet_b;
        else receiving_packet =  &Rx_Packet_a;
        
        // Restore state
        mrf_state = MRF_IDLE;
        packetCounter = 0;
        receiving_packet->payloadSize = 0;
    }
}

ISR(MRF_IRO_VECTOR, ISR_BLOCK)
{
	// Set the MRF's CS pin low
	MRF_CS_PORTx &= ~(1 << MRF_CS_BIT);

	// This needs to be here to delay for the synchronizer
	mrf_alive = 1;
	
	// the MISO pin marks whether the FIFO needs attention
	if (digitalRead(MISO))
	{
		switch (mrf_state) 
		{
			case MRF_IDLE:              // Passively receiving
                IdleISR();
                break;

			case MRF_TRANSMIT_PACKET:   // Actively transmitting
                TransmitISR();
                break;
								
			case MRF_RECEIVE_PACKET:	// We've received at least the size
				ReceiveISR();
				break;

			case MRF_TRANSMIT_ZERO:
				RegisterSet(MRF_TXBREG | 0x0000);
				break;
				
			case MRF_TRANSMIT_ONE:
				RegisterSet(MRF_TXBREG | 0x00FF);
				break;
				
			case MRF_TRANSMIT_ALT:
				RegisterSet(MRF_TXBREG | 0x00AA);
				break;
				
			default:
				break;
		}
	} 
	// There was no FIFO flag, just leave.
	else 
	{
		MRF_CS_PORTx |=  (1 << MRF_CS_BIT);
		return;
	}	
}

void MRF49XA_t::Initialize(void)
{
	fiforstregUser = MRF_DRSTM;
	// The Chip Select is the only SPI pin that needs to be set here.
    // The rest are taken care of in the SPI init function.
    
	// Enable the IRO pin as input w/o pullup
	MRF_IRO_PORTx &= ~(1 << MRF_IRO_BIT);
	MRF_IRO_DDRx  &= ~(1 << MRF_IRO_BIT);
	
	// Enable the CS line as output with high value
	MRF_CS_PORTx  |=  (1 << MRF_CS_BIT);
	MRF_CS_DDRx   |=  (1 << MRF_CS_BIT);
	
	// Enable the FSEL as output with high value (default, low for receive)
	MRF_FSEL_PORTx |= (1 << MRF_FSEL_BIT);
	MRF_FSEL_DDRx  |= (1 << MRF_FSEL_BIT);
	
	// Enable the External interrupt for the IRO pin (falling edge)
    MRF_INT_SETUP();
	
	// configuring the MRF49XA radio
	RegisterSet(MRF_FIFOSTREG_SET);             // Set 8 bit FIFO interrupt count
	RegisterSet(MRF_FIFOSTREG_SET | MRF_FSCF);	// Enable sync. latch
	RegisterSet(MRF_GENCREG_SET);               // From the header: 434mhz, 10pF
    RegisterSet(MRF_PMCREG | MRF_CLKODIS);      // Shutdown everything

    RegisterSet(MRF_TXCREG  | MRF_MODBW_30K | MRF_OTXPWR_0);
    RegisterSet(MRF_RXCREG  | MRF_FINTDIO   | MRF_RXBW_67K | MRF_DRSSIT_103db);
    RegisterSet(MRF_BBFCREG | MRF_ACRLC | (4 & MRF_DQTI_MASK));

    // antenna tuning on startup
    RegisterSet(MRF_PMCREG | MRF_CLKODIS | MRF_TXCEN); // turn on the transmitter
    delay(5);                            // wait for oscillator to stablize
	// end of antenna tuning
	
	// turn off transmitter, turn on receiver
    RegisterSet(MRF_PMCREG | MRF_CLKODIS | MRF_RXCEN);
    RegisterSet(MRF_GENCREG_SET | MRF_FIFOEN);
	RegisterSet(MRF_FIFOSTREG_SET);
	RegisterSet(MRF_FIFOSTREG_SET | MRF_FSCF);
	
	// Setup the packet pointers
	receiving_packet = &Rx_Packet_a;
	
	// Dummy read of status registers to clear Power on reset flag
	mrf_status = ReadStatus();
	
    mrf_state = MRF_IDLE;
    
	// Enable interrupt last, just in case they're already globally enabled
	MRF_INT_MASK();
}

uint8_t MRF49XA_t::IsIdle(void)
{
	if (mrf_state == MRF_IDLE) return 1;
	
	return 0;
}

// This function checks the alive flag in the ISR.
// If for whatever reason the ISR doesn't fire since the last
// time checked return 0, unless the module is idle.
// Otherwise, if the ISR has run since last time, send 1.
uint8_t MRF49XA_t::IsAlive(void)
{
	if (mrf_alive) 
	{
		mrf_alive = 0;
		return 1;
	} 
	

	if (mrf_state == MRF_IDLE) return 1;	
	return 0;
}

uint16_t MRF49XA_t::ReadStatus(void)
{
	uint16_t retval = 0x0000;

	retval |= SPI.transfer(0x00);
	retval << 8;

	retval |= SPI.transfer(0x00);

	return retval;
}

void MRF49XA_t::SetRegister(uint16_t value)
{
	// We need to detect whether the FIFORSTREG is being set.  There are user
    // flags and core flags in the same register, therefore, we need to save
    // the user parts of it, and bitwise-OR them with the core flags.
    if ((value & 0xFF00) == MRF_FIFORSTREG) 
    {
        fiforstregUser = value & (MRF_DRSTM | MRF_SYCHLEN); // Filter-out all but the user fields
        return;
    }
    
    RegisterSet(value);
}

void MRF49XA_t::TransmitPacket(MRF_packet_t *packet)
{
	uint8_t	i;
	uint8_t wait = 1;

	// We can check, without synchronization
	// (because it doesn't change in the ISR)
	// Whether we're in a testing mode.
    // If we are, reset the device and proceed
	if (mrf_state & MRF_TX_TEST_MASK) Reset();
	
	// Wait for the module to be idle (this is cheezy synchronization)
	do 
	{
		noInterrupts();			// Disable interrupts, this is a critical section

		if (mrf_state == MRF_IDLE) 
		{
			mrf_state = MRF_TRANSMIT_PACKET;
			wait = 0;
			interrupts();		// Atomic operation complete, reenable interrupts
		} 
		else 
		{
			interrupts();		// Atomic operation complete, reenable interrupts
			delay(1);			// This should be roughly 1 byte period
		}
	} 
	while (wait);

	// Initialize the constant parts of the transmit buffer
	packetCounter = 0;

    // Copy the packet
    Tx_packet.payloadSize = packet->payloadSize;
    Tx_packet.type        = packet->type;
    
    for (i = 0; i < packet->payloadSize; i++) Tx_packet.payload[i] = packet->payload[i];

	RegisterSet(MRF_PMCREG);					// Turn everything off
	RegisterSet(MRF_GENCREG_SET | MRF_TXDEN);	// Enable TX FIFO
	// Reset value of TX FIFO is 0xAAAA
	
	RegisterSet(MRF_PMCREG | MRF_TXCEN);		// Begin transmitting
	// Everything else is handled in the ISR
}

MRF_packet_t* MRF49XA_t::ReceivePacket(void)
{
	if (hasPacket)
	{
		hasPacket = 0;
		return (MRF_packet_t *)finished_packet;
	}

	return 0;
}

// TODO: Missing?
void MRF49XA_t::SetBaudrate(uint16_t baud)
{

}

void MRF49XA_t::SetFrequency(uint16_t freqb)
{
	// Mask the input value
    freqb = freqb & MRF_FREQB_MASK;
    
    // Make sure it's within the range (do nothing if its not)
    if (freqb < 97 || freqb > 3903) return;
    
    RegisterSet(MRF_CFSREG | freqb);
}

void MRF49XA_t::TransmitZero(void)
{
	// If we're already doing a spectrum test, just mark the new pattern
	if (mrf_state & MRF_TX_TEST_MASK) 
	{
		mrf_state = MRF_TRANSMIT_ZERO;
		return;
	}
	
	mrf_state = MRF_TRANSMIT_ZERO;
    
	// Enable the TX Register
	RegisterSet(MRF_GENCREG_SET | MRF_TXDEN);
	
	// The transmit register is filled with 0xAAAA, we want it to be zeros
	RegisterSet(MRF_TXBREG | 0x0000);
	
	// Enable the transmitter
	RegisterSet(MRF_PMCREG | MRF_CLKODIS | MRF_TXCEN);
	
	// Upon completion of a byte !IRO should toggle
	return;	
}

void MRF49XA_t::TransmitOne(void)
{
	// If we're already doing a spectrum test, just mark the new pattern
	if (mrf_state & MRF_TX_TEST_MASK) 
	{
		mrf_state = MRF_TRANSMIT_ONE;
		return;
	}
	
	mrf_state = MRF_TRANSMIT_ONE;
	
	// Enable the TX Register
	RegisterSet(MRF_GENCREG_SET | MRF_TXDEN);
	
	// The transmit register is filled with 0xAAAA, we want it to be ones
	RegisterSet(MRF_TXBREG | 0x00FF);
	
	// Enable the transmitter
	RegisterSet(MRF_PMCREG | MRF_CLKODIS | MRF_TXCEN);
	
	// Upon completion of a byte !IRO should toggle
	return;	
}

void MRF49XA_t::TransmitAlternating(void)
{
	// If we're already doing a spectrum test, just mark the new pattern
	if (mrf_state & MRF_TX_TEST_MASK) 
	{
		mrf_state = MRF_TRANSMIT_ALT;
		return;
	}
	
	mrf_state = MRF_TRANSMIT_ALT;

	// Enable the TX Register
	RegisterSet(MRF_GENCREG_SET | MRF_TXDEN);
	
	// The transmit register is filled with 0xAAAA, we can leave it alone
	
	// Enable the transmitter
	RegisterSet(MRF_PMCREG | MRF_CLKODIS | MRF_TXCEN);
	
	// Upon completion of a byte !IRO should toggle
	return;	
}

// TODO: Missing?
void MRF49XA_t::PacketReflect(void)
{

}

// TODO: Missing?
void MRF49XA_t::PacketGenerator(void)
{

}

void MRF49XA_t::Reset(void)
{
	RegisterSet(MRF_PMCREG);
	RegisterSet(MRF_FIFOSTREG_SET | fiforstregUser);
	RegisterSet(MRF_GENCREG_SET);
	RegisterSet(MRF_GENCREG_SET | MRF_FIFOEN);
	RegisterSet(MRF_FIFOSTREG_SET | MRF_FSCF | fiforstregUser);
	RegisterSet(MRF_PMCREG | MRF_RXCEN);	

    mrf_status = MRF_IDLE;
}
