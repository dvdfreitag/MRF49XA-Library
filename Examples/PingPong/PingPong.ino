#include <SPI.h>
#include <EEPROM.h>
#include "MRF49XA.h"
#include "Registers.h"
#include "Packet.h"
#include "Modes.h"

const char pingString[]			PROGMEM = "PINGING: ";

const char typeSerialString[]	PROGMEM = "\n\rSerial, ";
const char typeSerialECCString[]	PROGMEM = "\n\rSerial ECC, ";
const char typePacketString[]	PROGMEM = "\n\rPacket, ";
const char typePacketECCString[]	PROGMEM = "\n\rPacket ECC, ";
const char typeUnknownString[]	PROGMEM = "\n\rUnknown type, ";
const char packetLengthString[]	PROGMEM = "length ";
const char invalidModeString[]	PROGMEM = " Invalid mode, ";

volatile enum device_mode mode = MODE_SERIAL;
volatile uint8_t counter = 0;
volatile MRF_packet_t packet;

void setup()
{
	SPI.begin();
	Serial.begin(9600);
	MRF49XA.Initialize();

	Registers.ApplySavedRegisters();
	MRF49XA.Reset();

	mode = Registers.GetBootState();
}

void loop()
{
	if (mode == MODE_TEST_ALT  || mode == MODE_TEST_ZERO || mode == MODE_TEST_ONE  || mode == MODE_CAPTURE   || mode == MODE_TEST_PING ) {
		// Test for a new packet
		MRF_packet_t *rx_packet = MRF49XA.ReceivePacket();

		// Was the packet correctly received?
		if (rx_packet  != NULL) {
			switch (mode) {
				case MODE_TEST_PING:
					MRF49XA.TransmitPacket(rx_packet);
					Serial.println(pingString);
					printPacket(rx_packet);
					break;
				case MODE_CAPTURE:
					printPacket(rx_packet);
					break;
				default:
					// Other menu modes would end up here, ignore the packet
					break;
			}
		}
	}
	// These modes are responsible for actually using the RF interface
	else {
		switch (mode) {
			case MODE_SERIAL:
			case MODE_SERIAL_ECC:
				if (Serial.available() > 0) Packet.ByteReceived((uint8_t)Serial.read(), counter, mode, packet);
				break;							
			default:
				// This would catch any weird modes
				Serial.print(invalidModeString);
				Serial.print(mode, DEC);
				mode = MODE_SERIAL;
				break;
		}
	}
}

void printPacket(MRF_packet_t *rx_packet)
{
	// Print a label for the packet type
	switch (rx_packet->type) 
	{
		case PACKET_TYPE_SERIAL:
			Serial.print(typeSerialString);
			break;
		case PACKET_TYPE_SERIAL_ECC:
			Serial.print(typeSerialECCString);
			break;
		case PACKET_TYPE_PACKET:
			Serial.print(typePacketString);
			break;
		case PACKET_TYPE_PACKET_ECC:
			Serial.print(typePacketECCString);
			break;
		default:
			Serial.print(typeUnknownString);
			break;
	}
	
	// Print the packet length
	Serial.print(packetLengthString);
	Serial.print(rx_packet->payloadSize, DEC);
	Serial.print(": ");
	
	// Print the packet contents
	for (int i = 0; i < rx_packet->payloadSize; i++) 
	{
		if (i % 10 == 0) Serial.print("\n\r");

		Serial.print(rx_packet->payload[i]);
	}
}