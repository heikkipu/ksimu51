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
// Johdettu luokasta i8031
#ifndef I8032_H
#define I8032_H


#include "I8031.h"
#include "I8032_NameS.h"
#include "InternalRAM.h"
#include "TimersCounters2.h"

class I8032: virtual public I8031
{
	public:

		I8032();
		~I8032();

		//Haussa ja kirjoituksessa käytettävä oikeita osoitevälejä 0x80 - 0xFF
		unsigned char readInternalIndirectDataMemory(unsigned int const  address);
		void writeInternalIndirectDataMemory(unsigned int const  address, unsigned char const  value);
		bool isInternalIndirectDataMemoryChanged(unsigned int const  address);
		bool isInternalIndirectDataMemoryChangedAfterClear(unsigned int const  address);

		unsigned int readTimerValue16(char const  timerNumber) override;

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	protected:
		void reset_VirtualPart(void) override;
		void powerOnReset_VirtualPart(void) override;      //Tehdään syväresetti, clearaa kaikki alueet

		//Keskeytysten käsittely päällä
		//Timer2
		bool tf2P0_InterruptServiceIsOn_;
		bool tf2P1_InterruptServiceIsOn_;

		InternalRAM internalIndirectDataMemory_[I8032_NameS::INTERNAL_INDIRECT_RAM_SIZE];

		//Uudellen määritetyt keskeytyskäsittelijät
		unsigned int priority0IntCheck(void) override;
		unsigned int priority1IntCheck(void) override;
		bool putInterruptServiceOn(unsigned int const interruptToPutOn) override;
		//Sekä keskytyksistä paluut
		bool priority0IntReturn(void) override;
		bool priority1IntReturn(void) override;

		void pop_pc(void) override; //Uudellenmääritelty koska lisä indirect rammi
		void push_pc(void) override;//jota voidaan käyttää

		//Uudelleenmääritetyt epäsuorat sisäisen muistin haut
		unsigned int readDataFromInternalDataMemoryIndirectlyUsingRBRegister(unsigned int r) override;
		void writeDataToInternalDataMemoryIndirectlyUsingRBRegister(unsigned int r, unsigned int val) override;

		//Uudellen määritys: lisätty Timer2
		//void timersAndCountersHandling(void);

		//Uudellenmääritys: Serial timer2 käyttö baud rate geniksenä
		void serialPortRxTxHandling(void) override;

		//8051 käskyjen käsittely
		//pus ja pop voivat osoittaa myös alueella 0x80 - 0xFF
		void push(unsigned char dpush) override;
		void pop(unsigned char dpop) override;

	private:
		void formatInternalIndirectDataMemory(void);
		void createSFRBitNameTable(void);
		void createSFRNameTable(void);

		void clearInternalIndirectDataMemory(void);

};


#endif
