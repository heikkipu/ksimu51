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
#include "MAX765x_ExternalCycle.h"

MAX765x_ExternalCycle::MAX765x_ExternalCycle(IOPinList::PinIndex const aleIndex, IOPinList::PinIndex const psenIndex, IOPinList::PinIndex const resetIndex, IOPinList::PinIndex const eaIndex, IOPinList::PinIndex const int0Index, IOPinList::PinIndex const int1Index, IOPinList::PinIndex const pwmAIndex, IOPinList::PinIndex const pwmBIndex, unsigned char p0Address, unsigned char const p1Address, unsigned char const p2Address, unsigned char const p3Address, InternalRAM *pToMemoryStretchRegister) : I8051_ExternalCycle(aleIndex, psenIndex, resetIndex, eaIndex, p0Address, p1Address, p2Address, p3Address)
{
	int0Index_.index_=int0Index.index_;
	int1Index_.index_=int1Index.index_;
	pwmAIndex_.index_=pwmAIndex.index_;
	pwmBIndex_.index_=pwmBIndex.index_;

	pToMemoryStretchRegister_=pToMemoryStretchRegister;

	cycleIsHalfCLKPulse_=true; //External cyclet puolen kellojakson pulsseiksi.
	cycleStartCLKPulseLevel_=1; //Cyclen kello alkaa levelillä 1
	serialPort0Mode0MachineCyclesCount=0;
	serialPort1Mode0MachineCyclesCount=0;
	serialPort0Mode0Transmitting=false;
	serialPort1Mode0Transmitting=false;
}

MAX765x_ExternalCycle::~MAX765x_ExternalCycle()
{
}

I8031_NameS::alePulses_Type MAX765x_ExternalCycle::appendToExternalCycle_nonPSENPulseCycle(I8031_NameS::programCounter_Type programCounter)
{
	//1., 2., 3. half CLK
	cycles_.writePinValue(aleIndex_, 1);
	cycles_.writePinValue(psenIndex_, 1);
	cycles_.addWrittenValuesToVector(3);
	
	//4., 5. half CLK
	cycles_.writePinValue(aleIndex_, 0);
	cycles_.addWrittenValuesToVector(2);
	
	//6. half CLK
	cycles_.writePortToSample(0, p0Address_);
	cycles_.writePortToSample(1, p1Address_);
	cycles_.writePortToSample(2, p2Address_);
	cycles_.writePortToSample(3, p3Address_);
	cycles_.addWrittenValuesToVector(3); //+7. and 8. half CLK

	lastCycleWasWriteDataMemory_=false;
	return 1;
}

I8031_NameS::alePulses_Type MAX765x_ExternalCycle::appendToExternalCycle_ReadFromExternalCodeMemoryCycle(unsigned char const addressLow, unsigned char const addressHigh, unsigned char const code, bool const zeroPSEN)
{
	//1. half CLK
	cycles_.writePinValue(aleIndex_, 1);
	cycles_.writePinValue(psenIndex_, 1);
	cycles_.addWrittenValuesToVector(1);

	//2., 3. half CLK
	cycles_.writePortValue(p0Address_, addressLow, 0xff);
	cycles_.writePortValue(p2Address_, addressHigh, 0xff);
	cycles_.addWrittenValuesToVector(2);

	//4. half CLK
	cycles_.writePinValue(aleIndex_, 0);
	cycles_.addWrittenValuesToVector(1);
	
	//5. half CLK
	cycles_.writePinValue(psenIndex_, zeroPSEN);
	cycles_.writePortValue(p0Address_, code, 0xff);
	cycles_.addWrittenValuesToVector(1);
	
	//6. half CLK
	cycles_.writePortToSample(0, p0Address_);
	cycles_.writePortToSample(1, p1Address_);
	cycles_.writePortToSample(2, p2Address_);
	cycles_.writePortToSample(3, p3Address_);
	cycles_.addWrittenValuesToVector(3); //+7. and 8. half CLK
	
	lastCyclePSENEndState_=zeroPSEN;
	lastCycleWasWriteDataMemory_=false;

	return 1;
}

