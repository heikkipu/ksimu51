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
#ifndef I8031_NAMES_H
#define I8031_NAMES_H

#include "Global_NameS.h"
#include "InstructionSet_NameS.h"

using namespace Global_NameS;

namespace I8031_NameS
{
	using namespace InstructionSet_NameS;
	//Muistien koot
	//Ulkoinen RAM
	unsigned int const EXTERNAL_DATA_MEMORY_SIZE=65536;
	unsigned int const MIN_EXTERNAL_DATA_MEMORY_ADDRESS=0;
	unsigned int const MAX_EXTERNAL_DATA_MEMORY_ADDRESS=0xFFFF;

	//Ulkoinen ROM
	unsigned int const EXTERNAL_PROGRAM_MEMORY_SIZE=65536;
	unsigned int const MIN_EXTERNAL_PROGRAM_MEMORY_ADDRESS=0;
	unsigned int const MAX_EXTERNAL_PROGRAM_MEMORY_ADDRESS=0xFFFF;

	//Sisäinen RAM
	unsigned int const INTERNAL_DATA_MEMORY_SIZE=256;
	unsigned int const MIN_INTERNAL_DATA_MEMORY_ADDRESS=0;
	unsigned int const MAX_INTERNAL_DATA_MEMORY_ADDRESS=0xFF;
	unsigned int const REGISTER_BANK_SIZE=0x20;
	unsigned int const MIN_REGISTER_BANK_ADDRESS=0x00;
	unsigned int const REGISTER_BANK0_ADDRESS=0x00;
	unsigned int const REGISTER_BANK1_ADDRESS=0x08;
	unsigned int const REGISTER_BANK2_ADDRESS=0x10;
	unsigned int const REGISTER_BANK3_ADDRESS=0x18;
	unsigned int const MAX_REGISTER_BANK_ADDRESS=0x1F;
	unsigned int const BIT_AREA_SIZE=0x10;
	unsigned int const MIN_BIT_AREA_ADDRESS=0x20;
	unsigned int const MAX_BIT_AREA_ADDRESS=0x2F;
	unsigned int const MIN_BIT_AREA_BIT_ADDRESS=0x0;
	unsigned int const MAX_BIT_AREA_BIT_ADDRESS=0x7F;
	unsigned int const SCRATCH_PAD_SIZE=0x50;
	unsigned int const MIN_SCRATCH_PAD_ADDRESS=0x30;
	unsigned int const MAX_SCRATCH_PAD_ADDRESS=0x7F;
	unsigned int const SFR_SIZE=0x80;
	unsigned int const MIN_SFR_ADDRESS=0x80;
	unsigned int const MAX_SFR_ADDRESS=0xFF;

	//SFR REKKARIT
	unsigned char const
		ACC=0xE0,
		B=0xF0,
		PSW=0xD0,
		SP=0x81,
		DPL=0x82,
		DPH=0x83,
		P0=0x80,
		P1=0x90,
		P2=0xA0,
		P3=0xB0,
		IP=0xB8,
		IE=0xA8,
		TMOD=0x89,
		TCON=0x88,
		TH0=0x8C,
		TL0=0x8A,
		TH1=0x8D,
		TL1=0x8B,
		SCON=0x98,
		SBUF=0x99;

