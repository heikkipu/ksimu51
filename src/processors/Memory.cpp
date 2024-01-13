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
#include "Memory.h"
#include "../ksimu51.h"


Memory::Memory(void)
{
	changed_=false;
	changedAfterClear_=false;
	memoryValue_=0;
}

Memory::~Memory(void)
{
}

inline void Memory::changedToTrue(void)
{
	changed_=true;
	changedAfterClear_=true;
}

void Memory::clear(void)
{
	changed_=false;
	changedAfterClear_=false;
	memoryValue_=0;
}

bool Memory::isChanged(void)
{
	bool c=changed_;
	changed_=false;
	return c;
}

unsigned char Memory::readValue(void)
{
	return memoryValue_;
}

void Memory::writeValue(unsigned char const v, bool const changedAfterClearBit)
{
	memoryValue_=v;
	changedAfterClear_=changedAfterClearBit;
}

Memory& Memory::operator=(Memory const& r)
{
	changedToTrue();
	memoryValue_=r.memoryValue_;
	return *this;
}

Memory& Memory::operator=(unsigned char const& c)
{
	changedToTrue();
	memoryValue_=c;
	return *this;
}

Memory& Memory::operator|=(unsigned char const& c)
{
	changedToTrue();
	memoryValue_|=c;
	return *this;
}

Memory& Memory::operator&=(unsigned char const& c)
{
	changedToTrue();
	memoryValue_&=c;
	return *this;
}

Memory& Memory::operator^=(unsigned char const& c)
{
	changedToTrue();
	memoryValue_^=c;
	return *this;
}

Memory& Memory::operator++(void)
{
	changedToTrue();
	memoryValue_++;
	return *this;
}

Memory& Memory::operator--(void)
{
	changedToTrue();
	memoryValue_--;
	return *this;
}

Memory& Memory::operator+=(Memory const& r)
{
	changedToTrue();
	memoryValue_=static_cast<unsigned char>(memoryValue_+r.memoryValue_);
	return *this;
}

Memory& Memory::operator+=(unsigned char const& c)
{
	changedToTrue();
	memoryValue_=static_cast<unsigned char>(memoryValue_+c);
	return *this;
}

Memory& Memory::operator-=(Memory const& r)
{
	changedToTrue();
	memoryValue_=static_cast<unsigned char>(memoryValue_-r.memoryValue_);
	return *this;
}

Memory& Memory::operator-=(unsigned char const& c)
{
	changedToTrue();
	memoryValue_=static_cast<unsigned char>(memoryValue_-c);
	return *this;
}

Memory& Memory::operator<<=(unsigned char const& c)
{
	changedToTrue();
	memoryValue_=static_cast<unsigned char>(memoryValue_<<c);
	return *this;
}

Memory& Memory::operator>>=(unsigned char const& c)
{
	changedToTrue();
	memoryValue_=static_cast<unsigned char>(memoryValue_>>c);
	return *this;
}

void Memory::saveWorkDataToFile(QDataStream& saveFileStream)
{
	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)changed_;
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)changedAfterClear_;
	saveFileStream<<(Q_UINT16)2<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)memoryValue_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void Memory::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	Q_UINT8 value8Bool;

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case 0:
				readFileStream>>typeReadData>>value8Bool;
				changed_=(bool)value8Bool;
				break;
			case 1:
				readFileStream>>typeReadData>>value8Bool;
				changedAfterClear_=(bool)value8Bool;
				break;
			case 2:
				readFileStream>>typeReadData>>value8Bool;
				memoryValue_=(unsigned char)value8Bool;
				break;
			case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
				done=true;
				break;
			default:
				done = !funcHandleUnknownIndexRead(readFileStream);
		}
	}
	while(!done);
}

//External RAM periytymä----------------------------------------------------------------
externalRAM::externalRAM(void)
{
	upd=false;
}

externalRAM::~externalRAM()
{
}

bool externalRAM::update(void)
{
	bool u=upd;
	upd=false;
	return u;
}

externalRAM& externalRAM::operator=(unsigned char const& c)
{
	upd=true;      //Laitetaan myös päivitettäväksi
	changedToTrue();
	memoryValue_=c;
	return *this;
}