I8031_NameS::alePulses_Type MAX765x_ExternalCycle::appendToExternalCycle_ReadFromDataMemoryCycle(unsigned char const addressLow, unsigned char const addressHigh, unsigned char const data, bool const address8)
{
	//Haetaan tieto stretchistä
	//unsigned char memStretch=internalDataMemory_[CKCON].readValue()&0x07; //Maskataan ylimmät bitit pois.
	unsigned char memStretch=(pToMemoryStretchRegister_->readValue())&0x07; //Maskataan ylimmät bitit pois.
	//1., 2. half CLK
	cycles_.writePinValue(aleIndex_, 1);
	cycles_.writePinValue(psenIndex_, 1);
	cycles_.writePortValue(p3Address_, I8031_NameS::P3_WR|I8031_NameS::P3_RD, I8031_NameS::P3_WR|I8031_NameS::P3_RD);//WR ja RD 1
	cycles_.addWrittenValuesToVector(2);

	//3. half CLK
	cycles_.writePortValue(p0Address_, addressLow, 0xff);
	if(!address8)
		cycles_.writePortValue(p2Address_, addressHigh, 0xff);
	cycles_.addWrittenValuesToVector(1);

	//4. half CLK
	cycles_.writePinValue(aleIndex_, 0);
	cycles_.addWrittenValuesToVector(1);

	//5., 6. and stretch half CLKs. also 7.+stretch half CLK
	cycles_.writePortValue(p0Address_, data, 0xff);
	cycles_.writePortValue(p3Address_, 0, I8031_NameS::P3_RD);//RD 0
	cycles_.addWrittenValuesToVector(memStretch*4+3);

	//8.+stretch half CLK
	cycles_.writePortValue(p3Address_, 0xff, I8031_NameS::P3_RD);//RD 1
	cycles_.addWrittenValuesToVector(1);

	lastCyclePSENEndState_=1;
	return memStretch+1;
}

I8031_NameS::alePulses_Type MAX765x_ExternalCycle::appendToExternalCycle_WriteToDataMemoryCycle(unsigned char const addressLow, unsigned char const addressHigh, unsigned char const data, bool const address8)
{
	//Haetaan tieto stretchistä
	unsigned char memStretch=(pToMemoryStretchRegister_->readValue())&0x07; //Maskataan ylimmät bitit pois.
	
	//1., 2. half CLK
	cycles_.writePinValue(aleIndex_, 1);
	cycles_.writePinValue(psenIndex_, 1);
	cycles_.writePortValue(p3Address_, I8031_NameS::P3_WR|I8031_NameS::P3_RD, I8031_NameS::P3_WR|I8031_NameS::P3_RD);//WR ja RD 1
	cycles_.addWrittenValuesToVector(2);

	//3. half CLK
	cycles_.writePortValue(p0Address_, addressLow, 0xff);
	if(!address8)
		cycles_.writePortValue(p2Address_, addressHigh, 0xff);
	cycles_.addWrittenValuesToVector(1);

	//4. half CLK
	cycles_.writePinValue(aleIndex_, 0);
	cycles_.addWrittenValuesToVector(1);

	//5., 6. and stretch half CLKs. also 7.+stretch half CLK
	cycles_.writePortValue(p0Address_, data, 0xff);
	cycles_.writePortValue(p3Address_, 0, I8031_NameS::P3_WR);//WR 0
	cycles_.addWrittenValuesToVector(memStretch*4+3);

	//8+stretch. half CLK
	cycles_.writePortValue(p3Address_, 0xff, I8031_NameS::P3_WR);//WR 1
	cycles_.addWrittenValuesToVector(1);

	lastCyclePSENEndState_=1;
	lastCycleWasWriteDataMemory_=true;

	return memStretch+1;
}

void MAX765x_ExternalCycle::correctCycle(I8031 *pToProcessor, I8031_NameS::machineCycles_Type const& oneStepMachineCycles, I8031_NameS::alePulses_Type const &oneStepAlePulses) //Korjataan external cyclen loppua. Määriteltävä kutsuvissa.
{
	using namespace MAX765x_NameS;
	while(oneStepAlePulses<oneStepMachineCycles)
	{
		programCounter_Type pc=pToProcessor->readProgramCounterValue();
		pToProcessor->readInstructionDoCycle(DO_CYCLE_ALSO);
		pToProcessor->setProgramCounterValue(pc);
	}
}

