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
#include "I8031_Xram.h"

I8031_Xram::I8031_Xram(unsigned long const startAddress, unsigned long const xramSize)
{
	xram_=new MemoryTable(xramSize);
	startAddress_=startAddress;
}

I8031_Xram::~I8031_Xram()
{
	delete xram_;
}

void I8031_Xram::writeInternalXRAMValue(unsigned int const  address, unsigned char value)
{
	xram_->setValue(address, value);
}

unsigned char I8031_Xram::readInternalXRAMValue(unsigned int const  address)
{
	return xram_->readValue(address);
}

unsigned long I8031_Xram::readInternalXRAMSize(unsigned int & startAddress)
{
	startAddress=startAddress_;
	return xram_->readSize();
}

bool I8031_Xram::isInternalXRAMValueChanged(unsigned int const  address)
{
	return xram_->isValueChanged(address);
}

bool I8031_Xram::isInternalXRAMValueChanged(unsigned int& minAddress, unsigned int& maxAddress)
{
	return xram_->isValueChanged(minAddress, maxAddress);
}

