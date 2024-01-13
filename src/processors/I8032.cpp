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
#include "I8032.h"
#include "../ksimu51.h"



//Muodostin
I8032::I8032():I8031()
{
	using namespace I8032_NameS;
	processorName_="8032";
	formatInternalIndirectDataMemory();
	createSFRBitNameTable();
	createSFRNameTable();
	clearInternalIndirectDataMemory();
	//Tämän processorin erityisominaisuudet
	delete timersCounters_;
	timersCounters_=new TimersCounters2(&internalDataMemory_[TL0], &internalDataMemory_[TH0], &internalDataMemory_[TL1], &internalDataMemory_[TH1], &internalDataMemory_[TMOD], &internalDataMemory_[TCON], &internalDataMemory_[IE], &internalDataMemory_[P3], &internalDataMemory_[TL2], &internalDataMemory_[TH2], &internalDataMemory_[RCAP2L], &internalDataMemory_[RCAP2H], &internalDataMemory_[T2CON], &internalDataMemory_[P1]);
}

//Hajotin
I8032::~I8032()
{
}

void I8032::reset_VirtualPart()
{
	using namespace I8032_NameS;
	tf2P0_InterruptServiceIsOn_=false;
	tf2P1_InterruptServiceIsOn_=false;
	internalDataMemory_[T2CON]=0x0;
	internalDataMemory_[RCAP2L]=0x0;
	internalDataMemory_[RCAP2H]=0x0;
	internalDataMemory_[TL2]=0x0;
	internalDataMemory_[TH2]=0x0;
}

//Tehdään syväresetti, clearaa kaikki alueet
void I8032::powerOnReset_VirtualPart(void)
{
	clearInternalIndirectDataMemory();
}

bool I8032::isInternalIndirectDataMemoryChanged(unsigned int const  address)
{
	using namespace I8032_NameS;
	return internalIndirectDataMemory_[address-MIN_INTERNAL_INDIRECT_RAM_ADDRESS].isChanged();
}

bool I8032::isInternalIndirectDataMemoryChangedAfterClear(unsigned int const  address)
{
	using namespace I8032_NameS;
	return internalIndirectDataMemory_[address-MIN_INTERNAL_INDIRECT_RAM_ADDRESS].isChangedAfterClear();
}

//Haetaan epäsuoran muistin tiedot luokkaa käyttävälle
//Haussa käytettävä oikeita osoitevälejä 0x80 - 0xFF
unsigned char I8032::readInternalIndirectDataMemory(unsigned int const  address)
{
	using namespace I8032_NameS;
	return internalIndirectDataMemory_[address-MIN_INTERNAL_INDIRECT_RAM_ADDRESS].readValue();
}

//Kirjoitetaan epäsuoran muistin tiedot luokkaa käyttävälle
//Kirjoituksessa käytettävä oikeita osoitevälejä 0x80 - 0xFF
void I8032::writeInternalIndirectDataMemory(unsigned int const  address, unsigned char const  value)
{
	using namespace I8032_NameS;
	if((address>=MIN_INTERNAL_INDIRECT_RAM_ADDRESS)&&(address<=MAX_INTERNAL_INDIRECT_RAM_ADDRESS))
		internalIndirectDataMemory_[address-MIN_INTERNAL_INDIRECT_RAM_ADDRESS]=value;
}

//(UM)Luetaan timerin arvo 16 bittisenä.
//Annetaan luettavan timerin numero parametrillä
unsigned int I8032::readTimerValue16(char const  timerNumber)
{
	using namespace I8032_NameS;
	unsigned int timerValue;
	if(timerNumber==2)
	{
		timerValue=internalDataMemory_[TH2].readValue();
		timerValue<<=8;
		timerValue|=internalDataMemory_[TL2].readValue();
	}
	else
	{
		//Haetaan arvo kantaluokasta
		timerValue=I8031::readTimerValue16(timerNumber);
	}
	return timerValue;
}


void I8032::formatInternalIndirectDataMemory()
{
	using namespace I8032_NameS;
	//Pitää lähteä nollasta koska taulukko
	unsigned int address;
	for(address=0;address<=MAX_INTERNAL_INDIRECT_RAM_ADDRESS-MIN_INTERNAL_INDIRECT_RAM_ADDRESS;address++)
	{
		internalIndirectDataMemory_[address].setInUse(true);
		internalIndirectDataMemory_[address].setIsBitAddressable(false);
		internalIndirectDataMemory_[address].setIsBitNames(false);
		internalIndirectDataMemory_[address].setIsDirect(false);
		internalIndirectDataMemory_[address].setIsIndirect(true);
		internalIndirectDataMemory_[address].setIsRegisterBank(false);
		internalIndirectDataMemory_[address].setIsSFR(false);
		internalIndirectDataMemory_[address].setIsPort(false);
	}
}

