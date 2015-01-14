/*
 *  MRF49XA_definitions.h
 *  MRF49XA
 *
 *  Created by William Dillon on 11/2/10.
 *  Copyright 2010 Oregon State University. All rights reserved.
 *
 *  Modified by David Freitag on 1/13/15
 *
 */

#ifndef MRF49XA_DEFINITIONS_H
#define MRF49XA_DEFINITIONS_H

/*******************************************************************************
 * Bit definitions for MRF registers
 *
 * These defines are provided for use configuring the MRF49XA module.  The
 * register address is the first define in each section, and must be written
 * to the MOSI pins to access the register.
 *
 * To set register states, bit-wise or the address with the desired set bits
 * for the register.
 *
 * To read the register states, you must also write to any bits that are R/W
 * Luckily, the only register worth reading is STSREG, and has no settable bits
 * 
 * Some registers contain values that need to be derived, or select a range of
 * settings.  Each of these has their own section containing the equation
 * and/or individual defines for each setting.
 *
 ******************************************************************************/
// Status read register
#define MRF_STSREG		0x0000		// Status read register address
#define MRF_TXRXFIFO	0x8000		// FIFO state (1 = ready)
#define MRF_POR			0x4000		// Power-on-Reset flag
#define MRF_TXOWRXOF	0x2000		// Underrun/Overwrite/Overflow
#define MRF_WUTINT		0x1000		// Wakeup timer overflow interrupt
#define MRF_LCEXINT		0x0800		// Logic change interrupt
#define MRF_LBTD		0x0400		// Low Battery threshold detect
#define MRF_FIFOEM		0x0200		// Receiver FIFO empty (1 = empty)
#define MRF_ATTRSSI		0x0100		// Antenna Tuning and RSSI indicator
#define MRF_DQDO		0x0080		// Data Quality Detect/Indicate output
#define MRF_CLKRL		0x0040		// Clock recovery lock bit
#define MRF_AFCCT		0x0020		// Automatic frequency control cycle toggle
#define	MRF_OFFSV		0x0010		// Measured frequency offset of AFC cycle
#define MRF_OFFSET_MASK	0x000F		// Value of the AFC offset (see datasheet)

/*******************************************************************************
 * Convenience definitions for Automatic frequency control configuration reg.
 *
 * These defines are provided for use configuring the MRF49XA module.
 * 
 * The AutoMS field sets the mode, and ARFO sets the allowable tuning range.
 * The MFCS, HAM, FOREN, and FOFEN bits set the manual control strobe, high
 * accuracy mode, frequency offset register, and frequency offset respectively
 *
 ******************************************************************************/
// Automatic frequency control configuration register
#define MRF_AFCCREG		0xC400		// AFC configuration register address

// Automatic frequency mode selection
#define MRF_AUTOMS_INDP	0x00C0		// Offset independent for state of DIO sig.
#define MRF_AUTOMS_RECV	0x0080		// Offset only during receive
#define MRF_AUTOMS_ONCE	0x0040		// Offset once after power-cycle
#define MRF_AUTOMS_OFF	0x0000		// Auto mode off

// Allowable tuning range selection
#define MRF_ARFO_3to4	0x0030		// +3 to -4 Fres (tuning bits)
#define MRF_ARFO_7to8	0x0020		// +7 to -8 Fres
#define MRF_ARFO_15to16	0x0010		// +15 to -16 Fres
#define MRF_ARFO_unlim	0x0000		// Unlimited

// Other flags in automatic tuning set register
#define MRF_MFCS		0x0008		// Manual Frequency control strobe
#define MRF_HAM			0x0004		// High accuracy mode
#define MRF_FOREN		0x0002		// Frequency Offset Register Enable
#define MRF_FOFEN		0x0001		// Frequency Offset Enable

/*******************************************************************************
 * Convenience definitions for Transmit byte register
 * 
 * This register allows for filling the transmit register one byte at a time.
 * The top byte of the command is the address, the bottom byte is the data.
 ******************************************************************************/
