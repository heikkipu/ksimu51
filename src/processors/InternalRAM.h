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
#ifndef INTERNALRAM_H
#define INTERNALRAM_H

#include "Memory.h"

class InternalRAM : public Memory
{
	public:
		InternalRAM();
		~InternalRAM();

		InternalRAM& operator=(unsigned char const& c);

		void setInUse(bool const trueFalse){inUse_=trueFalse;};
		void setIsBitAddressable(bool const trueFalse){bitAddressable_=trueFalse;};
		void setIsBitNames(bool const trueFalse){bitNames_=trueFalse;};
		void setIsDirect(bool const trueFalse){direct_=trueFalse;};
		void setIsIndirect(bool const trueFalse){indirect_=trueFalse;};
		void setIsRegisterBank(bool const trueFalse){registerBank_=trueFalse;};
		void setIsSFR(bool const trueFalse){sfr_=trueFalse;};
		void setIsPort(bool const trueFalse){port_=trueFalse;};

		bool isInUse(void){return inUse_;};
		bool isBitAddressable(void){return bitAddressable_;};
		bool isBitNames(void){return bitNames_;};
		bool isDirect(void){return direct_;};
		bool isIndirect(void){return indirect_;};
		bool isRegisterBank(void){return registerBank_;};
		bool isSFR(void){return sfr_;};
		bool isPort(void){return port_;};

	private:

		bool inUse_;            //Käytössä
		bool bitAddressable_;            //Bittiosoitettava
		bool bitNames_;      //Bitit nimetty
		bool direct_;         //Suorasti osoitettava
		bool indirect_;      //Epäsuorasti osoitettava
		bool registerBank_;   //Rekisteripankki
		bool sfr_;            //Special Function register
		bool port_;         //SFR:n I/O portti rekkari
		unsigned char canReadBits_;      //Maski biteistä jotka voidaan lukea
		unsigned char canWriteBits_;   //Maski biteistä jotka voidaan muuttaa.


};

#endif
