/**************************************************************************
*   Copyright (C) 2008 - ... Heikki Pulkkinen                             *
*   heikki@CompaQ7113EA                                                   *
*
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
#include "ExternalCycle.h"

ExternalCycleVector::ExternalCycleVector(void)
{
}

ExternalCycleVector::~ExternalCycleVector(void)
{
}

unsigned int ExternalCycleVector::size(void)
{
	return cycleVec_.size();
}

void ExternalCycleVector::clear(void)
{
	if(size())
	{
		for(unsigned int i=0; i<size(); i++)
		{
			cycleVec_[i].ioPinOutVec.clear();
			cycleVec_[i].ioPinSampleVec.clear();
			cycleVec_[i].portOutVec.clear();
			cycleVec_[i].portSampleVec.clear();
		}
		cycleVec_.clear();
		clearWorkCycle();
	}
}

void ExternalCycleVector::clearWorkCycle(void)
{
	workCycle_.ioPinOutVec.clear();
	workCycle_.ioPinSampleVec.clear();
	workCycle_.portOutVec.clear();
	workCycle_.portSampleVec.clear();
}

void ExternalCycleVector::writePinValue(IOPinList::PinIndex const pinIndex, bool const value)
{
	ioPinOut_s wPin;
	wPin.index.index_=pinIndex.index_;
	wPin.value=value;
	workCycle_.ioPinOutVec.push_back(wPin);
}

void ExternalCycleVector::writePinInputSample(IOPinList::PinIndex const pinIndex)
{
	workCycle_.ioPinSampleVec.push_back(pinIndex);
}

void ExternalCycleVector::writePortValue(unsigned int const portAddress, unsigned char const value, unsigned char const bitMask)
{
	portOut_s wPort;
	wPort.address=portAddress;
	wPort.value=value;
	wPort.mask=bitMask;
	workCycle_.portOutVec.push_back(wPort);
}

void ExternalCycleVector::writePortToSample(unsigned int const portNumber, unsigned int const portAddress)
{
	portSample_s wPort;
	wPort.number=portNumber;
	wPort.address=portAddress;
	workCycle_.portSampleVec.push_back(wPort);
}

//Tämä lisää sisäiseen listaan edellä talletetut tilat
void ExternalCycleVector::addWrittenValuesToVector(unsigned int const  numCycles)
{
	//workCycle_.numClkCyclesThatState=numCycles;
	cycleVec_.push_back(workCycle_);
	clearWorkCycle();
	if(numCycles>1)
		for(unsigned int n=1; n<numCycles; n++)
			cycleVec_.push_back(workCycle_);
} 

void ExternalCycleVector::addPinValueInVec(unsigned int const cycleVecNum, IOPinList::PinIndex const pinIndex, bool const value)
{
	if(cycleVecNum<cycleVec_.size())
	{
		ioPinOut_s is;
		is.index.index_=pinIndex.index_;
		is.value=value;
		cycleVec_[cycleVecNum].ioPinOutVec.push_back(is);
	}
}

void ExternalCycleVector::addPinInputSampleInVec(unsigned int const cycleVecNum, IOPinList::PinIndex const pinIndex)
{
	if(cycleVecNum<cycleVec_.size())
	{
		cycleVec_[cycleVecNum].ioPinSampleVec.push_back(pinIndex);
	}
}

void ExternalCycleVector::addPortValueInVec(unsigned int const cycleVecNum, unsigned int const portAddress, unsigned char const value, unsigned char const bitMask)
{
	if(cycleVecNum<cycleVec_.size())
	{
		portOut_s is;
		is.address=portAddress;
		is.value=value;
		is.mask=bitMask;
		cycleVec_[cycleVecNum].portOutVec.push_back(is);
	}
}

void ExternalCycleVector::addPortToSampleInVec(unsigned int const cycleVecNum, unsigned int const portNumber, unsigned int const portAddress)
{
	if(cycleVecNum<cycleVec_.size())
	{
		portSample_s is;
		is.number=portNumber;
		is.address=portAddress;
		cycleVec_[cycleVecNum].portSampleVec.push_back(is);
	}
}

/*unsigned int ExternalCycleVector::readOneStateCycles(unsigned int const cycleVecNum)
{
	if(cycleVecNum<cycleVec_.size())
		return cycleVec_[cycleVecNum].numClkCyclesThatState;
	else
		return 0;
}
*/
unsigned int ExternalCycleVector::readIOPinOutVecCount(unsigned int const cycleVecNum)
{
	if(cycleVecNum<cycleVec_.size())
		return cycleVec_[cycleVecNum].ioPinOutVec.size();
	return 0;
}