// Transmit Byte Register
#define MRF_TXBREG		0xB800		// Transmit byte register address
#define MRF_TXDB_MASK	0x00FF		// Transmit byte mask


// Baseband Filter Configuration Register
#define MRF_BBFCREG		0xC228		// Baseband filter config. register address
#define MRF_ACRLC		0x0080		// Automatic clock recovery lock control
#define MRF_MCRLC		0x0040		// Manual clock recovery lock control
#define MRF_FTYPE		0x0010		// Filter type (0 = digital, 1 = Ext. RC)
#define MRF_DQTI_MASK	0x0007		// Data quality threshold indicator

// Receiver FIFO read register
#define MRF_RXFIFOREG	0xB000		// Receiver FIFO read register
#define MRF_RXDB_MASK	0x00FF		// Receive data byte mask

/*******************************************************************************
 * Convenience definitions for FIFO and Reset mode configuration register
 *
 * These defines are provided for use configuring the MRF49XA module.
 * 
 * The FFBC field of this register sets the number of bits to receive before
 * a FIFO full interrupt is generated.  A reasonable number is 8.  The maximum
 * is 15 bits.
 *
 ******************************************************************************/
// FIFO and reset mode configuration register
#define MRF_FIFORSTREG	0xCA00		// FIFO/Reset mode configuration register
#define MRF_FFBC_MASK	0x00F0		// FIFO Fill Bit Count (maximum = 15)
#define MRF_SYCHLEN		0x0008		// Synchronous Character length
#define MRF_FFSC		0x0004		// FIFO Fill Start condition
#define MRF_FSCF		0x0002		// FIFO Synchronous Character fill
#define MRF_DRSTM		0x0001		// Disable (sensitive) reset mode

// Synchrnonous byte configuration register
#define MRF_SYNBREG		0xCE00		// Synchrnous byte config. register address
#define MRF_SYNCB		0x00FF		// Sync byte configuration

// Power Management Configuration Register
#define MRF_PMCREG		0x8200		// Power Mgmt. Config. Register address
#define MRF_RXCEN		0x0080		// Receiver chain enable
#define MRF_BBCEN		0x0040		// Baseband chain enable
#define MRF_TXCEN		0x0020		// Transmitter chain enable
#define MRF_SYNEN		0x0010		// Synthesier enable
#define MRF_OSCEN		0x0008		// Oscillator enable
#define MRF_LBDEN		0x0004		// Low Battery Detector Enable
#define	MRF_WUTEN		0x0002		// Wakeup timer enable
#define MRF_CLKODIS		0x0001		// Clock output disable

// Wakeup timer value set register
#define MRF_WTSREG		0xE000		// Wakeup timer value register address
#define MRF_WTEV_MASK	0x1F00		// Wakeup timer exponential value
#define MRF_WTMV_MASK	0x00FF		// Wakeup timer multiplier exponential value

// Duty cycle value set register
#define MRF_DCSREG		0xC800		// Duty cycle value set register
#define MRF_DCMV_MASK	0x00FE		// Duty cycle multiplier value mask
#define MRF_DCMEN		0x0001		// Duty cycle mode enable

// Battery threshold and clock output value set register
#define MRF_CSREG		0xC000		// Battery thres. and clock set reg. address
#define MRF_COFSB_MASK	0x00E0		// Clock output frequency set
#define MRF_LBDVB_MASK	0x0008		// Low battery detect value

/*******************************************************************************
 * Convenience definitions for PLL Configuration register
 *
 * These defines are provided for use configuring the MRF49XA module.
 * 
 * The default setting is for 5-10 MHz output clock, Phase detector delay
 * disabled, PLL dithering disabled, and PLL bandwidth set to -102dBc/Hz
 * which is appropriate for higher bit rates.
 *
 * > 90kbps -> -102dBc/Hz (max 256kbps)
 * < 90kbps -> -107dBc/Hz (max 86.2kbps) 
 *
 ******************************************************************************/
