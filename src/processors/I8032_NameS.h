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
#ifndef I8032_NAMES_H
#define I8032_NAMES_H

#include "I8031_NameS.h"
namespace I8032_NameS
{
	using namespace I8031_NameS;
	
	unsigned int const  INTERNAL_INDIRECT_RAM_SIZE=128;
	unsigned int const  MIN_INTERNAL_INDIRECT_RAM = 0x00;//Taulukon alkuosoite
	unsigned int const  MAX_INTERNAL_INDIRECT_RAM = 0x7F;//Taulukon loppuosoite
	unsigned int const  MIN_INTERNAL_INDIRECT_RAM_ADDRESS=0x80;//Taulukko alkaa 0x0
	unsigned int const  MAX_INTERNAL_INDIRECT_RAM_ADDRESS=0xFF;//Taulukko loppuu 0x7F
	//SFR Registers
	unsigned char const
	T2CON=0xC8,
	RCAP2L=0xCA,
	RCAP2H=0xCB,
	TL2=0xCC,
	TH2=0xCD;

	//Rekisterien bitit
	unsigned char const
	//T2CON bitit
	T2CON_CPRL2=BIT_0,
	T2CON_CT2=BIT_1,
	T2CON_TR2=BIT_2,
	T2CON_EXEN2=BIT_3,
	T2CON_TCLK=BIT_4,
	T2CON_RCLK=BIT_5,
	T2CON_EXF2=BIT_6,
	T2CON_TF2=BIT_7,

	//IE
	IE_ET2=BIT_5,

	//IP
	IP_PT2=BIT_5,

	//P1 Alternate functions
	P1_T2=BIT_0,
	P1_T2EX=BIT_1;

	//Keskeytysosoitteet:
	unsigned int const  TF2_INTERRUPT_VECTOR=0x2B;

	//Interrupt handlays
	unsigned int const TIMER2_INTBIT=BIT_16;   //Timer 2 keskeytysbitti
	unsigned int const TIMER2_P0_INTON=0x06;
	unsigned int const TIMER2_P1_INTON=0x86;
};

#endif
