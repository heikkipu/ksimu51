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
#ifndef T89C51RD2X_H
#define T89C51RD2X_H

#include "../I80C52.h"
#include "../I8051_Flash.h"
#include "../I8031_Xram.h"
#include "T89C51RD2x_NameS.h"

#include<cmath>

class T89C51RD2x:  public I80C52, public I8051_Flash, public I8031_Xram
{
	public:
		T89C51RD2x(unsigned long const flashSize, unsigned long const xramStartAddress, unsigned long const xramSize);
		virtual ~T89C51RD2x();

		//UM koska uusia rutiineja:WATCHDOG, AD, FLASH

		//XRAM
		void writeInternalXRAMValue(unsigned int const  address, unsigned char value);
		unsigned char readInternalXRAMValue(unsigned int const  address);
		unsigned long readInternalXRAMSize(unsigned int & startAddress);
		bool isInternalXRAMValueChanged(unsigned int const  address);
		bool isInternalXRAMValueChanged(unsigned int& minAddress, unsigned int& maxAddress);

		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;




	protected:
		void reset_VirtualPart(void) override;

		static unsigned long const MAX_CLOCKSPEED=60000000;//Vaihtelee moden mukaan, tämä maksimi

		//Sarjaportti uudelleenmääritys:

		//Uudellen määritetyt keskeytyskäsittelijät. Lisätty:
		//PCA ja 4 tasoinen keskytysprioriteetti
		//Uudellen määritetyt keskeytyskäsittelijät
		unsigned int priority0IntCheck(void) override;
		unsigned int priority1IntCheck(void) override;
		bool putInterruptServiceOn(unsigned int const interruptToPutOn) override;
		//Sekä keskytyksistä paluut
		bool priority0IntReturn(void) override;
		bool priority1IntReturn(void) override;

		//Keskeytysten käsittely päällä. Lisätty 4 tasoinen keskytys
		/*bool ie0P2_InterruptServiceIsOn_;      //Prioriteetti 2
		bool tf0P2_InterruptServiceIsOn_;
		bool ie1P2_InterruptServiceIsOn_;
		bool tf1P2_InterruptServiceIsOn_;
		bool serialReceiveP2_InterruptServiceIsOn_;
		bool serialTransmitP2_InterruptServiceIsOn_;
		bool tf2P2_InterruptServiceIsOn_;
		bool ie0P3_InterruptServiceIsOn_;      //Prioriteetti 3
		bool tf0P3_InterruptServiceIsOn_;
		bool ie1P3_InterruptServiceIsOn_;
		bool tf1P3_InterruptServiceIsOn_;
		bool serialReceiveP3_InterruptServiceIsOn_;
		bool serialTransmitP3_InterruptServiceIsOn_;
		bool tf2P3_InterruptServiceIsOn_;
		//Tämän proskun keskeytys
		bool PCAP0_InterruptServiceIsOn_;
		bool PCAP1_InterruptServiceIsOn_;
		bool PCAP2_InterruptServiceIsOn_;
		bool PCAP3_InterruptServiceIsOn_;
	*/
	private:
		T89C51RD2x();

		void createSFRBitNameTable(void); //SFR bittinimien asetus/enablointi
		void createSFRNameTable(void); //SFR rekkarien nimien asetus/enablointi


		//Oheislaitteiden handlays:


};

#endif