// PLL Configuration register
#define MRF_PLLCREG		0xCC12		// PLL configuration register

// Clock buffer time control settings
#define MRF_CBTC_5p		0x0060		// Clock buffer 5-10 Mhz
#define MRF_CBTC_3		0x0040		// Clock buffer 3.3 Mhz
#define MRF_CBTC_2p		0x0020		// Clock buffer > 2.5 Mhz
#define MRF_CBTC_2m		0x0000		// Clock buffer < 2.5 Mhz

#define MRF_PDDS		0x0008		// Phase detector delay
#define MRF_PLLDD		0x0004		// PLL Dithering Disable
#define MRF_PLLBWB		0x0001		// PLL Bandwidth (102dBc/Hz)

/*******************************************************************************
 * Convenience definitions for band setting
 *
 * These defines are provided for use configuring the MRF49XA module.
 * Select the frequency band for the given hardware by uncommenting the
 * appropriate line.  Set the crystal load capactiance using the MRF_XTAL_LD_CAP
 * value.
 *
 * The load capactiance is given by the following equation:
 *
 * Cap pF = 8.5 + (LCS / 2)
 * LCS = (10 - 8.5) * 2
 *
 * For 10pF: LCS = (10 - 8.5) * 2 = 1.5 * 2 = 3
 * 
 ******************************************************************************/
// General Configuration Register
#define MRF_GENCREG		0x8000		// General configuration register addres
#define MRF_TXDEN		0x0080		// TX Data Register enable bit
#define MRF_FIFOEN		0x0040		// FIFO enable bit
#define MRF_FBS_MASK	0x0030		// Mask for the band selection
#define MRF_LCS_MASK	0x000F		// Mask for the crystal load capactiance

// 10pF Crystal load capacitance
#define MRF_LCS			3			// Crystal Load capactiance

// Frequency band settings
#define MRF_FBS_434		0x0010		// 434 mHz band
#define MRF_FBS_868		0x0020		// 868 mHz band
#define MRF_FBS_915		0x0030		// 915 mHz band

/*******************************************************************************
 * Convenience definitions for center frequency
 *
 * These defines are provided for use configuring the MRF49XA module.
 * Select the center frequency using the following equation:
 * 
 * Fo = 10 * FA1 * (FA0 + Fval/4000)
 * Fval = Decimal value of FREQB, 96 < Fval < 3903
 * 
 * Choose FA1 anf FA0 using the following table according to band:
 * 
 *		Range		FA1			FA0
 *		434			1			43
 *		868			2			43
 *		915			3			30
 *
 * Fo = 10 * (43 + Fval/4000) = 10*43 + 10*(Fval/4000) = 430 + Fval/400
 *
 * Fval = (Fo - 430) * 400 = (432.10 - 430) * 400 = 2.1 * 400 = 840
 * 
 ******************************************************************************/
// Center Frequency Value Set Register
#define MRF_CFSREG		0xA000		// Center Frequency value register address
#define MRF_FREQB_MASK	0x0FFF		// Center Frequency value (see datasheet)

// Frequency setting for 432.10 mHz
#define MRF_FREQB		840

/*******************************************************************************
 * Convenience definitions for receiver control register
 *
 * These defines are provided for use configuring the MRF49XA module.
 * It is possible to set the values for Data Integrity integration time,
 * Baseband bandwidth values, and LNA gain.
 *
 * Use the MRF_RXCREG_SET define for Microchip defaults
 * 
 ******************************************************************************/
// Receive control register
#define MRF_RXCREG		0x9000		// Receive control register address
#define MRF_FINTDIO		0x0400		// Function interrupt/dio output
#define MRF_DIORT_MASK	0x0300		// Data indicator response time
#define MRF_RXBW_MASK	0x00E0		// Receiver baseband bandwidth
#define MRF_RXLNA_MASK	0x0018		// Receiver LNA gain
#define MRF_DRSSIT_MASK 0x0007		// Digital RSSI threshold

