/**************************************************************************
*   Copyright (C) 2008 - ... Heikki Pulkkinen                             *
*   heikki@CompaQ7113EA                                                   *
*                                                                         *
*   This program is free software; you can redistribute it and/or modify  *
*   it under the terms of the GNU General Public License as published by  *
*   the Free Software Foundation; either version 2 of the License, or     *
*   (at your option) any later version.                                   *
*                                                                         *
*   This program is distributed in the hope that it will be useful,       *
*   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
*   GNU General Public License for more details.                          *
*                                                                         *
*   You should have received a copy of the GNU General Public License     *
*   along with this program; if not, write to the                         *
*   Free Software Foundation, Inc.,                                       *
*   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
***************************************************************************/
#ifndef MAX765x_NAMES_H
#define MAX765x_NAMES_H

#include "../I80C52_NameS.h"

namespace MAX765x_NameS
{
	using namespace I80C52_NameS;

	//SFR REKKARIT
	unsigned char const
		DPL1=0x84,
		DPH1=0x85,
		DPS=0x86,
		CKCON=0x8E,
		EXIF=0x91,
		VER_ID=0xB2,
		SCON1=0xC0,
		SBUF1=0xC1,
		ADDAT0=0xC2,
		ADDAT1=0xC3,
		ADCON=0xC5,
		EICON=0xD8,      //Ei ole bittiosoitettava vaikka onkin 8 loppuinen
		PWPS=0xDA,
		PWDA=0xDB,
		PWDB=0xDC,
		WDT=0xDD,
		EIE=0xE8,
		EEAL=0xEA,
		EEAH=0xEB,
		EEDAT=0xEC,
		EESTCMD=0xED,
		EIP=0xF8,
		PWMC=0xFE;


	unsigned char const
		//DPS:n bitti
		DPS_SEL=BIT_0, //DPTR select 0=DPTR0(originaali) 1= DPTR1

		//CKCON bitit
		CKCON_MD0=BIT_0,   //MOVX stretc memory cycle select bit 0
		CKCON_MD1=BIT_1,   //MOVX stretc memory cycle select bit 1
		CKCON_MD2=BIT_2,   //MOVX stretc memory cycle select bit 2
		CKCON_T0M=BIT_3,   //Timer 0 clk/4 select
		CKCON_T1M=BIT_4,   //Timer 1 clk/4 select
		CKCON_T2M=BIT_5,   //Timer 2 clk/4 select
		CKCON_WD0=BIT_6,   //Select watchdog timeout periods bit0
		CKCON_WD1=BIT_7,   //Select watchdog timeout periods bit 1

		//EXIF bitit
		EXIF_IE2=BIT_4,   //Flash write/page erase done.
		EXIF_IE3=BIT_5,   //AD-Converter End of conversion interrupt

		//SCON1 bitit
		SCON1_RI=BIT_0,   //Serial port 1 Receive interrupt
		SCON1_TI=BIT_1,   //Serial port 1 Transmit interrupt
		SCON1_RB8=BIT_2,   //Serial port 1 received 9:th databit
		SCON1_TB8=BIT_3,   //Serial port 1 transmitted 9:th detabit
		SCON1_REN=BIT_4,   //Serial port 1 receive enable
		SCON1_SM2=BIT_5,   //Serial port 1 Multiprocessor mode
		SCON1_SM1=BIT_6,   //Serial port 1 mode bit 1
		SCON1_SM0=BIT_7,   //Serial port 1 mode bit 0

		//ADCON bitit
		ADCON_M0=BIT_0,   //Analog input multiplexer bit 0
		ADCON_M1=BIT_1,   //Analog input multiplexer bit 1
		ADCON_M2=BIT_2,   //Analog input multiplexer bit 2
		ADCON_M3=BIT_3,   //Analog input multiplexer bit 3
		ADCON_OVRN=BIT_4,   //Overrun flag
		ADCON_CCIE=BIT_5,   //Conversion complete interrupt enable
		ADCON_CCVT=BIT_6,   //Continous conversion enable
		ADCON_CC=BIT_7,   //Conversion complete flag

