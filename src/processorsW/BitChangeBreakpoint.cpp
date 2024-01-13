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
#include "BitChangeBreakpoint.h"

//BitChangeBreakpoint
BitChangeBreakpoint::BitChangeBreakpoint()
{
}

BitChangeBreakpoint::~BitChangeBreakpoint()
{
}

bool BitChangeBreakpoint::toggleBreakpoint(unsigned int const bitAddress, bool const currentValue)
{
	if(!breakpointBitList_.empty())
	{
		QValueList<unsigned int>::iterator it;
		for(it=breakpointBitList_.begin(); it!=breakpointBitList_.end();++it)
		{
			unsigned int iv=*it;
			if(bitAddress==(iv&0xFF0F))
			{
				it=breakpointBitList_.remove(it);
				return false;
			}
		}
	}
	unsigned int valueToSet=bitAddress;
	if(currentValue)
		valueToSet|=0x0010;
	breakpointBitList_.append(valueToSet);
	return true;
}

unsigned int BitChangeBreakpoint::isBreakpointsAtBitChange(I8031* const processor)
{
	breakpoint_=NO_BITCHANGES;
	if(!breakpointBitList_.empty())
	{
		QValueList<unsigned int>::iterator it;
		for(it=breakpointBitList_.begin(); it!=breakpointBitList_.end();++it)
		{
			unsigned int iv=*it;
			unsigned char pval=processor->readInternalDataMemory(iv>>8);
			pval>>=iv&0x000F;
			pval&=0x1;
			if(pval!=((iv>>4)&0x1))
			{
				if(pval)
					*it|=0x0010;
				else
					*it&=0xFF0F;
				breakpoint_=iv;
			}
		}
	}
	return breakpoint_;
}

bool BitChangeBreakpoint::isBitBreakpointBit(unsigned int const& bitAddress)
{
	if(!breakpointBitList_.empty())
	{
		QValueList<unsigned int>::iterator it;
		for(it=breakpointBitList_.begin(); it!=breakpointBitList_.end();++it)
		{
			unsigned int iv=*it;
			if(bitAddress==(iv&0xFF0F))
				return true;
		}

	}
	return false;
}