IOPinList::PinIndex ExternalCycleVector::readIOPinIndexAndValue(unsigned int const cycleVecNum, unsigned int const pinVecNum, bool &value)
{
	if(cycleVecNum<cycleVec_.size())
		if(pinVecNum<cycleVec_[cycleVecNum].ioPinOutVec.size())
		{
			value=cycleVec_[cycleVecNum].ioPinOutVec[pinVecNum].value;
			return cycleVec_[cycleVecNum].ioPinOutVec[pinVecNum].index;
		}
	IOPinList rv;
	return rv.NO_INDEX();
}

unsigned int ExternalCycleVector::readIOPinSampleVecCount(unsigned int const cycleVecNum)
{
	if(cycleVecNum<cycleVec_.size())
		return cycleVec_[cycleVecNum].ioPinSampleVec.size();
	return 0;
}

IOPinList::PinIndex ExternalCycleVector::readIOPinIndexToSample(unsigned int const cycleVecNum, unsigned int const pinVecNum)
{
	if(cycleVecNum<cycleVec_.size())
		if(pinVecNum<cycleVec_[cycleVecNum].ioPinSampleVec.size())
			return cycleVec_[cycleVecNum].ioPinSampleVec[pinVecNum];
	IOPinList rv;
	return rv.NO_INDEX();
}

unsigned int ExternalCycleVector::readPortOutVecCount(unsigned int const cycleVecNum)
{
	if(cycleVecNum<cycleVec_.size())
		return cycleVec_[cycleVecNum].portOutVec.size();
	return 0;
}

unsigned char ExternalCycleVector::readPortValue(unsigned int const cycleVecNum, unsigned int const portVecNum, unsigned char &portAddress, unsigned char &portMask)
{
	if(cycleVecNum<cycleVec_.size())
		if(portVecNum<cycleVec_[cycleVecNum].portOutVec.size())
		{
			portAddress=cycleVec_[cycleVecNum].portOutVec[portVecNum].address;
			portMask=cycleVec_[cycleVecNum].portOutVec[portVecNum].mask;
			return cycleVec_[cycleVecNum].portOutVec[portVecNum].value;
		}
	return 0;
}

unsigned int ExternalCycleVector::readPortSampleVecCount(unsigned int const cycleVecNum)
{
	if(cycleVecNum<cycleVec_.size())
		return cycleVec_[cycleVecNum].portSampleVec.size();
	return 0;
}

unsigned char ExternalCycleVector::readPortAddressToSample(unsigned int const cycleVecNum, unsigned int const portVecNum, unsigned int &portNumber)
{
	if(cycleVecNum<cycleVec_.size())
		if(portVecNum<cycleVec_[cycleVecNum].portSampleVec.size())
		{
			portNumber=cycleVec_[cycleVecNum].portSampleVec[portVecNum].number;
			return cycleVec_[cycleVecNum].portSampleVec[portVecNum].address;
		}
	return 0;
}

void ExternalCycleVector::saveToFileStream(QDataStream& saveFileStream)
{
/*	saveFileStream<<(Q_UINT8)numCycles_;

	for(int n=0; n<MAX_CYCLES; n++)
	{
		saveFileStream<<(Q_UINT8)cycleTable_[n].address8bit;
		saveFileStream<<(Q_UINT8)cycleTable_[n].ale;
		saveFileStream<<(Q_UINT8)cycleTable_[n].psen;
		saveFileStream<<(Q_UINT8)cycleTable_[n].sampleP0In;
		saveFileStream<<(Q_UINT8)cycleTable_[n].samplePxIn;
		saveFileStream<<(Q_UINT8)cycleTable_[n].port3_RD;
		saveFileStream<<(Q_UINT8)cycleTable_[n].port3_WR;
		saveFileStream<<(Q_INT16)cycleTable_[n].port0;
		saveFileStream<<(Q_INT16)cycleTable_[n].port2;
	}
*/
}

void ExternalCycleVector::readFromFileStream(QDataStream& readFileStream)
{
/*
	Q_UINT8 uint8;
	Q_INT16 int16;

	readFileStream>>uint8; numCycles_=uint8;

	for(int n=0; n<MAX_CYCLES; n++)
	{
		readFileStream>>uint8; cycleTable_[n].address8bit=uint8;
		readFileStream>>uint8; cycleTable_[n].ale=uint8;
		readFileStream>>uint8; cycleTable_[n].psen=uint8;
		readFileStream>>uint8; cycleTable_[n].sampleP0In=uint8;
		readFileStream>>uint8; cycleTable_[n].samplePxIn=uint8;
		readFileStream>>uint8; cycleTable_[n].port3_RD=uint8;
		readFileStream>>uint8; cycleTable_[n].port3_WR=uint8;
		readFileStream>>int16; cycleTable_[n].port0=int16;
		readFileStream>>int16; cycleTable_[n].port2=int16;
	}
*/
}