		//EICON bitit
		EICON_RWT=BIT_0,   //Reset watchdog
		EICON_EWT=BIT_1,   //Enable watchdog
		EICON_WDIF=BIT_3,   //Watchdog interrupt
		EICON_SMOD1=BIT_7,   //Serial port 1 baud rate doubler

		//WDT bitit
		WDT_WTRF=BIT_2,   //Watchdog reset flag

		//EIE bitit
		EIE_EX2=BIT_0,   //Enable flash interrupt
		EIE_EX3=BIT_1,   //Enable AD- converter interrupt
		EIE_EWDI=BIT_4,   //Enable watchdog interrupt

		//EIP bitit
		EIP_PX2=BIT_0,   //Flash interrupt priority
		EIP_PX3=BIT_1,   //AD-converter interrrupt priority
		EIP_PWDI=BIT_4,   //Watchdog interrupt priority

		//PWMC bitit
		PWMC_PWENB=BIT_0,   //PWM output B enable
		PWMC_PWENA=BIT_1,   //PWM output A enable
		PWMC_PWON=BIT_7,   //PWM enable

		//P1 Alternate functions
		P1_RXD1=BIT_2,
		P1_TXD1=BIT_3,

		//IE
		IE_ES1=BIT_6,   //Serial Port1 Interrupt enable
		//IP
		IP_PS1=BIT_6;   //Serial Port1 Interrupt priority

		//Interrupt handlays
		//Lisätty tämän prosessorin keskeytykset.
		//Jos prioriteetti 0 low niin arvo alle 0x80 jos prioriteetti 1 niin yli 0x80
		//eli MSB kertoo keskeytyksen prioriteetin
	unsigned int const SERIALPORT1_INTBIT=BIT_15;   //Sarjaportti 1 keskeytysbitti
	unsigned int const FLASH_INTBIT=BIT_14;   //Flash keskeytysbitti
	unsigned int const ADC_INTBIT=BIT_13;   //AD-Converter keskeytysbitti
	unsigned int const WDT_INTBIT=BIT_12;   //Watcdog keskeytysbitti

	unsigned int const SERIALPORT1_P0_INTON=0x07;
	unsigned int const FLASH_P0_INTON=0x08;
	unsigned int const ADC_P0_INTON=0x09;
	unsigned int const WDT_P0_INTON=0x0A;
	unsigned int const SERIALPORT1_P1_INTON=0x87;
	unsigned int const FLASH_P1_INTON=0x88;
	unsigned int const ADC_P1_INTON=0x89;
	unsigned int const WDT_P1_INTON=0x8A;

	//Keskeytysosoitteet:
	unsigned int const
		SERIALPORT1_INTERRUPT_VECTOR=0x3B,
		FLASH_INTERRUPT_VECTOR=0x43,
		ADC_INTERRUPT_VECTOR=0x4B,
		WDT_INTERRUPT_VECTOR=0x63;

	//Pinninimet
	#define PINNAME_INT0 "/INT0"
	#define PINNAME_INT1 "/INT1"
	#define PINNAME_PWMA "PWMA"
	#define PINNAME_PWMB "PWMB"
	#define PINNAME_AIN0 "AIN0"
	#define PINNAME_AIN1 "AIN1"
	#define PINNAME_AIN2 "AIN2"
	#define PINNAME_AIN3 "AIN3"
	#define PINNAME_AIN4 "AIN4"
	#define PINNAME_AIN5 "AIN5"
	#define PINNAME_AIN6 "AIN6"
	#define PINNAME_AIN7 "AIN7"
	#define PINNAME_ACOM "ACOM"
	#define PINNAME_VREFP "VREF+"
	#define PINNAME_VREFN "VREF-"
};

#endif

