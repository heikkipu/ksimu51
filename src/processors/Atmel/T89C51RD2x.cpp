/**************************************************************************
*   Copyright (C) 2005 - ... Heikki Pulkkinen                             *
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
#include "T89C51RD2x.h"

//Muodostin
T89C51RD2x::T89C51RD2x(unsigned long const flashSize, unsigned long const xramStartAddress, unsigned long const xramSize) : I80C52(), I8051_Flash(flashSize), I8031_Xram(xramStartAddress, xramSize)
{
	processorName_="T89C51RD2x";
	maxClockSpeed_=T89C51RD2x::MAX_CLOCKSPEED;
	//clockCyclesPerMachineCycle=4;
	createSFRBitNameTable();
	createSFRNameTable();

	clearProgramMemory();
}

T89C51RD2x::~T89C51RD2x(void)
{
}

void T89C51RD2x::reset_VirtualPart(void)
{
	using namespace T89C51RD2x_NameS;
	//Kantaluokkien resetit
	I80C52::reset_VirtualPart();
	I8051_Flash::reset_VirtualPart();
	I8031_Xram::reset_VirtualPart();
	
	internalDataMemory_[AUXR]=0x08;
	internalDataMemory_[CKCON]=0x00;
	internalDataMemory_[AUXR1]=0x00;
	internalDataMemory_[WDTRST]&=0xFF;
	internalDataMemory_[WDTPRG]&=0xF8;
	internalDataMemory_[SADDR]=0x00;
	internalDataMemory_[IPH]=0x00;
	internalDataMemory_[SADEN]=0x00;
	internalDataMemory_[FCON]&=0xF0;
	internalDataMemory_[EECON]&=0xFC;
	internalDataMemory_[EETIM]=0x00;
	internalDataMemory_[CCON]&=0x20;
	internalDataMemory_[CMOD]&=0x38;
	internalDataMemory_[CCAPM0]&=0x80;
	internalDataMemory_[CCAPM1]&=0x80;
	internalDataMemory_[CCAPM2]&=0x80;
	internalDataMemory_[CCAPM3]&=0x80;
	internalDataMemory_[CCAPM4]&=0x80;
	internalDataMemory_[CL]=0x00;
	internalDataMemory_[CCAP0L]&=0xFF;
	internalDataMemory_[CCAP1L]&=0xFF;
	internalDataMemory_[CCAP2L]&=0xFF;
	internalDataMemory_[CCAP3L]&=0xFF;
	internalDataMemory_[CCAP4L]&=0xFF;
	internalDataMemory_[CH]=0x00;
	internalDataMemory_[CCAP0H]&=0xFF;
	internalDataMemory_[CCAP1H]&=0xFF;
	internalDataMemory_[CCAP2H]&=0xFF;
	internalDataMemory_[CCAP3H]&=0xFF;
	internalDataMemory_[CCAP4H]&=0xFF;

}

unsigned int T89C51RD2x::priority0IntCheck(void)
{
}

unsigned int T89C51RD2x::priority1IntCheck(void)
{
}

bool T89C51RD2x::putInterruptServiceOn(unsigned int const interruptToPutOn)
{
}

bool T89C51RD2x::priority0IntReturn(void)
{
}

bool T89C51RD2x::priority1IntReturn(void)
{
}

/*unsigned char T89C51RD2x::interruptChecking(void)
{
	using namespace T89C51RD2x_NameS;
	unsigned char interruptToHandle=I80C52::interruptChecking();//Katsotaan kantaluokan keskeytykset
	if(!interruptToHandle)      //Jos ei keskytyksiä
	{
		//Timer2 prioriteetilla 0
		if(!(internalDataMemory_[IP].readValue()&IP_PT2) && (internalDataMemory_[IE].readValue()&IE_ET2) && (internalDataMemory_[T2CON].readValue()&T2CON_TF2) && !tf2P0_InterruptServiceIsOn_ && !serialReceiveP0_InterruptServiceIsOn_ && !serialTransmitP0_InterruptServiceIsOn_ && !tf1P0_InterruptServiceIsOn_ && !ie1P0_InterruptServiceIsOn_ && !tf0P0_InterruptServiceIsOn_ && !ie0P0_InterruptServiceIsOn_ && !tf2P1_InterruptServiceIsOn_ && !serialReceiveP1_InterruptServiceIsOn_ && !serialTransmitP1_InterruptServiceIsOn_ && !tf1P1_InterruptServiceIsOn_ && !ie1P1_InterruptServiceIsOn_ && !tf0P1_InterruptServiceIsOn_ && !ie0P1_InterruptServiceIsOn_)
			interruptToHandle=TF2P0IH;
	}
	if(interruptToHandle<0x80) //Jos kantaluokan 0 prioriteetin keskytys tai ei keskytyksiä
	{
		//Timer2 prioriteetilla 1
		if((internalDataMemory_[IP].readValue()&IP_PT2) && (internalDataMemory_[IE].readValue()&IE_ET2) && (internalDataMemory_[T2CON].readValue()&T2CON_TF2) && !tf2P1_InterruptServiceIsOn_ && !serialReceiveP1_InterruptServiceIsOn_ && !serialTransmitP1_InterruptServiceIsOn_ && !tf1P1_InterruptServiceIsOn_ && !ie1P1_InterruptServiceIsOn_ && !tf0P1_InterruptServiceIsOn_ && !ie0P1_InterruptServiceIsOn_)
			interruptToHandle=TF2P1IH;
	}
	return interruptToHandle;
}

//
void T89C51RD2x::putInterruptServiceOn(unsigned char const  interruptToHandle)
{
	using namespace T89C51RD2x_NameS;
	if(interruptToHandle!=0)
	{
		switch(interruptToHandle)
		{
			case TF2P0IH:
				push_pc();
				programCounter_=TF2_INTERRUPT_VECTOR;
				tf2P0_InterruptServiceIsOn_=true;
				break;
			case TF2P1IH:
				push_pc();
				programCounter_=TF2_INTERRUPT_VECTOR;
				tf2P1_InterruptServiceIsOn_=true;
				break;
			default:
				I80C52::putInterruptServiceOn(interruptToHandle); //Kantaluokan keskeytys
		}
		interruptStarted_=true;
	}
}

unsigned char T89C51RD2x::interruptOnChecking(void)
{
	using namespace T89C51RD2x_NameS;
	//Tarkistetaan alemmasta prioriteetista lähtien.
	//Paluu tapahtuu aina korkeammasta prioriteetista.
	unsigned char interruptIsOn=I80C52::interruptOnChecking();//Haetaan kantaluokan päälläoleva keskytys

	if(!interruptIsOn)//Jos kantaluokassa ei keskytyksiä päällä
	{
		//Timer2 prioriteetilla 0 Kaikista alin keskytys
		if(!(internalDataMemory_[IP].readValue()&IP_PT2) && tf2P0_InterruptServiceIsOn_ && !serialReceiveP0_InterruptServiceIsOn_ && !serialTransmitP0_InterruptServiceIsOn_ && !tf1P0_InterruptServiceIsOn_ && !ie1P0_InterruptServiceIsOn_ && !tf0P0_InterruptServiceIsOn_ && !ie0P0_InterruptServiceIsOn_ && !tf2P1_InterruptServiceIsOn_ && !serialReceiveP1_InterruptServiceIsOn_ && !serialTransmitP1_InterruptServiceIsOn_ && !tf1P1_InterruptServiceIsOn_ && !ie1P1_InterruptServiceIsOn_ && !tf0P1_InterruptServiceIsOn_ && !ie0P1_InterruptServiceIsOn_)
			interruptIsOn=TF2P0IH;
	}
	if(interruptIsOn<0x80) //Jos kantaluokassa 0 prioriteetin keskytys tai ei
	{
		//Timer2 prioriteetilla 1
		if((internalDataMemory_[IP].readValue()&IP_PT2) && tf2P1_InterruptServiceIsOn_ && !serialReceiveP1_InterruptServiceIsOn_ && !serialTransmitP1_InterruptServiceIsOn_ && !tf1P1_InterruptServiceIsOn_ && !ie1P1_InterruptServiceIsOn_ && !tf0P1_InterruptServiceIsOn_ && !ie0P1_InterruptServiceIsOn_)
			interruptIsOn=TF2P1IH;
	}
	return interruptIsOn;
}

void T89C51RD2x::returnFromInterrupt(unsigned char const  interruptReturnFrom)
{
	using namespace T89C51RD2x_NameS;
	if(interruptReturnFrom!=0)
	{
		switch(interruptReturnFrom)
		{
			case TF2P0IH:
				tf2P0_InterruptServiceIsOn_=false;
				pop_pc();
				break;
			case TF2P1IH:
				tf2P1_InterruptServiceIsOn_=false;
				pop_pc();
				break;
			default:
				I80C52::returnFromInterrupt(interruptReturnFrom); //Kantaluokan paluu
		}
	}
}
*/
void T89C51RD2x::createSFRBitNameTable(void)
{
	using namespace T89C51RD2x_NameS;
	//AUXR bitit
	setSFRBitNameInUse(AUXR, BIT_NUMBER_0, "A0");
	setSFRBitNameInUse(AUXR, BIT_NUMBER_1, "EXTRAM");
	setSFRBitNameInUse(AUXR, BIT_NUMBER_2, "XRS0");
	setSFRBitNameInUse(AUXR, BIT_NUMBER_3, "XRS1");
	setSFRBitNameInUse(AUXR, BIT_NUMBER_5, "M0");
	//CKCON bitit
	setSFRBitNameInUse(CKCON, BIT_NUMBER_0, "X2");
	setSFRBitNameInUse(CKCON, BIT_NUMBER_1, "T0X2");
	setSFRBitNameInUse(CKCON, BIT_NUMBER_2, "T1X2");
	setSFRBitNameInUse(CKCON, BIT_NUMBER_3, "T2X2");
	setSFRBitNameInUse(CKCON, BIT_NUMBER_4, "SiX2");
	setSFRBitNameInUse(CKCON, BIT_NUMBER_5, "PcaX2");
	setSFRBitNameInUse(CKCON, BIT_NUMBER_6, "WdX2");
	//AUXR1 bitit
	setSFRBitNameInUse(AUXR1, BIT_NUMBER_0, "DPS");
	setSFRBitNameInUse(AUXR1, BIT_NUMBER_2, "0");
	setSFRBitNameInUse(AUXR1, BIT_NUMBER_3, "GF3");
	//WDTPRG bitit
	setSFRBitNameInUse(WDTPRG, BIT_NUMBER_0, "S0");
	setSFRBitNameInUse(WDTPRG, BIT_NUMBER_1, "S1");
	setSFRBitNameInUse(WDTPRG, BIT_NUMBER_2, "S2");
	setSFRBitNameInUse(WDTPRG, BIT_NUMBER_3, "T0");
	setSFRBitNameInUse(WDTPRG, BIT_NUMBER_4, "T1");
	setSFRBitNameInUse(WDTPRG, BIT_NUMBER_5, "T2");
	setSFRBitNameInUse(WDTPRG, BIT_NUMBER_6, "T3");
	setSFRBitNameInUse(WDTPRG, BIT_NUMBER_7, "T4");
	//Tämän prosesssorin IE bitit
	setSFRBitNameInUse(IE, BIT_NUMBER_6, "EC");
	//Tämän processorin IP bitit
	setSFRBitNameInUse(IP, BIT_NUMBER_6, "PPC");
	//IPH bitit
	setSFRBitNameInUse(IPH, BIT_NUMBER_0, "PX0H");
	setSFRBitNameInUse(IPH, BIT_NUMBER_1, "PT0H");
	setSFRBitNameInUse(IPH, BIT_NUMBER_2, "PX1H");
	setSFRBitNameInUse(IPH, BIT_NUMBER_3, "PT1H");
	setSFRBitNameInUse(IPH, BIT_NUMBER_4, "PSH");
	setSFRBitNameInUse(IPH, BIT_NUMBER_5, "PT2H");
	setSFRBitNameInUse(IPH, BIT_NUMBER_6, "PPCH");
	//FCON bitit
	setSFRBitNameInUse(FCON, BIT_NUMBER_0, "FBUSY");
	setSFRBitNameInUse(FCON, BIT_NUMBER_1, "FMOD0");
	setSFRBitNameInUse(FCON, BIT_NUMBER_2, "FMOD1");
	setSFRBitNameInUse(FCON, BIT_NUMBER_3, "FPS");
	setSFRBitNameInUse(FCON, BIT_NUMBER_4, "FPL0");
	setSFRBitNameInUse(FCON, BIT_NUMBER_5, "FPL1");
	setSFRBitNameInUse(FCON, BIT_NUMBER_6, "FPL2");
	setSFRBitNameInUse(FCON, BIT_NUMBER_7, "FPL3");
	//EECON bitit
	setSFRBitNameInUse(EECON, BIT_NUMBER_0, "EEBUSY");
	setSFRBitNameInUse(EECON, BIT_NUMBER_1, "EEE");
	setSFRBitNameInUse(EECON, BIT_NUMBER_4, "EEPL0");
	setSFRBitNameInUse(EECON, BIT_NUMBER_5, "EEPL1");
	setSFRBitNameInUse(EECON, BIT_NUMBER_6, "EEPL2");
	setSFRBitNameInUse(EECON, BIT_NUMBER_7, "EEPL3");
	//CCON bitit
	setSFRBitNameInUse(CCON, BIT_NUMBER_0, "CCF0");
	setSFRBitNameInUse(CCON, BIT_NUMBER_1, "CCF1");
	setSFRBitNameInUse(CCON, BIT_NUMBER_2, "CCF2");
	setSFRBitNameInUse(CCON, BIT_NUMBER_3, "CCF3");
	setSFRBitNameInUse(CCON, BIT_NUMBER_4, "CCF4");
	setSFRBitNameInUse(CCON, BIT_NUMBER_6, "CR");
	setSFRBitNameInUse(CCON, BIT_NUMBER_7, "CF");
	//CMOD bitit
	setSFRBitNameInUse(CMOD, BIT_NUMBER_0, "ECF");
	setSFRBitNameInUse(CMOD, BIT_NUMBER_1, "CPS0");
	setSFRBitNameInUse(CMOD, BIT_NUMBER_2, "CPS1");
	setSFRBitNameInUse(CMOD, BIT_NUMBER_6, "WDTE");
	setSFRBitNameInUse(CMOD, BIT_NUMBER_7, "CIDL");
	//CCAPM0 bitit
	setSFRBitNameInUse(CCAPM0, BIT_NUMBER_0, "ECCF0");
	setSFRBitNameInUse(CCAPM0, BIT_NUMBER_1, "PWM0");
	setSFRBitNameInUse(CCAPM0, BIT_NUMBER_2, "TOG0");
	setSFRBitNameInUse(CCAPM0, BIT_NUMBER_3, "MAT0");
	setSFRBitNameInUse(CCAPM0, BIT_NUMBER_4, "CAPN0");
	setSFRBitNameInUse(CCAPM0, BIT_NUMBER_5, "CAPP0");
	setSFRBitNameInUse(CCAPM0, BIT_NUMBER_6, "ECOM0");
	//CCAPM1 bitit
	setSFRBitNameInUse(CCAPM1, BIT_NUMBER_0, "ECCF1");
	setSFRBitNameInUse(CCAPM1, BIT_NUMBER_1, "PWM1");
	setSFRBitNameInUse(CCAPM1, BIT_NUMBER_2, "TOG1");
	setSFRBitNameInUse(CCAPM1, BIT_NUMBER_3, "MAT1");
	setSFRBitNameInUse(CCAPM1, BIT_NUMBER_4, "CAPN1");
	setSFRBitNameInUse(CCAPM1, BIT_NUMBER_5, "CAPP1");
	setSFRBitNameInUse(CCAPM1, BIT_NUMBER_6, "ECOM1");
	//CCAPM2 bitit
	setSFRBitNameInUse(CCAPM2, BIT_NUMBER_0, "ECCF2");
	setSFRBitNameInUse(CCAPM2, BIT_NUMBER_1, "PWM2");
	setSFRBitNameInUse(CCAPM2, BIT_NUMBER_2, "TOG2");
	setSFRBitNameInUse(CCAPM2, BIT_NUMBER_3, "MAT2");
	setSFRBitNameInUse(CCAPM2, BIT_NUMBER_4, "CAPN2");
	setSFRBitNameInUse(CCAPM2, BIT_NUMBER_5, "CAPP2");
	setSFRBitNameInUse(CCAPM2, BIT_NUMBER_6, "ECOM2");
	//CCAPM3 bitit
	setSFRBitNameInUse(CCAPM3, BIT_NUMBER_0, "ECCF3");
	setSFRBitNameInUse(CCAPM3, BIT_NUMBER_1, "PWM3");
	setSFRBitNameInUse(CCAPM3, BIT_NUMBER_2, "TOG3");
	setSFRBitNameInUse(CCAPM3, BIT_NUMBER_3, "MAT3");
	setSFRBitNameInUse(CCAPM3, BIT_NUMBER_4, "CAPN3");
	setSFRBitNameInUse(CCAPM3, BIT_NUMBER_5, "CAPP3");
	setSFRBitNameInUse(CCAPM3, BIT_NUMBER_6, "ECOM3");
	//CCAPM4 bitit
	setSFRBitNameInUse(CCAPM4, BIT_NUMBER_0, "ECCF4");
	setSFRBitNameInUse(CCAPM4, BIT_NUMBER_1, "PWM4");
	setSFRBitNameInUse(CCAPM4, BIT_NUMBER_2, "TOG4");
	setSFRBitNameInUse(CCAPM4, BIT_NUMBER_3, "MAT4");
	setSFRBitNameInUse(CCAPM4, BIT_NUMBER_4, "CAPN4");
	setSFRBitNameInUse(CCAPM4, BIT_NUMBER_5, "CAPP4");
	setSFRBitNameInUse(CCAPM4, BIT_NUMBER_6, "ECOM4");
}