//-------------------------------------- ExternalCycle::
ExternalCycle::ExternalCycle(){}

ExternalCycle::ExternalCycle(IOPinList::PinIndex const aleIndex, IOPinList::PinIndex const psenIndex, IOPinList::PinIndex const resetIndex, IOPinList::PinIndex const eaIndex, unsigned char p0Address, unsigned char const p1Address, unsigned char const p2Address, unsigned char const p3Address)
{
	aleIndex_.index_=aleIndex.index_;
	psenIndex_.index_=psenIndex.index_;
	resetIndex_.index_=resetIndex.index_;
	eaIndex_.index_=eaIndex.index_;
	p0Address_=p0Address;
	p1Address_=p1Address;
	p2Address_=p2Address;
	p3Address_=p3Address;
	
	cycleIsHalfCLKPulse_=false; //Koko kelljakson Cycle.
	cycleStartCLKPulseLevel_=0; //Alakaa kellon puoljaksolla 0.
	
	lastCyclePSENEndState_=false;
	lastCycleWasWriteDataMemory_=false;

}

ExternalCycle::~ExternalCycle()
{
}

unsigned int ExternalCycle::readNumCycles_FromVec(void)
{
	return cycles_.size();
}

void ExternalCycle::clear_Vecs(void)
{
	cycles_.clear();
}

void ExternalCycle::addPinValue_ToReadyVec(unsigned int const cycleVecNum, IOPinList::PinIndex const pinIndex, bool const value)
{
	cycles_.addPinValueInVec(cycleVecNum, pinIndex, value);
}

void ExternalCycle::addPinInputSample_ToReadyVec(unsigned int const cycleVecNum, IOPinList::PinIndex const pinIndex)
{
	cycles_.addPinInputSampleInVec(cycleVecNum, pinIndex);
}

void ExternalCycle::addPortValue_ToReadyVec(unsigned int const cycleVecNum, unsigned int const portAddress, unsigned char const value, unsigned char const bitMask)
{
	cycles_.addPortValueInVec(cycleVecNum, portAddress, value, bitMask);
}

void ExternalCycle::addPortToSample_ToReadyVec(unsigned int const cycleVecNum, unsigned int const portNumber, unsigned int const portAddress)
{
	cycles_.addPortToSampleInVec(cycleVecNum, portNumber, portAddress);
}

/*unsigned int ExternalCycle::readOneStateCycles_FromVec(unsigned int const cycleVecNum)
{
	return cycles_.readOneStateCycles(cycleVecNum);
}
*/
unsigned int ExternalCycle::readIOPinOutVecCount_FromVec(unsigned int const cycleVecNum)
{
	return cycles_.readIOPinOutVecCount(cycleVecNum);
}

IOPinList::PinIndex ExternalCycle::readIOPinIndexAndValue_FromVec(unsigned int const cycleVecNum, unsigned int const pinVecNum, bool &value)
{
	return cycles_.readIOPinIndexAndValue(cycleVecNum, pinVecNum, value);
}

unsigned int ExternalCycle::readIOPinSampleVecCount_FromVec(unsigned int const cycleVecNum)
{
	return cycles_.readIOPinSampleVecCount(cycleVecNum);
}

IOPinList::PinIndex ExternalCycle::readIOPinIndexToSample_FromVec(unsigned int const cycleVecNum, unsigned int const pinVecNum)
{
	return cycles_.readIOPinIndexToSample(cycleVecNum, pinVecNum);
}

unsigned int ExternalCycle::readPortOutVecCount_FromVec(unsigned int const cycleVecNum)
{
	return cycles_.readPortOutVecCount(cycleVecNum);
}

unsigned char ExternalCycle::readPortValue_FromVec(unsigned int const cycleVecNum, unsigned int const portVecNum, unsigned char &portAddress, unsigned char &portMask)
{
	return cycles_.readPortValue(cycleVecNum, portVecNum, portAddress, portMask);
}

unsigned int ExternalCycle::readPortSampleVecCount_FromVec(unsigned int const cycleVecNum)
{
	return cycles_.readPortSampleVecCount(cycleVecNum);
}

unsigned char ExternalCycle::readPortAddressToSample_FromVec(unsigned int const cycleVecNum, unsigned int const portVecNum, unsigned int &portNumber)
{
	return cycles_.readPortAddressToSample(cycleVecNum, portVecNum, portNumber);
}


