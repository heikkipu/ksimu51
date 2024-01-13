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
#ifndef EXTERNALCYCLE_H
#define EXTERNALCYCLE_H


//Mallintaa ulkoisen muistin lukua ja kirjoitusjaksoja.
#include <vector>
#include <qdatastream.h>
#include "I8031_NameS.h"
#include "I8031.h"
#include "SerialPort.h"

class I8031;
class SerialPort;

class ExternalCycleVector
{
	public:

		ExternalCycleVector(void);
		~ExternalCycleVector(void);

		//Talletus
		inline unsigned int size(void);
		inline void clear(void);   //Pitää nollata joka konejakson alussa tällä.
		//Talleteaan seuravaan cycleen
		inline void writePinValue(IOPinList::PinIndex const pinIndex, bool const value);
		inline void writePinInputSample(IOPinList::PinIndex const pinIndex);
		inline void writePortValue(unsigned int const portAddress, unsigned char const value, unsigned char const bitMask);
		inline void writePortToSample(unsigned int const portNumber, unsigned int const portAddress);
		inline void addWrittenValuesToVector(unsigned int const  numCycles); //Tämä lisää sisäiseen listaan edellä talletetut tilat

		inline void addPinValueInVec(unsigned int const cycleVecNum, IOPinList::PinIndex const pinIndex, bool const value);
		inline void addPinInputSampleInVec(unsigned int const cycleVecNum, IOPinList::PinIndex const pinIndex);
		inline void addPortValueInVec(unsigned int const cycleVecNum, unsigned int const portAddress, unsigned char const value, unsigned char const bitMask);
		inline void addPortToSampleInVec(unsigned int const cycleVecNum, unsigned int const portNumber, unsigned int const portAddress);

//		inline unsigned int readOneStateCycles(unsigned int const cycleVecNum); //Luetaan yhden tilan cyclemäärä.
		inline unsigned int readIOPinOutVecCount(unsigned int const cycleVecNum);
		inline IOPinList::PinIndex readIOPinIndexAndValue(unsigned int const cycleVecNum, unsigned int const pinVecNum, bool &value);
		inline unsigned int readIOPinSampleVecCount(unsigned int const cycleVecNum);
		inline IOPinList::PinIndex readIOPinIndexToSample(unsigned int const cycleVecNum, unsigned int const pinVecNum);
		inline unsigned int readPortOutVecCount(unsigned int const cycleVecNum);
		inline unsigned char readPortValue(unsigned int const cycleVecNum, unsigned int const portVecNum, unsigned char &portAddress, unsigned char &portMask);
		inline unsigned int readPortSampleVecCount(unsigned int const cycleVecNum);
		inline unsigned char readPortAddressToSample(unsigned int const cycleVecNum, unsigned int const portVecNum, unsigned int &portNumber);
		
		void saveToFileStream(QDataStream& saveFileStream);
		void readFromFileStream(QDataStream& readFileStream);

	protected:
		
	private:
		struct ioPinOut_s
		{
			IOPinList::PinIndex index;
			bool value;
		}; 
		struct portOut_s
		{
			unsigned int address;
			unsigned char value;
			unsigned char mask;
		};
		struct portSample_s
		{
			unsigned int number;
			unsigned int address;
		};
		struct ExternalCycleVector_s
		{
			vector<ioPinOut_s> ioPinOutVec;
			vector<IOPinList::PinIndex> ioPinSampleVec;
			vector<portOut_s> portOutVec;
			vector<portSample_s> portSampleVec;
		//	unsigned int numClkCyclesThatState;
		};
		ExternalCycleVector_s workCycle_; //Työcycle josta talletetaan cycleVec_ vektoriin.
		vector<ExternalCycleVector_s> cycleVec_;

		inline void clearWorkCycle(void);

};

class ExternalCycle
{
	public:
		ExternalCycle();
		ExternalCycle(IOPinList::PinIndex const aleIndex, IOPinList::PinIndex const psenIndex, IOPinList::PinIndex const resetIndex, IOPinList::PinIndex const eaIndex, unsigned char p0Address, unsigned char const p1Address, unsigned char const p2Address, unsigned char const p3Address);
		~ExternalCycle();

