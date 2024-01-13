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
//CMOS version 8031
//Lisänä PCON rekkari
#ifndef I80C31_H
#define I80C31_H

#include "I8031.h"
#include "I80C31_NameS.h"
#include "I8031_NameS.h"

class I80C31: virtual public I8031
{
	public:
		I80C31();
		virtual ~I80C31();

		//Uudelleen määritetty, koska IDLE ja POWER DOWN MODET
		void step(bool const disassemble, bool const run) override;

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;

	protected:
		void reset_VirtualPart(void) override;
		
		//Uudelleenmääritys koska SMOD bitti kolla voidaan nopeuttaa baudeja
		//Lähetetään sarjaportista, tekee keskytyksen TI
		void serialPortRxTxHandling(void) override;

	private:
		bool idleMode_;      //Keskeytys tai resetti lopettaa idle moden
		bool powerDownMode_; //Vain resetti poistaa
		I8031_NameS::programCounter_Type programCounterValueInPowerSaveMode_;

		void createSFRBitNameTable(void);
		void createSFRNameTable(void);

};

#endif