//Tehdään code memorystä lukutoiminto. zero Peseniä voi käyttää sisäisen codemuitin ja cyclen täydennyksen kanssa.
//Palauttaa ALE cyclien määrän
I8031_NameS::alePulses_Type ExternalCycle::appendToExternalCycle_ReadFromExternalCodeMemoryCycle(unsigned char const addressLow, unsigned char const addressHigh, unsigned char const code, bool const zeroPSEN)
{
	//Puolen konejakson cycle.
	
	//1.CLK STATE S1||S4 P1
	//Pitää ekaksi kattoa edellisen cycle PSEN:in tila
	cycles_.writePinValue(aleIndex_, 0);
	cycles_.writePinValue(psenIndex_, lastCyclePSENEndState_);
	cycles_.addWrittenValuesToVector(1);
	
	//2.CLK STATE S1||S4 P2
	cycles_.writePinValue(aleIndex_, 1);
	cycles_.writePinValue(psenIndex_, 1);
	if(lastCycleWasWriteDataMemory_)
	{
		cycles_.writePortValue(p0Address_, addressLow, 0xff);
		cycles_.addWrittenValuesToVector(1);
		lastCycleWasWriteDataMemory_=false;
	}
	cycles_.addWrittenValuesToVector(1);

	//3.CLK STATE S2||S5 P1
	cycles_.writePortValue(p0Address_, addressLow, 0xff);
	cycles_.writePortValue(p2Address_, addressHigh, 0xff);
	cycles_.writePortToSample(0, p0Address_);
	cycles_.addWrittenValuesToVector(1);
	
	//4.CLK STATE S2||S5 P2
	cycles_.writePinValue(aleIndex_, 0);
	cycles_.writePortToSample(1, p1Address_);
	cycles_.writePortToSample(2, p2Address_);
	cycles_.writePortToSample(3, p3Address_);
	cycles_.addWrittenValuesToVector(1);
	
	//5.CLK STATE S3||S6 P1
	cycles_.writePinValue(aleIndex_, 0);
	cycles_.writePinValue(psenIndex_, zeroPSEN);
	cycles_.writePortValue(p2Address_, addressHigh, 0xff);
	cycles_.addWrittenValuesToVector(1);
	//6.CLK STATE S3||S6 P2
	cycles_.writePinValue(aleIndex_, 0);
	cycles_.writePinValue(psenIndex_, zeroPSEN);
	cycles_.writePortValue(p0Address_, code, 0xff);
	cycles_.writePortValue(p2Address_, addressHigh, 0xff);
	cycles_.addWrittenValuesToVector(1);

	lastCyclePSENEndState_=zeroPSEN;
	lastCycleWasWriteDataMemory_=false;

	return 1;
}

//Tekee ulkoisen data alueen lukutoiminnon.
//Palauttaa ALE cyclien määrän. Toinen ale pulssi olematon.
I8031_NameS::alePulses_Type ExternalCycle::appendToExternalCycle_ReadFromDataMemoryCycle(unsigned char const addressLow, unsigned char const addressHigh, unsigned char const data, bool const address8)
{
	
	//1.CLK STATE S4 P1
	cycles_.writePinValue(aleIndex_, 0);
	cycles_.writePinValue(psenIndex_, lastCyclePSENEndState_);
	cycles_.writePortValue(p3Address_, I8031_NameS::P3_WR|I8031_NameS::P3_RD, I8031_NameS::P3_WR|I8031_NameS::P3_RD);//WR ja RD 1
	cycles_.addWrittenValuesToVector(1);

	//2.CLK STATE S4 P2
	cycles_.writePinValue(aleIndex_, 1);
	cycles_.writePinValue(psenIndex_, 1);
	cycles_.addWrittenValuesToVector(1);

	//3.CLK STATE S5 P1
	cycles_.writePortValue(p0Address_, addressLow, 0xff);
	if(!address8)
		cycles_.writePortValue(p2Address_, addressHigh, 0xff);
	cycles_.addWrittenValuesToVector(1);

	//4.CLK STATE S5 P1 and S6 P1
	cycles_.writePinValue(aleIndex_, 0);
	cycles_.addWrittenValuesToVector(2);

	//6.CLK STATE S6 P2
	cycles_.writePortValue(p0Address_, 0, 0xff);
	cycles_.addWrittenValuesToVector(1);

	//(next)(7.CLK STATE S1 P1, 8. CLK STATE S1 P2, 9. CLK STATE S2 P1, 10. CLK STATE S2 P2 ,11. CLK STATE S3 P1
	cycles_.writePortValue(p0Address_, data, 0xff);
	cycles_.writePortValue(p3Address_, 0, I8031_NameS::P3_RD);//RD 0
	cycles_.addWrittenValuesToVector(5);

	//12.CLK STATE S3 P2 )(next)
	cycles_.writePortValue(p3Address_, I8031_NameS::P3_RD, I8031_NameS::P3_RD);//RD 1
	cycles_.addWrittenValuesToVector(1);

	lastCyclePSENEndState_=1;
	lastCycleWasWriteDataMemory_=false;
	return 2;

}

