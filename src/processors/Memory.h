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
#ifndef I51MEMORY_H
#define I51MEMORY_H

#include <functional>

#include <qdatastream.h>


//Muistin kantaluokka
class Memory
{
	public:
		Memory();
		virtual ~Memory();

		//Sijoitusoperaatiot
		//Sijoittaa arvot muuntavasti
		Memory& operator=(unsigned char const& c);
		Memory& operator=(Memory const& r);

		//Loogiset sijoitusoperaatiot
		Memory& operator|=(unsigned char const& c);
		Memory& operator&=(unsigned char const& c);
		Memory& operator^=(unsigned char const& c);

		//Laskuoperaatiot
		Memory& operator++(void);
		Memory& operator--(void);

		//Lasku Sijoitus
		virtual Memory& operator+=(Memory const& r);
		Memory& operator+=(unsigned char const& c);

		virtual Memory& operator-=(Memory const& r);
		Memory& operator-=(unsigned char const& c);

		//Bittioperaatiot
		Memory& operator<<=(unsigned char const& c);
		Memory& operator>>=(unsigned char const& c);

		void clear(void);
		unsigned char readValue(void); //Ei merkkaa ei muutetuksi. changed bitin luku isChanged() muuttaa.
		void writeValue(unsigned char const v, bool const changedAfterClearBit);//Ei merkkaa muuntuneeksi. Operointi merkkaa. Voidaan muutta changedAfterClear bittiä

		bool isChanged(void);
		bool isChangedAfterClear(void){return changedAfterClear_;};

		virtual void saveWorkDataToFile(QDataStream& saveFileStream);
		virtual void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead);

	protected:
		bool changed_;
		bool changedAfterClear_;
		unsigned char memoryValue_;

		void changedToTrue(void);

};

//-------------------------------------------------------------------------------------------------
class externalRAM:public Memory
{
	public:
		externalRAM();
		~externalRAM();
		bool update(void);
		externalRAM& operator=(unsigned char const& c);
		void writeValue(unsigned char const v, bool const changedAfterClearBit);
	private:
		bool upd;
};

//Memory Taulukkoluokka----------------------------------------------------------------------
class MemoryTable
{
	public:
		MemoryTable(void);
		MemoryTable(unsigned long const memorySize);
		virtual ~MemoryTable(void);

		unsigned long readSize(void);
		unsigned char readValue(unsigned int const address);
		virtual void writeValue(unsigned int const address, char const value, bool const changedAfterClearBit);
		virtual void setValue(unsigned int const address, unsigned char const value);
		bool isValueChanged(unsigned int& minChangedAddress, unsigned int& maxChangedAddress);
		bool isValueChanged(unsigned int const address); //Yksittäisen alkion tila vaihtunut

		bool isValueChangedAfterClear(unsigned int& minChangedAddress, unsigned int& maxChangedAddress);
		bool isValueChangedAfterClear(unsigned int const address); //Yksittäisen alkion tila vaihtunut

		void clear(void);

		virtual void saveWorkDataToFile(QDataStream& saveFileStream);
		virtual void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead);

	protected:

		Memory* mTable;
		unsigned long size;

	private:
		bool valueChanged;
		bool valueChangedAfterClear;
		unsigned int minChAddress;
		unsigned int maxChAddress;
		unsigned int minChAddressAfterClear;
		unsigned int maxChAddressAfterClear;
};

class externalDataMemoryTable : public  MemoryTable
{
	public:
		externalDataMemoryTable(unsigned long const memorySize);
		~externalDataMemoryTable(void);
		bool toUpdate(unsigned int& minUpdateAddress, unsigned int& maxUpdateAddress);
		bool toUpdate(unsigned int const address);
		void writeValue(unsigned int const address, char const value, bool const changedAfterClearBit) override;
		void setValue(unsigned int const address, unsigned char const value) override;

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	private:
		bool toUpd;
		bool* toUpdTable;
		unsigned int minUpdateA;
		unsigned int maxUpdateA;

};

#endif
