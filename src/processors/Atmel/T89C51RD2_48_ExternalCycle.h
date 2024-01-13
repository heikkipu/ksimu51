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
#ifndef T89C51RD2_48_EXTERNALCYCLE_H
#define T89C51RD2_48_EXTERNALCYCLE_H

#include "../I8051_ExternalCycle.h"

class T89C51RD2_48_ExternalCycle : public I8051_ExternalCycle
{
	public:
		T89C51RD2_48_ExternalCycle(IOPinList::PinIndex const aleIndex, IOPinList::PinIndex const psenIndex, IOPinList::PinIndex const resetIndex, IOPinList::PinIndex const eaIndex, unsigned char p0Address, unsigned char const p1Address, unsigned char const p2Address, unsigned char const p3Address, unsigned char const p4Address, unsigned char const p5Address);
		~T89C51RD2_48_ExternalCycle();
		

	private:
		unsigned char p4Address_;
		unsigned char p5Address_;
};

#endif