void externalRAM::writeValue(unsigned char const v, bool const changedAfterClearBit)
{
	upd=true;
	Memory::writeValue(v, changedAfterClearBit);
}

//Memory Table -------------------------------------------------------------------------------
MemoryTable::MemoryTable(void)
{
	valueChanged=false;
	valueChangedAfterClear=false;
	size=1; //Oletuksena yhden alkion taulukko
	mTable = new Memory[size];
	minChAddress=0;
	maxChAddress=0;
	minChAddressAfterClear=0;
	maxChAddressAfterClear=0;
}

MemoryTable::MemoryTable(unsigned long const memorySize)
{
	valueChanged=false;
	valueChangedAfterClear=false;
	size=memorySize;
	mTable = new Memory[size];
	minChAddress=size-1;
	maxChAddress=0;
	minChAddressAfterClear=size-1;
	maxChAddressAfterClear=0;
}

MemoryTable::~MemoryTable(void)
{
}

unsigned long MemoryTable::readSize(void)
{
	return size;
}

void MemoryTable::clear(void)
{
	valueChangedAfterClear=false;
	minChAddressAfterClear=size-1;
	maxChAddressAfterClear=0;
	for(unsigned int n=0; n<size; n++)
		mTable[n].clear();
}

unsigned char MemoryTable::readValue(unsigned int const address)
{
	if(address<size)
		return mTable[address].readValue();
	else
		return 0;
}

void MemoryTable::writeValue(unsigned int const address, char const value, bool const changedAfterClearBit)
{
	if(address<size)
		mTable[address].writeValue(value, changedAfterClearBit);
}

//Laittaa arvon muuntobittejä muuntaen
void MemoryTable::setValue(unsigned int const address, unsigned char const value)
{
	if(address<size)
	{
		mTable[address]=value;
		valueChanged=true;
		if(address<minChAddress)
			minChAddress=address;
		if(address>maxChAddress)
			maxChAddress=address;
		valueChangedAfterClear=true;
	}
}

bool MemoryTable::isValueChanged(unsigned int& minChangedAddress, unsigned int& maxChangedAddress)
{
	if(valueChanged)
	{
		valueChanged=false;
		minChangedAddress=minChAddress;
		maxChangedAddress=maxChAddress;
		minChAddress=size-1;
		maxChAddress=0;
		return true;
	}
	else
		return false;
}

//Yksittäisen alkion tila vaihtunut
bool MemoryTable::isValueChanged(unsigned int const address)
{
	if(address<size)
		return mTable[address].isChanged();
	else
		return false;
}

bool MemoryTable::isValueChangedAfterClear(unsigned int& minChangedAddress, unsigned int& maxChangedAddress)
{
	if(valueChangedAfterClear)
	{
		minChangedAddress=minChAddressAfterClear;
		maxChangedAddress=maxChAddressAfterClear;
		minChAddressAfterClear=size-1;
		maxChAddressAfterClear=0;
		return true;
	}
	else
		return false;
}

bool MemoryTable::isValueChangedAfterClear(unsigned int const address)
{
	if(address<size)
		return mTable[address].isChangedAfterClear();
	else
		return false;
}

