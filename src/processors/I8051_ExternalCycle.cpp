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
#include "I8051_ExternalCycle.h"

I8051_ExternalCycle::I8051_ExternalCycle(IOPinList::PinIndex const aleIndex, IOPinList::PinIndex const psenIndex, IOPinList::PinIndex const resetIndex, IOPinList::PinIndex const eaIndex, unsigned char p0Address, unsigned char const p1Address, unsigned char const p2Address, unsigned char const p3Address) : ExternalCycle(aleIndex, psenIndex, resetIndex, eaIndex, p0Address, p1Address, p2Address, p3Address)
{
}

I8051_ExternalCycle::~I8051_ExternalCycle()
{
}

I8031_NameS::alePulses_Type I8051_ExternalCycle::appendToExternalCycle_nonPSENPulseCycle(I8031_NameS::programCounter_Type programCounter)
{
	//1.CLK STATE S1||S4 P1
	cycles_.writePinValue(aleIndex_, 0);
	cycles_.writePinValue(psenIndex_, 1);
	cycles_.addWrittenValuesToVector(1);

	//2.CLK STATE S1||S4 P2
	cycles_.writePinValue(aleIndex_, 1);
	if(lastCycleWasWriteDataMemory_)
		cycles_.writePortValue(p0Address_, programCounter&0x00ff, 0xff);
	cycles_.addWrittenValuesToVector(1);

	//3.CLK STATE S2||S5 P1
	cycles_.writePortToSample(0, p0Address_);
	cycles_.addWrittenValuesToVector(1);

	//4.CLK STATE S2||S5 P2, 5.CLK STATE S3||S6 P1, 6.CLK STATE S3||S6 P2
	cycles_.writePinValue(aleIndex_, 0);
	cycles_.writePortToSample(1, p1Address_);
	cycles_.writePortToSample(2, p2Address_);
	cycles_.writePortToSample(3, p3Address_);
	cycles_.addWrittenValuesToVector(3); 


	lastCyclePSENEndState_=1;
	lastCycleWasWriteDataMemory_=false;
	return 1;

}