// Data indicator output response time
#define MRF_DIORT_CONT	0x0300		// Continuous
#define MRF_DIORT_SLOW	0x0200		// Slow
#define MRF_DIORT_MED	0x0100		// Medium
#define MRF_DIORT_FAST	0x0000		// Fast

// Receiver Baseband bandwidth
#define MRF_RXBW_67K	0x00C0		// Receiver Bandwidth 67 khz
#define MRF_RXBW_134K	0x00A0		// Receiver Bandwidth 134 khz
#define MRF_RXBW_200K	0x0080		// Receiver Bandwidth 200 khz
#define MRF_RXBW_270K	0x0060		// Receiver Bandwidth 270 khz
#define MRF_RXBW_340K	0x0040		// Receiver Bandwidth 340 khz
#define MRF_RXBW_400K	0x0020		// Receiver Bandwidth 400 khz

// Receiver LNA Gain
#define MRF_RXLNA_20DB	0x0018		// LNA Gain -20dB
#define MRF_RXLNA_14DB	0x0010		// LNA Gain -14dB
#define MRF_RXLNA_6DB	0x0008		// LNA Gain -6dB
#define MRF_RXLNA_0DB	0x0000		// LNA Gain  0dB

// Digital RSSI threshold
#define MRF_DRSSIT_73db	0x0005		// -73dB Threshold
#define MRF_DRSSIT_79db	0x0004		// -79dB Threshold
#define MRF_DRSSIT_85db	0x0003		// -85dB Threshold
#define MRF_DRSSIT_91db	0x0002		// -91dB Threshold
#define MRF_DRSSIT_97db	0x0001		// -97dB Threshold
#define MRF_DRSSIT_103db 0x0000		// -103dB Threshold

/*******************************************************************************
 * Convenience definitions for transmitter control register
 *
 * These defines are provided for use configuring the MRF49XA module.
 * Select the modulation bandwidth and output power by bit-wise oring them
 * with the register address.
 *
 * Use the MRF_TXCREG_SET for Microchip defaults
 * 
 ******************************************************************************/
// Transmit Configuration register
#define MRF_TXCREG		0x9800		// Transmit configuration register address
#define MRF_MODPLY		0x0100		// Modulation polarity
#define MRF_MODBW_MASK	0x00F0		// Modulation bandwidth
#define MRF_OTXPWR_MASK 0x0007		// Output transmit power

// Modulation bandwidth settings
#define MRF_MODBW_240K	0x00F0		// 240kHz modulation bandwidth
#define MRF_MODBW_225K	0x00E0		// 225kHz modulation bandwidth
#define MRF_MODBW_210K	0x00D0		// 210kHz modulation bandwidth
#define MRF_MODBW_195K	0x00C0		// 195kHz modulation bandwidth
#define MRF_MODBW_180K	0x00B0		// 180kHz modulation bandwidth
#define MRF_MODBW_165K	0x00A0		// 165kHz modulation bandwidth
#define MRF_MODBW_150K	0x0090		// 150kHz modulation bandwidth
#define MRF_MODBW_135K	0x0080		// 135kHz modulation bandwidth
#define MRF_MODBW_120K	0x0070		// 120kHz modulation bandwidth
#define MRF_MODBW_105K	0x0060		// 105kHz modulation bandwidth
#define MRF_MODBW_90K	0x0050		//  90kHz modulation bandwidth
#define MRF_MODBW_75K	0x0040		//  75kHz modulation bandwidth
#define MRF_MODBW_60K	0x0030		//  60kHz modulation bandwidth
#define MRF_MODBW_45K	0x0020		//  45kHz modulation bandwidth
#define MRF_MODBW_30K	0x0010		//  30kHz modulation bandwidth
#define MRF_MODBW_15K	0x0000		//  15kHz modulation bandwidth