void MemoryTable::saveWorkDataToFile(QDataStream& saveFileStream)
{
	saveFileStream<<(Q_UINT16)2<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)size;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;

	for(uint n=0; n<size; n++)
		mTable[n].saveWorkDataToFile(saveFileStream);

	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)valueChanged;
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)valueChangedAfterClear;
	saveFileStream<<(Q_UINT16)2<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)minChAddress;
	saveFileStream<<(Q_UINT16)3<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)maxChAddress;
	saveFileStream<<(Q_UINT16)4<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)minChAddressAfterClear;
	saveFileStream<<(Q_UINT16)5<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)maxChAddressAfterClear;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void MemoryTable::readWorkDataFromFile(QDataStream& readFileStream,
									   std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	Q_UINT32 value32;
	Q_UINT8 value8;

	readFileStream>>indexReadData>>typeReadData>>value32;
	size=(unsigned long)value32;
	readFileStream>>indexReadData; //INDEX_SAVEDWORKDATA_END

	for(uint n=0; n<size; n++)
		mTable[n].readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case 0:
				readFileStream>>typeReadData>>value8;
				valueChanged=(bool)value8;
				break;
			case 1:
				readFileStream>>typeReadData>>value8;
				valueChangedAfterClear=(bool)value8;
				break;
			case 2:
				readFileStream>>typeReadData>>value32;
				minChAddress=(unsigned int)value32;
				break;
			case 3:
				readFileStream>>typeReadData>>value32;
				maxChAddress=(unsigned int)value32;
				break;
			case 4:
				readFileStream>>typeReadData>>value32;
				minChAddressAfterClear=(unsigned int)value32;
				break;
			case 5:
				readFileStream>>typeReadData>>value32;
				maxChAddressAfterClear=(unsigned int)value32;
				break;
			case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
				done=true;
				break;
			default:
				done = !funcHandleUnknownIndexRead(readFileStream);
		}
	}
	while(!done);
}

//external RAM Table malli periytymä-------------------------------------------------------------
externalDataMemoryTable::externalDataMemoryTable(unsigned long const memorySize) : MemoryTable(memorySize)
{
	toUpdTable=new bool[size];
	toUpd=false;
	for(unsigned long n=0;n<size;n++)
		toUpdTable[n]=false;
	minUpdateA=size-1;
	maxUpdateA=0;
}

externalDataMemoryTable::~externalDataMemoryTable(void)
{
}

bool externalDataMemoryTable::toUpdate(unsigned int& minUpdateAddress, unsigned int& maxUpdateAddress)
{
	if(toUpd)
	{
		minUpdateAddress=minUpdateA;
		maxUpdateAddress=maxUpdateA;
		toUpd=false;
		minUpdateA=size-1;
		maxUpdateA=0;
		return true;
	}
	else
		return false;
}

bool externalDataMemoryTable::toUpdate(unsigned int const address)
{
	if(toUpdTable[address])
	{
		toUpdTable[address]=false;
		return true;
	}
	else
		return false;
}

void externalDataMemoryTable::writeValue(unsigned int const address, char const value, bool const changedAfterClearBit)
{
	if(address<size)
	{
		if(address<minUpdateA)
			minUpdateA=address;
		if(address>maxUpdateA)
			maxUpdateA=address;
		toUpd=true;
		toUpdTable[address]=true;
		MemoryTable::writeValue(address, value, changedAfterClearBit);
	}
}

void externalDataMemoryTable::setValue(unsigned int const address, unsigned char const value)
{
	if(address<size)
	{
		if(address<minUpdateA)
			minUpdateA=address;
		if(address>maxUpdateA)
			maxUpdateA=address;
		toUpd=true;
		toUpdTable[address]=true;
		MemoryTable::setValue(address, value);
	}
}

void externalDataMemoryTable::saveWorkDataToFile(QDataStream& saveFileStream)
{
	MemoryTable::saveWorkDataToFile(saveFileStream); //Kantaluokan tallennus

	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)toUpd;
	for(unsigned long n=0; n<size; n++)
		saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)toUpdTable[n];

	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)minUpdateA;
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)maxUpdateA;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void externalDataMemoryTable::readWorkDataFromFile(QDataStream& readFileStream,
												   std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	MemoryTable::readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);

	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	Q_UINT32 value32;
	Q_UINT8 value8;

	readFileStream>>indexReadData>>typeReadData>>value8; toUpd=(bool)value8;
	for(unsigned long n=0; n<size; n++)
	{
		readFileStream>>indexReadData>>typeReadData>>value8; toUpdTable[n]=(bool)value8;
	}

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case 0:
				readFileStream>>typeReadData>>value32;
				minUpdateA=(unsigned int)value32;
				break;
			case 1:
				readFileStream>>typeReadData>>value32;
				maxUpdateA=(unsigned int)value32;
				break;
			case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
				done=true;
				break;
			default:
				done = !funcHandleUnknownIndexRead(readFileStream);
		}
	}
	while(!done);
}