//Tehdään ulkoiselle data alueelle kirjoitustoiminto
//Palauttaa ALE cyclien määrän. Toinen ale olematon.
I8031_NameS::alePulses_Type ExternalCycle::appendToExternalCycle_WriteToDataMemoryCycle(unsigned char const addressLow, unsigned char const addressHigh, unsigned char const data, bool const address8)
{
	//1.CLK STATE S4 P1
	cycles_.writePinValue(aleIndex_, 0);
	cycles_.writePinValue(psenIndex_, lastCyclePSENEndState_);
	cycles_.writePortValue(p3Address_, I8031_NameS::P3_WR|I8031_NameS::P3_RD, I8031_NameS::P3_WR|I8031_NameS::P3_RD);//WR ja RD 1
	cycles_.addWrittenValuesToVector(1);
	
	//2.CLK STATE S4 P2
	cycles_.writePinValue(aleIndex_, 1);
	cycles_.writePinValue(psenIndex_, 1);
	cycles_.addWrittenValuesToVector(1);
	
	//3.CLK STATE S5 P1
	cycles_.writePortValue(p0Address_, addressLow, 0xff);
	if(!address8)
		cycles_.writePortValue(p2Address_, addressHigh, 0xff);
	cycles_.addWrittenValuesToVector(1);
	
	//4.CLK STATE S5 P2, 5.CLK STATE S6 P1
	cycles_.writePinValue(aleIndex_, 0);
	cycles_.addWrittenValuesToVector(2);
	
	//6.CLK STATE S6 P2, (next)(7.CLK STATE S1 P1, 8.CLK STATE S1 P2, 9.CLK STATE S2 P1, 10.CLK STATE S2 P2, 11.CLK STATE S3 P1
	cycles_.writePortValue(p0Address_, data, 0xff);
	cycles_.writePortValue(p3Address_, 0, I8031_NameS::P3_WR);//WR 0
	cycles_.addWrittenValuesToVector(6);
	
	//12.CLK STATE S3 P2)(next)
	cycles_.writePortValue(p3Address_, I8031_NameS::P3_WR, I8031_NameS::P3_WR);//WR 1
	cycles_.addWrittenValuesToVector(1);

	lastCyclePSENEndState_=1;
	lastCycleWasWriteDataMemory_=true;
	
	return 2;
}

void ExternalCycle::correctCycle(I8031 *pToProcessor, I8031_NameS::machineCycles_Type const& oneStepMachineCycles, I8031_NameS::alePulses_Type const &oneStepAlePulses)
{
	using namespace I8031_NameS;
	while(oneStepMachineCycles*2>oneStepAlePulses)//*2 koska 1:ssä cyslessä 2 ale pulssia.
	{
		programCounter_Type pc=pToProcessor->readProgramCounterValue();
		pToProcessor->readInstructionDoCycle(DO_CYCLE_ALSO);
		pToProcessor->setProgramCounterValue(pc);
	}
}

void ExternalCycle::addSerialPortMode0Transmit(SerialPort *pToUart, I8031_NameS::machineCycles_Type const &oneStepMachineCycles) //Sarjaportin mode0 pulssitus.
{
	using namespace I8031_NameS;
	
	if(!pToUart->readMode())
	{
		if(pToUart->isTaransmitOn(0))
		{
			for(int n=0; n<oneStepMachineCycles; n++)
			{
				int bitToTransmit;
				int tBuffer=pToUart->readTransmitSbufMode0(bitToTransmit);
				if(bitToTransmit<8)
				{
					int multipler=12*n;
					int value=(tBuffer>>bitToTransmit)|P3_TXD; //Lähtevä data bittiin0 ja kello ylös
					addPortValue_ToReadyVec(multipler+0, P3, value, P3_RXD|P3_TXD); //S0
					addPortValue_ToReadyVec(multipler+3, P3, 0, P3_TXD); //S3 Kello alas
					addPortValue_ToReadyVec(multipler+9, P3, 0xff, P3_TXD); //S6 Kello ylös
				}
				else
					pToUart->stopTransmitMode0();
			}
		}
	}
}