void I8032::createSFRBitNameTable(void)
{
	using namespace I8032_NameS;
	//T2CON:n bitit
	setSFRBitNameInUse(T2CON, BIT_NUMBER_0, "CP/RL2");
	setSFRBitNameInUse(T2CON, BIT_NUMBER_1, "C/T2");
	setSFRBitNameInUse(T2CON, BIT_NUMBER_2, "TR2");
	setSFRBitNameInUse(T2CON, BIT_NUMBER_3, "EXEN2");
	setSFRBitNameInUse(T2CON, BIT_NUMBER_4, "TCLK");
	setSFRBitNameInUse(T2CON, BIT_NUMBER_5, "RCLK");
	setSFRBitNameInUse(T2CON, BIT_NUMBER_6, "EXF2");
	setSFRBitNameInUse(T2CON, BIT_NUMBER_7, "TF2");

	//IE:n bitit
	setSFRBitNameInUse(IE, BIT_NUMBER_5, "ET2");
	//IP:n bitit
	setSFRBitNameInUse(IP, BIT_NUMBER_5, "PT2");
	//P1:n bitit
	setSFRBitNameInUse(P1, BIT_NUMBER_0, "T2");
	setSFRBitNameInUse(P1, BIT_NUMBER_1, "T2EX");
}

void I8032::createSFRNameTable(void)
{
	using namespace I8032_NameS;
	setSFRInUse(T2CON, "T2CON", false);
	setSFRInUse(RCAP2L, "RCAP2L", false);
	setSFRInUse(RCAP2H, "RCAP2H", false);
	setSFRInUse(TL2, "TL2", false);
	setSFRInUse(TH2, "TH2", false);
}

void I8032::pop_pc(void) //Tarvitaan keskeytysten käsittelyssä
{
	using namespace I8032_NameS;
	string sapu;
	if(internalDataMemory_[SP].readValue()<MIN_SFR_ADDRESS)
	{
		I8031::pop_pc();
	}
	else
	{
		if((internalDataMemory_[SP].readValue()>=MIN_INTERNAL_INDIRECT_RAM_ADDRESS) && (internalDataMemory_[SP].readValue()<=MAX_INTERNAL_INDIRECT_RAM_ADDRESS))
		{
			programCounter_=internalIndirectDataMemory_[internalDataMemory_[SP].readValue()-MIN_INTERNAL_INDIRECT_RAM_ADDRESS].readValue();
			programCounter_<<=8;
			--internalDataMemory_[SP];
			programCounter_|=internalIndirectDataMemory_[internalDataMemory_[SP].readValue()-MIN_INTERNAL_INDIRECT_RAM_ADDRESS].readValue();
			--internalDataMemory_[SP];
		}
		else
		{
			addErrorToCodeBufferStr(":BAD Stack address ");
			sapu=hexToString(internalDataMemory_[SP].readValue(), 2);
			sapu+="H!";
			addErrorToCodeBufferStr(sapu);
		}
	}
}

void I8032::push_pc(void)
{
	using namespace I8032_NameS;
	string sapu;
	if((internalDataMemory_[SP].readValue()+1)<MIN_SFR_ADDRESS)
	{
		I8031::push_pc();
	}
	else
	{
		if(((internalDataMemory_[SP].readValue()+1)>=MIN_INTERNAL_INDIRECT_RAM_ADDRESS) && ((internalDataMemory_[SP].readValue()+1)<=MAX_INTERNAL_INDIRECT_RAM_ADDRESS))
		{
			++internalDataMemory_[SP];
			internalIndirectDataMemory_[internalDataMemory_[SP].readValue()-MIN_INTERNAL_INDIRECT_RAM_ADDRESS]=programCounter_&0xFF;
			++internalDataMemory_[SP];
			internalIndirectDataMemory_[internalDataMemory_[SP].readValue()-MIN_INTERNAL_INDIRECT_RAM_ADDRESS]=(programCounter_>>8)&0xFF;
		}
		else
		{
			addErrorToCodeBufferStr(":BAD Stack address ");
			sapu=hexToString(internalDataMemory_[SP].readValue(), 2);
			sapu+="H!";
			addErrorToCodeBufferStr(sapu);
		}
	}
}

