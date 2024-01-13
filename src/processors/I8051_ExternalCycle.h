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
#ifndef I8051_EXTERNALCYCLE_H
#define I8051_EXTERNALCYCLE_H

#include "ExternalCycle.h"

class I8051_ExternalCycle : public ExternalCycle
{
	public:
		I8051_ExternalCycle(IOPinList::PinIndex const aleIndex, IOPinList::PinIndex const psenIndex, IOPinList::PinIndex const resetIndex, IOPinList::PinIndex const eaIndex, unsigned char p0Address, unsigned char const p1Address, unsigned char const p2Address, unsigned char const p3Address);
		~I8051_ExternalCycle();

		virtual I8031_NameS::alePulses_Type appendToExternalCycle_nonPSENPulseCycle(I8031_NameS::programCounter_Type programCounter);
		
};

#endif
