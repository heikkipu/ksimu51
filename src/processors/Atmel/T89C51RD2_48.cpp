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
#include "T89C51RD2_48.h"

T89C51RD2_48::T89C51RD2_48() : T89C51RD2x(T89C51RD2_48_NameS::INTERNAL_PROGRAM_MEMORY_SIZE, T89C51RD2_48_NameS::INTERNAL_XRAM_START_ADDRESS, T89C51RD2_48_NameS::INTERNAL_XRAM_MEMORY_SIZE)
{
	using namespace I8031_NameS;
	using namespace T89C51RD2_48_NameS;
	
	processorName_="T89C51RD2_48";
	createSFRNameTable();
	createSFRBitNameTable();

	delete externalCycleList_;
	externalCycleList_=new  T89C51RD2_48_ExternalCycle(pinList_.readIndex(PINNAME_ALE), pinList_.readIndex(PINNAME_PSEN), pinList_.readIndex(PINNAME_RESET), pinList_.readIndex(PINNAME_EA), P0, P1, P2, P3, P4, P5);
}

T89C51RD2_48::~T89C51RD2_48()
{
}

//UM resetti
void T89C51RD2_48::reset_VirtualPart(void)
{
	using namespace T89C51RD2_48_NameS;
	T89C51RD2x::reset_VirtualPart(); //Kantaluokkien resetit
	internalDataMemory_[P4]=0xFF;
	internalDataMemory_[P5]=0xFF;
}

void T89C51RD2_48::createSFRNameTable(void)
{
	using namespace T89C51RD2_48_NameS;
	//Pari uutta porttia
	setSFRInUse(P4, "P4", true);
	setSFRInUse(P5, "P5", true);
}

void T89C51RD2_48::createSFRBitNameTable(void)
{
	using namespace T89C51RD2_48_NameS;
	//P4:n bitit
	setSFRBitNameInUse(P4, BIT_NUMBER_0, "P4.0");
	setSFRBitNameInUse(P4, BIT_NUMBER_1, "P4.1");
	setSFRBitNameInUse(P4, BIT_NUMBER_2, "P4.2");
	setSFRBitNameInUse(P4, BIT_NUMBER_3, "P4.3");
	setSFRBitNameInUse(P4, BIT_NUMBER_4, "P4.4");
	setSFRBitNameInUse(P4, BIT_NUMBER_5, "P4.5");
	setSFRBitNameInUse(P4, BIT_NUMBER_6, "P4.6");
	setSFRBitNameInUse(P4, BIT_NUMBER_7, "P4.7");
	//P5:n bitit
	setSFRBitNameInUse(P5, BIT_NUMBER_0, "P5.0");
	setSFRBitNameInUse(P5, BIT_NUMBER_1, "P5.1");
	setSFRBitNameInUse(P5, BIT_NUMBER_2, "P5.2");
	setSFRBitNameInUse(P5, BIT_NUMBER_3, "P5.3");
	setSFRBitNameInUse(P5, BIT_NUMBER_4, "P5.4");
	setSFRBitNameInUse(P5, BIT_NUMBER_5, "P5.5");
	setSFRBitNameInUse(P5, BIT_NUMBER_6, "P5.6");
	setSFRBitNameInUse(P5, BIT_NUMBER_7, "P5.7");
}