void I8032::pop(unsigned char dpop) //Tarvitaan keskeytysten käsittelyssä
{
	using namespace I8032_NameS;
	if(internalDataMemory_[SP].readValue()<MIN_SFR_ADDRESS)
	{
		I8031::pop(dpop);
	}
	else
	{
		//Jos pysytään alueella voidaan toimia
		if((internalDataMemory_[SP].readValue()>=MIN_INTERNAL_INDIRECT_RAM_ADDRESS) && (internalDataMemory_[SP].readValue()<=MAX_INTERNAL_INDIRECT_RAM_ADDRESS))
		{
			internalDataMemory_[dpop]=internalIndirectDataMemory_[internalDataMemory_[SP].readValue()-MIN_INTERNAL_INDIRECT_RAM_ADDRESS].readValue();
			--internalDataMemory_[SP];
		}
		else
		{
			addErrorToCodeBufferStr(":BAD Stack address ");
			string sapu;
			sapu=hexToString(internalDataMemory_[SP].readValue(), 2);
			sapu+="H!";
			addErrorToCodeBufferStr(sapu);
		}
	}
}

void I8032::push(unsigned char dpush)
{
	using namespace I8032_NameS;
	if((internalDataMemory_[SP].readValue()+1)<MIN_SFR_ADDRESS)
	{
		I8031::push(dpush);
	}
	else
	{
		//Jos pysytään alueella voidaan kirjoitella
		if(((internalDataMemory_[SP].readValue()+1)>=MIN_INTERNAL_INDIRECT_RAM_ADDRESS) && ((internalDataMemory_[SP].readValue()+1)<=MAX_INTERNAL_INDIRECT_RAM_ADDRESS))
		{
			++internalDataMemory_[SP];
			internalIndirectDataMemory_[internalDataMemory_[SP].readValue()-MIN_INTERNAL_INDIRECT_RAM_ADDRESS]=internalDataMemory_[dpush].readValue();
		}
		else
		{
			addErrorToCodeBufferStr(":BAD Stack address ");
			string sapu;
			sapu=hexToString(internalDataMemory_[SP].readValue(), 2);
			sapu+="H!";
			addErrorToCodeBufferStr(sapu);
		}
	}
}

unsigned int I8032::priority0IntCheck(void)
{
	using namespace I8032_NameS;
	unsigned int  retVal=I8031::priority0IntCheck();//Katsotaan kantaluokan keskeytykset
	if(!retVal)
	{
		//Timer2 prioriteetilla 0
		if(internalDataMemory_[T2CON].readValue()&(T2CON_TF2|T2CON_EXF2))
			if((priority0IntON_<TIMER2_INTBIT))
				if(!(internalDataMemory_[IP].readValue()&IP_PT2) && (internalDataMemory_[IE].readValue()&IE_ET2))
					return TIMER2_P0_INTON;
	}
	return retVal;
}

unsigned int I8032::priority1IntCheck(void)
{
	using namespace I8032_NameS;
	unsigned int  retVal=I8031::priority1IntCheck();//Katsotaan kantaluokan keskeytykset
	if(!retVal)
	{
		//Timer2 prioriteetilla 1
		if(internalDataMemory_[T2CON].readValue()&(T2CON_TF2|T2CON_EXF2))
			if(priority1IntON_<TIMER2_INTBIT)
				if((internalDataMemory_[IP].readValue()&IP_PT2) && (internalDataMemory_[IE].readValue()&IE_ET2))
					return TIMER2_P1_INTON;
	}
	return retVal;
}

bool I8032::putInterruptServiceOn(unsigned int const interruptToPutOn)
{
	using namespace I8032_NameS;
	switch(interruptToPutOn)
	{
		case TIMER2_P1_INTON:
			priority1IntON_|=TIMER2_INTBIT;
			programCounter_=TF2_INTERRUPT_VECTOR;
			return true;
		case TIMER2_P0_INTON:
			priority0IntON_|=TIMER2_INTBIT;
			programCounter_=TF2_INTERRUPT_VECTOR;
			return true;
	}
	return I8031::putInterruptServiceOn(interruptToPutOn);
}

bool I8032::priority0IntReturn(void)
{
	using namespace I8032_NameS;
	bool retVal=I8031::priority0IntReturn();
	if((!retVal)&&(priority0IntON_))
	{
		if(priority0IntON_&TIMER2_INTBIT)
		{
			priority0IntON_&=~TIMER2_INTBIT;
			return true;
		}
	}
	return retVal;
}
bool I8032::priority1IntReturn(void)
{
	using namespace I8032_NameS;
	bool retVal=I8031::priority1IntReturn();
	if((!retVal)&&(priority1IntON_))
	{
		if(priority1IntON_&TIMER2_INTBIT)
		{
			priority1IntON_&=~TIMER2_INTBIT;
			return true;
		}
	}
	return retVal;
}