		//ExternalCycleVector *pToCycles(void){return &cycles_;};
		unsigned int readNumCycles_FromVec(void);
		void clear_Vecs(void);   //Pitää nollata joka konejakson alussa tällä.
//		unsigned int readOneStateCycles_FromVec(unsigned int const cycleVecNum); //Luetaan yhden tilan cyclemäärä.
		
		void addPinValue_ToReadyVec(unsigned int const cycleVecNum, IOPinList::PinIndex const pinIndex, bool const value);
		void addPinInputSample_ToReadyVec(unsigned int const cycleVecNum, IOPinList::PinIndex const pinIndex);
		void addPortValue_ToReadyVec(unsigned int const cycleVecNum, unsigned int const portAddress, unsigned char const value, unsigned char const bitMask);
		void addPortToSample_ToReadyVec(unsigned int const cycleVecNum, unsigned int const portNumber, unsigned int const portAddress);
		
		unsigned int readIOPinOutVecCount_FromVec(unsigned int const cycleVecNum);
		IOPinList::PinIndex readIOPinIndexAndValue_FromVec(unsigned int const cycleVecNum, unsigned int const pinVecNum, bool &value);
		unsigned int readIOPinSampleVecCount_FromVec(unsigned int const cycleVecNum);
		IOPinList::PinIndex readIOPinIndexToSample_FromVec(unsigned int const cycleVecNum, unsigned int const pinVecNum);
		unsigned int readPortOutVecCount_FromVec(unsigned int const cycleVecNum);
		unsigned char readPortValue_FromVec(unsigned int const cycleVecNum, unsigned int const portVecNum, unsigned char &portAddress, unsigned char &portMask);
		unsigned int readPortSampleVecCount_FromVec(unsigned int const cycleVecNum);
		unsigned char readPortAddressToSample_FromVec(unsigned int const cycleVecNum, unsigned int const portVecNum, unsigned int &portNumber);
		
		
		virtual I8031_NameS::alePulses_Type appendToExternalCycle_ReadFromExternalCodeMemoryCycle(unsigned char const addressLow, unsigned char const addressHigh, unsigned char const code, bool const zeroPSEN);
		virtual I8031_NameS::alePulses_Type appendToExternalCycle_ReadFromDataMemoryCycle(unsigned char const addressLow, unsigned char const addressHigh, unsigned char const data, bool const address8);
		virtual I8031_NameS::alePulses_Type appendToExternalCycle_WriteToDataMemoryCycle(unsigned char const addressLow, unsigned char const addressHigh, unsigned char const data, bool const address8);
		virtual void correctCycle(I8031 *pToProcessor, I8031_NameS::machineCycles_Type const& oneStepMachineCycles, I8031_NameS::alePulses_Type const &oneStepAlePulses); //Korjataan external cyclen loppua. Määriteltävä kutsuvissa.
		virtual void addSerialPortMode0Transmit(SerialPort *pToUart, I8031_NameS::machineCycles_Type const &oneStepMachineCycles); //Sarjaportin mode0 pulssitus.
		
		bool isHalfCLKPulse(void){return cycleIsHalfCLKPulse_;};
		bool clkPulseStartLevel(void){return cycleStartCLKPulseLevel_;};

	protected:
		bool cycleIsHalfCLKPulse_; //Jos puolikas cycle. Must be set at constructor.
		bool cycleStartCLKPulseLevel_; //CLK pulssin taso cyclen aloituksessa. Must be set at constructor.
	
		ExternalCycleVector cycles_;
	
		//Tuodaan konstruktorissa tiedot
		IOPinList::PinIndex aleIndex_;
		IOPinList::PinIndex psenIndex_;
		IOPinList::PinIndex resetIndex_;
		IOPinList::PinIndex eaIndex_;

		unsigned char p0Address_;
		unsigned char p1Address_;
		unsigned char p2Address_;
		unsigned char p3Address_;
	
		bool lastCyclePSENEndState_;   //Edellisn Cyclen PSEN tila. Tarvitaan jotta PSEN deaktiivinen toimii
		bool lastCycleWasWriteDataMemory_; //PCL out seuraavan ale pullsin alussa.

};

#endif