void MAX765x_ExternalCycle::addSerialPortMode0Transmit(SerialPort *pToUart, I8031_NameS::machineCycles_Type const &oneStepMachineCycles) //Sarjaportin mode0 pulssitus.
{
	using namespace MAX765x_NameS;
	doSerialPortMode0Transmit(pToUart, oneStepMachineCycles, serialPort0Mode0MachineCyclesCount, serialPort0Mode0Transmitting, SCON_SM2);
}

void MAX765x_ExternalCycle::addSerialPort1Mode0Transmit(SerialPort *pToUart, I8031_NameS::machineCycles_Type const &oneStepMachineCycles) //Sarjaportin1 mode0 pulssitus. !UM
{
	using namespace MAX765x_NameS;
	doSerialPortMode0Transmit(pToUart, oneStepMachineCycles, serialPort1Mode0MachineCyclesCount, serialPort1Mode0Transmitting, SCON1_SM2);
}

void MAX765x_ExternalCycle::doSerialPortMode0Transmit(SerialPort *pToUart, I8031_NameS::machineCycles_Type const &oneStepMachineCycles, int &serialPortMode0MachineCyclesCount, bool &serialPortMode0Transmitting, unsigned char sm2Bit)
{
	using namespace MAX765x_NameS;
	if(!pToUart->readMode())
	{
		if(pToUart->isTaransmitOn(0)||(serialPortMode0Transmitting&&serialPortMode0MachineCyclesCount))
		{
			serialPortMode0Transmitting=true;
			int cycleN=0;
			for(int n=0; n<oneStepMachineCycles; n++)
			{
				int bitToTransmit;
				int tBuffer;
				if(pToUart->readSCON()&sm2Bit) //High Speed
				{
					tBuffer=pToUart->readTransmitSbufMode0(bitToTransmit);
					if(bitToTransmit<8)
					{
						addPortValue_ToReadyVec(cycleN, P3, 0xff, P3_TXD); //Kello ylös
						cycleN+=4;
						int value=(tBuffer>>bitToTransmit)&~P3_TXD; //Lähtevä data bittiin0 ja kellobitti alas
						addPortValue_ToReadyVec(cycleN, P3, value, P3_RXD|P3_TXD); //Data ja kello alas
						cycleN+=4;
					}
					else
					{
						pToUart->stopTransmitMode0();
						addPortValue_ToReadyVec(cycleN, P3, 0xff, P3_TXD); //Kello ylös
						serialPortMode0Transmitting=false;
					}
				}
				else //Low speed
				{
					if(!(serialPortMode0MachineCyclesCount%3))//0,3,6,9...
					{
						tBuffer=pToUart->readTransmitSbufMode0(bitToTransmit);
						if(bitToTransmit<8)
						{
							addPortValue_ToReadyVec(cycleN, P3, 0xff, P3_TXD); //Kello ylös
							cycleN+=5;
							int value=(tBuffer>>bitToTransmit)&~P3_TXD; //Lähtevä data bittiin0 ja kellobitti alas
							addPortValue_ToReadyVec(cycleN, P3, value, P3_RXD|P3_TXD); //Data ja kello alas
							cycleN+=3;
						}
					}
					else
						if(!((serialPortMode0MachineCyclesCount+1)%3))//2,5,8,11...
						{
							cycleN+=6;
							addPortValue_ToReadyVec(cycleN, P3, 0xff, P3_TXD); //Kello ylös
							cycleN+=2;
						}
						else //1,4,7,10...
							cycleN+=8;
					serialPortMode0MachineCyclesCount++;
					if(serialPortMode0MachineCyclesCount>24)
					{
						pToUart->stopTransmitMode0();
						addPortValue_ToReadyVec(cycleN, P3, 0xff, P3_TXD); //Kello ylös
						serialPortMode0Transmitting=false;
					}
				}
			}
		}
		else
			if(serialPortMode0MachineCyclesCount)
				serialPortMode0MachineCyclesCount=0;
	}
}