//Ohjaa sarjaportin lähetystä eri moodeissa
//Muutos edelliseen kantaluokkaan Timer2:n käyttömahdollisuus baudien laskentaan
void I8032::serialPortRxTxHandling(void)
{
	using namespace I8032_NameS;
	//Jos T2Con TCLK bitti on asettunut timer2Overflov
	unsigned char t2ConTclk=internalDataMemory_[T2CON].readValue()&T2CON_TCLK;
	if(t2ConTclk)
		uart_->stepRxTx(clockTicksCounted_, dynamic_cast<TimersCounters2*>(timersCounters_)->timer2Overflow());
	uart_->stepRxTx(clockTicksCounted_, timersCounters_->timer1Overflow());
}


unsigned int I8032::readDataFromInternalDataMemoryIndirectlyUsingRBRegister(unsigned int r)
{
	using namespace I8032_NameS;
	int bankAddress=8*((internalDataMemory_[PSW].readValue()>>3)&0x03);
	if((internalDataMemory_[REGISTER_BANK0_ADDRESS+(r&BIT_0)+bankAddress].readValue()) < MIN_INTERNAL_INDIRECT_RAM_ADDRESS)
	{
		//Käytetään kantaluokan metodia
		return I8031::readDataFromInternalDataMemoryIndirectlyUsingRBRegister(r);
	}
	else
	{
		unsigned char internalAddress=internalDataMemory_[REGISTER_BANK0_ADDRESS+(r&BIT_0)+bankAddress].readValue();
		return internalIndirectDataMemory_[internalAddress-MIN_INTERNAL_INDIRECT_RAM_ADDRESS].readValue();
	}

}

void I8032::writeDataToInternalDataMemoryIndirectlyUsingRBRegister(unsigned int r, unsigned int val)
{
	using namespace I8032_NameS;
	int bankAddress=8*((internalDataMemory_[PSW].readValue()>>3)&0x03);
	if((internalDataMemory_[REGISTER_BANK0_ADDRESS+(r&BIT_0)+bankAddress].readValue()) < MIN_INTERNAL_INDIRECT_RAM_ADDRESS)
	{
		//Käytetään kantaluokan metodia
		I8031::writeDataToInternalDataMemoryIndirectlyUsingRBRegister(r, val);
	}
	else
	{
		internalIndirectDataMemory_[internalDataMemory_[REGISTER_BANK0_ADDRESS+(r&BIT_0)+bankAddress].readValue()-MIN_INTERNAL_INDIRECT_RAM_ADDRESS]=val;
	}
}

void I8032::clearInternalIndirectDataMemory(void)
{
	using namespace I8032_NameS;
	for(int n=MIN_INTERNAL_INDIRECT_RAM;n<=MAX_INTERNAL_INDIRECT_RAM;n++)
	{
		internalIndirectDataMemory_[n].clear();
	}
}

void I8032::saveWorkDataToFile(QDataStream& saveFileStream)
{
	using namespace I8032_NameS;
	I8031::saveWorkDataToFile(saveFileStream);

	unsigned long address;
	for(address=MIN_INTERNAL_INDIRECT_RAM_ADDRESS; address<MIN_INTERNAL_INDIRECT_RAM_ADDRESS+INTERNAL_INDIRECT_RAM_SIZE; address++)
	{
		saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)isInternalIndirectDataMemoryChangedAfterClear(address);
		saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)readInternalIndirectDataMemory(address);
		saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
	}

	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)tf2P0_InterruptServiceIsOn_;
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)tf2P1_InterruptServiceIsOn_;

	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void I8032::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	using namespace I8032_NameS;
	
	I8031::readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);

	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	Q_UINT8 value8Bool;
	Q_UINT8 value8Data;
	unsigned long address;

	for(address=MIN_INTERNAL_INDIRECT_RAM_ADDRESS; address<MIN_INTERNAL_INDIRECT_RAM_ADDRESS+INTERNAL_INDIRECT_RAM_SIZE; address++)
	{
		readFileStream>>indexReadData>>typeReadData>>value8Bool;      //Muuttunut bitti
		readFileStream>>indexReadData>>typeReadData>>value8Data;
		readFileStream>>indexReadData; //INDEX_SAVEDWORKDATA_END
		if(value8Bool)
			writeInternalIndirectDataMemory(address,value8Data);
	}

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case 0:
				readFileStream>>typeReadData>>value8Bool;
				tf2P0_InterruptServiceIsOn_=(bool)value8Bool;
				break;
			case 1:
				readFileStream>>typeReadData>>value8Bool;
				tf2P1_InterruptServiceIsOn_=(bool)value8Bool;
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

