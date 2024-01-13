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
#ifndef MAX765X_EXTERNALCYCLE_H
#define MAX765X_EXTERNALCYCLE_H

#include "../I8051_ExternalCycle.h"
#include "../InternalRAM.h"
#include "MAX765x_NameS.h"

class MAX765x_ExternalCycle : public I8051_ExternalCycle
{
	public:
		MAX765x_ExternalCycle(IOPinList::PinIndex const aleIndex, IOPinList::PinIndex const psenIndex, IOPinList::PinIndex const resetIndex, IOPinList::PinIndex const eaIndex, IOPinList::PinIndex const int0Index, IOPinList::PinIndex const int1Index, IOPinList::PinIndex const pwmAIndex, IOPinList::PinIndex const pwmBIndex, unsigned char p0Address, unsigned char const p1Address, unsigned char const p2Address, unsigned char const p3Address, InternalRAM *pToMemoryStretchRegister);
		~MAX765x_ExternalCycle();
		
		//Uudelleenmääriteellyt external cycle hommat
		I8031_NameS::alePulses_Type appendToExternalCycle_nonPSENPulseCycle(I8031_NameS::programCounter_Type programCounter) override;
		I8031_NameS::alePulses_Type appendToExternalCycle_ReadFromExternalCodeMemoryCycle(unsigned char const addressLow, unsigned char const addressHigh, unsigned char const code, bool const zeroPSEN) override;

		I8031_NameS::alePulses_Type appendToExternalCycle_ReadFromDataMemoryCycle(unsigned char const addressLow, unsigned char const addressHigh, unsigned char const data, bool const address8) override;
		I8031_NameS::alePulses_Type appendToExternalCycle_WriteToDataMemoryCycle(unsigned char const addressLow, unsigned char const addressHigh, unsigned char const data, bool const address8) override;
		void correctCycle(I8031 *pToProcessor, I8031_NameS::machineCycles_Type const &oneStepMachineCycles, I8031_NameS::alePulses_Type const &oneStepAlePulses) override; //Korjataan external cyclen loppua. Määriteltävä kutsuvissa.
		void addSerialPortMode0Transmit(SerialPort *pToUart, I8031_NameS::machineCycles_Type const &oneStepMachineCycles) override; //Sarjaportin mode0 pulssitus. UM
		void addSerialPort1Mode0Transmit(SerialPort *pToUart, I8031_NameS::machineCycles_Type const &oneStepMachineCycles); //Sarjaportin1 mode0 pulssitus. !UM
		
	private:
		IOPinList::PinIndex int0Index_;
		IOPinList::PinIndex int1Index_;
		IOPinList::PinIndex pwmAIndex_;
		IOPinList::PinIndex pwmBIndex_;

		InternalRAM *pToMemoryStretchRegister_;
	
		int serialPort0Mode0MachineCyclesCount;
		int serialPort1Mode0MachineCyclesCount;
		bool serialPort0Mode0Transmitting;
		bool serialPort1Mode0Transmitting;
		void doSerialPortMode0Transmit(SerialPort *pToUart, I8031_NameS::machineCycles_Type const &oneStepMachineCycles, int &serialPortMode0MachineCyclesCount, bool &serialPortMode0Transmitting, unsigned char sm2Bit);
};

#endif