	//SFR rekkarien bitit
	//ACC
	unsigned char const
		ACC_0=BIT_0,
		ACC_1=BIT_1,
		ACC_2=BIT_2,
		ACC_3=BIT_3,
		ACC_4=BIT_4,
		ACC_5=BIT_5,
		ACC_6=BIT_6,
		ACC_7=BIT_7,
		//B
		B_0=BIT_0,
		B_1=BIT_1,
		B_2=BIT_2,
		B_3=BIT_3,
		B_4=BIT_4,
		B_5=BIT_5,
		B_6=BIT_6,
		B_7=BIT_7,
		//PSW
		PSW_P=BIT_0,
		PSW_F1=BIT_1,
		PSW_OV=BIT_2,
		PSW_RS0=BIT_3,
		PSW_RS1=BIT_4,
		PSW_F0=BIT_5,
		PSW_AC=BIT_6,
		PSW_CY=BIT_7,
		//P0
		P0_0=BIT_0,
		P0_1=BIT_1,
		P0_2=BIT_2,
		P0_3=BIT_3,
		P0_4=BIT_4,
		P0_5=BIT_5,
		P0_6=BIT_6,
		P0_7=BIT_7,
		//P1
		P1_0=BIT_0,
		P1_1=BIT_1,
		P1_2=BIT_2,
		P1_3=BIT_3,
		P1_4=BIT_4,
		P1_5=BIT_5,
		P1_6=BIT_6,
		P1_7=BIT_7,
		//P2
		P2_0=BIT_0,
		P2_1=BIT_1,
		P2_2=BIT_2,
		P2_3=BIT_3,
		P2_4=BIT_4,
		P2_5=BIT_5,
		P2_6=BIT_6,
		P2_7=BIT_7,
		//P3
		P3_0=BIT_0,
		P3_1=BIT_1,
		P3_2=BIT_2,
		P3_3=BIT_3,
		P3_4=BIT_4,
		P3_5=BIT_5,
		P3_6=BIT_6,
		P3_7=BIT_7,
		//P3 Alternate functions
		P3_RXD=BIT_0,
		P3_TXD=BIT_1,
		P3_INT0=BIT_2,
		P3_INT1=BIT_3,
		P3_T0=BIT_4,
		P3_T1=BIT_5,
		P3_WR=BIT_6,
		P3_RD=BIT_7,
		P3_RXD_NUM=BIT_NUMBER_0,
		P3_TXD_NUM=BIT_NUMBER_1,
		P3_INT0_NUM=BIT_NUMBER_2,
		P3_INT1_NUM=BIT_NUMBER_3,
		P3_T0_NUM=BIT_NUMBER_4,
		P3_T1_NUM=BIT_NUMBER_5,
		P3_WR_NUM=BIT_NUMBER_6,
		P3_RD_NUM=BIT_NUMBER_7,
		//IP
		IP_PX0=BIT_0,
		IP_PT0=BIT_1,
		IP_PX1=BIT_2,
		IP_PT1=BIT_3,
		IP_PS=BIT_4,
		//IE
		IE_EX0=BIT_0,
		IE_ET0=BIT_1,
		IE_EX1=BIT_2,
		IE_ET1=BIT_3,
		IE_ES=BIT_4,
		IE_EA=BIT_7,
		//TMOD
		TMOD_T0M0=BIT_0,
		TMOD_T0M1=BIT_1,
		TMOD_T0CT=BIT_2,
		TMOD_T0GATE=BIT_3,
		TMOD_T1M0=BIT_4,
		TMOD_T1M1=BIT_5,
		TMOD_T1CT=BIT_6,
		TMOD_T1GATE=BIT_7,
		//TCON
		TCON_IT0=BIT_0,
		TCON_IE0=BIT_1,
		TCON_IT1=BIT_2,
		TCON_IE1=BIT_3,
		TCON_TR0=BIT_4,
		TCON_TF0=BIT_5,
		TCON_TR1=BIT_6,
		TCON_TF1=BIT_7,
		//SCON rekkarin bitit
		SCON_RI=BIT_0,
		SCON_TI=BIT_1,
		SCON_RB8=BIT_2,
		SCON_TB8=BIT_3,
		SCON_REN=BIT_4,
		SCON_SM2=BIT_5,
		SCON_SM1=BIT_6,
		SCON_SM0=BIT_7;

	//Ohjelman kulkuun liittyvää
	//readInstructionDoCycle(bool const readOnlyInstruction) parametri. 
	bool const READ_ONLY_INSTRUCTION=true;
	bool const DO_CYCLE_ALSO=false;
	
	//Interrupt handlays
	unsigned int const EXTINT0_INTBIT=BIT_21;   //INT 0 keskeytysbitti
	unsigned int const TIMER0_INTBIT=BIT_20;   //Timer 0 keskeytysbitti
	unsigned int const EXTINT1_INTBIT=BIT_19;   //INT 1 keskeytysbitti
	unsigned int const TIMER1_INTBIT=BIT_18;   //Timer 1 keskeytysbitti
	unsigned int const SERIALPORT_INTBIT=BIT_17;    //Serial port receive keskeytysbitti

	unsigned int const EXTINT0_P0_INTON=0x01;
	unsigned int const TIMER0_P0_INTON=0x02;
	unsigned int const EXTINT1_P0_INTON=0x03;
	unsigned int const TIMER1_P0_INTON=0x04;
	unsigned int const SERIALPORT_P0_INTON=0x05;
	unsigned int const PRIORITY1_INTVALUES=0x80;
	unsigned int const EXTINT0_P1_INTON=0x81;
	unsigned int const TIMER0_P1_INTON=0x82;
	unsigned int const EXTINT1_P1_INTON=0x83;
	unsigned int const TIMER1_P1_INTON=0x84;
	unsigned int const SERIALPORT_P1_INTON=0x85;
	
	//Keskeytysosoitteet:
	unsigned int const EXTINT0_INTERRUPT_VECTOR=0x03;
	unsigned int const TIMER0_INTERRUPT_VECTOR=0x0B;
	unsigned int const EXTINT1_INTERRUPT_VECTOR=0x13;
	unsigned int const TIMER1_INTERRUPT_VECTOR=0x1B;
	unsigned int const SERIALPORT_INTERRUPT_VECTOR=0x23;

	unsigned char const UART=0;

	typedef unsigned long machineCycles_Type;
	typedef unsigned long alePulses_Type;
	typedef unsigned long clockTicks_Type;
	typedef unsigned long programCounter_Type;

	#define PINNAME_PSEN "/PSEN"
	#define PINNAME_ALE  "ALE"
	#define PINNAME_RESET "RESET"
	#define PINNAME_EA "/EA"

};

#endif