void T89C51RD2x::createSFRNameTable(void)
{
	using namespace T89C51RD2x_NameS;
	setSFRInUse(AUXR, "AUXR", false);
	setSFRInUse(CKCON, "CKCON", false);
	setSFRInUse(AUXR1, "AUXR1", false);
	setSFRInUse(WDTRST, "WDTRST", false);
	setSFRInUse(WDTPRG, "WDTPRG", false);
	setSFRInUse(SADDR, "SADDR", false);
	setSFRInUse(IPH, "IPH", false);
	setSFRInUse(SADEN, "SADEN", false);
	setSFRInUse(FCON, "FCON", false);
	setSFRInUse(EECON, "EECON", false);
	setSFRInUse(EETIM, "EETIM", false);
	setSFRInUse(CCON, "CCON", false);
	setSFRInUse(CMOD, "CMOD", false);
	setSFRInUse(CCAPM0, "CCAPM0", false);
	setSFRInUse(CCAPM1, "CCAPM1", false);
	setSFRInUse(CCAPM2, "CCAPM2", false);
	setSFRInUse(CCAPM3, "CCAPM3", false);
	setSFRInUse(CCAPM4, "CCAPM4", false);
	setSFRInUse(CL, "CL", false);
	setSFRInUse(CCAP0L, "CCAP0L", false);
	setSFRInUse(CCAP1L, "CCAP1L", false);
	setSFRInUse(CCAP2L, "CCAP2L", false);
	setSFRInUse(CCAP3L, "CCAP3L", false);
	setSFRInUse(CCAP4L, "CCAP4L", false);
	setSFRInUse(CH, "CH", false);
	setSFRInUse(CCAP0H, "CCAP0H", false);
	setSFRInUse(CCAP1H, "CCAP1H", false);
	setSFRInUse(CCAP2H, "CCAP2H", false);
	setSFRInUse(CCAP3H, "CCAP3H", false);
	setSFRInUse(CCAP4H, "CCAP4H", false);
}

void T89C51RD2x::saveWorkDataToFile(QDataStream& saveFileStream)
{
	I80C52::saveWorkDataToFile(saveFileStream);      //Kantaluokan tiedot talteen
	
	//XRAM
	xram_->saveWorkDataToFile(saveFileStream);
}

void T89C51RD2x::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	I80C52::readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);      //Kantaluokasta luku

	Q_UINT8 value8;

	//XRAM
	xram_->readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);
}