// Output power settings
#define MRF_OTXPWR_17D5	0x0007		// -17.5dB Transmit Output Power
#define MRF_OTXPWR_15D0	0x0006		// -15.0dB Transmit Output Power
#define MRF_OTXPWR_12D5	0x0005		// -12.5dB Transmit Output Power
#define MRF_OTXPWR_10D5	0x0004		// -10.5dB Transmit Output Power
#define MRF_OTXPWR_7D5	0x0003		// - 7.5dB Transmit Output Power
#define MRF_OTXPWR_5D0	0x0002		// - 5.0dB Transmit Output Power
#define MRF_OTXPWR_2D5	0x0001		// - 2.5dB Transmit Output Power
#define MRF_OTXPWR_0	0x0000		//   0.0dB Transmit Output Power

/*******************************************************************************
 * Convenience definitions for data rate value set register
 *
 * These defines are provided for use configuring the MRF49XA module.
 * 
 * To calculate the DRPV value using the following equation:
 *
 * DRPV = 10000/[29 * (1 + DPRE * 7) * DREx] - 1 
 *
 * Where DPRE is either 1 or 0, and DREx is the desired data rate (kbps)
 *
 * 10000/[29 * 1 * 9.6] - 1 = 10000/278.4 - 1 = 35.9 - 1 = 34.9 = 35
 * 
 ******************************************************************************/
// Data Rate Value set register
#define MRF_DRSREG		0xC600		// Data rate value set register address
#define MRF_DRPE		0x0080		// Data rate prescaler enable
#define MRF_DRPV_MASK	0x007F		// Data rate value mask

// Settings for approx. 9600 baud
#define MRF_DRPE_ENABLE	0x0080		// Data rate prescaler enable bit
#define MRF_DRPV_VALUE	35			// Derived value for 9579 baud

/* These defines set the chip select line and interrupt for the MRF module
 * TODO: These values need to be changed.
 */
#define MRF_CS_PINx		PINB
#define MRF_CS_PORTx	PORTB
#define MRF_CS_DDRx		DDRB
#define MRF_CS_BIT		4

#define MRF_FSEL_PORTx	PORTB
#define MRF_FSEL_DDRx	DDRB
#define MRF_FSEL_BIT	5

#define MRF_DIO_PINx	PINB
#define MRF_DIO_PORTx	PORTB
#define MRF_DIO_DDRx	DDRB
#define MRF_DIO_BIT     6

#define MRF_RCLK_PINx	PINB
#define MRF_RCLK_PORTx	PORTB
#define MRF_RCLK_DDRx	DDRB
#define MRF_RCLK_BIT	7

#define MRF_IRO_PINx	PINC
#define MRF_IRO_PORTx	PORTC
#define MRF_IRO_DDRx	DDRC
#define MRF_IRO_BIT		7

#define MRF_IRO_VECTOR	INT1_vect

// These are macros for setting up the interrupts for the MRF
#define MRF_INT_SETUP()	EICRA |= (1 << ISC11)
#define MRF_INT_MASK()	EIMSK |= (1 << INT1)

/*******************************************************************************
 * These defines set either the soldered-on characteristics of the MRF module,
 * or they are specific to this application. This includes the frequency band,
 * clock output, and pin settings.
 *
 ******************************************************************************/
// Set the module to 434 Mhz band, with 10pF series capactance crystal
#define MRF_GENCREG_SET		(MRF_GENCREG | (MRF_LCS & MRF_LCS_MASK) | MRF_FBS_434)
#define MRF_AFCCREG_SET		(MRF_AFCCREG | MRF_AUTOMS_INDP | MRF_ARFO_3to4 | MRF_HAM | MRF_FOFEN) // defaults
#define MRF_PLLCREG_SET		(MRF_PLLCREG | MRF_CBTC_5p)
#define MRF_FIFOSTREG_SET	(MRF_FIFORSTREG | MRF_DRSTM | ((8 << 4) & MRF_FFBC_MASK))

#endif