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
#include "InternalRAM.h"

InternalRAM::InternalRAM()
{
	inUse_=false;
	bitAddressable_=false;
	bitNames_=false;
	direct_=false;
	indirect_=false;
	registerBank_=false;
	sfr_=false;
	port_=false;
	canReadBits_=0xFF;
	canWriteBits_=0xFF;
}

InternalRAM::~InternalRAM()
{
}

InternalRAM& InternalRAM::operator=(unsigned char const& c)
{
	changedToTrue();
	memoryValue_=c;
	return *this;
}

