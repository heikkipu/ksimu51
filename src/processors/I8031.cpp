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
#include "I8031.h"
#include "../ksimu51.h"



I8031::I8031()
{
	using namespace I8031_NameS;
	
	clockCyclesPerMachineCycle_=12;
	clockCyclesClockTicksCounted_=6; //Puolet konejakson kellopulsseista
	maxClockSpeed_=I8031::MAX_CLOCKSPEED;
	clockSpeed_=DEFAULT_CLOCKSPEED;
	processorName_="8031";
	
	formatInternalDataMemory();
	createSFRBitNameTable();
	createSFRNameTable();
	
	createPins();
	pinALE_Index_=pinList_.readIndex(PINNAME_ALE);
	pinPSEN_Index_=pinList_.readIndex(PINNAME_PSEN);
	pinRESET_Index_=pinList_.readIndex(PINNAME_RESET);
	pinEA_Index_=pinList_.readIndex(PINNAME_EA);

	instructions_=new InstructionSet();
	externalCycleList_=new ExternalCycle(pinALE_Index_, pinPSEN_Index_, pinRESET_Index_, pinEA_Index_, P0, P1, P2, P3);
	
	SerialPort::RegSCONBits bitsToSCON;
	bitsToSCON.bitRI=SCON_RI;
	bitsToSCON.bitTI=SCON_TI;
	bitsToSCON.bitRB8=SCON_RB8;
	bitsToSCON.bitTB8=SCON_TB8;
	bitsToSCON.bitREN=SCON_REN;
	bitsToSCON.bitSM2=SCON_SM2;
	bitsToSCON.bitSM1=SCON_SM1;
	bitsToSCON.bitSM0=SCON_SM0;
	SerialPort::RxTxPins pinsRxTx;
	pinsRxTx.pinTx=P3_TXD;
	pinsRxTx.pinRx=P3_RXD;
	uart_=new SerialPort(&internalDataMemory_[SCON], bitsToSCON, &internalDataMemory_[SBUF], &internalDataMemory_[P3], pinsRxTx);

	timersCounters_=new TimersCounters(&internalDataMemory_[TL0], &internalDataMemory_[TH0], &internalDataMemory_[TL1], &internalDataMemory_[TH1], &internalDataMemory_[TMOD], &internalDataMemory_[TCON], &internalDataMemory_[IE], &internalDataMemory_[P3]);
	
	clearBitAddressableArea();
	clearRegisterBanks();
	clearScratchPadArea();
	clearExternalDataMemory();
	clearProgramMemory();

	programSize_=0;
	internalDataMemory_[SBUF]=0; //Laitetaan tässä joku arvo koska resetissä Indeterminate. (Päivittyy)

	resetState=true;
	interruptDone_=false; //Reti käskyn jälkeen päällä yhden stepin: tässä alkualustus
	movx16bAWriteCycle_=false;
	movx8bAWriteCycle_=false;
	codeBufferStr_="";

	lastExecutionProgramCounterValue_=0;
	lastExecutionMachineCycleCounterValue_=0;
	priority0IntON_=0;
	priority1IntON_=0;
}

//Hajotin
I8031::~I8031()
{
	delete uart_;
	delete instructions_;
	delete externalCycleList_;
	delete timersCounters_;
}

void I8031::changeClockSpeed(I8031_NameS::clockTicks_Type speed)
{
	if(speed<=maxClockSpeed_)
		clockSpeed_=speed;
}

//Luodaan yksittäiset pinnit.
void I8031::createPins(void)
{
	pinList_.addPin(PINNAME_PSEN, true, IOPin::PINTYPE_OUTPUT);//Outti 0 aktiivi, joten päälle
	pinList_.addPin(PINNAME_ALE, false, IOPin::PINTYPE_OUTPUT);//Outti 1 aktiivi
	pinList_.addPin(PINNAME_EA, false, IOPin::PINTYPE_INPUT);//Inputti. oletuksena ulkoinen ohjelmamuisti, koska sisäistä ei ole
	pinList_.addPin(PINNAME_RESET, true, IOPin::PINTYPE_INPUT); //Asetetaan resetti aluksi päälle pitää muistaa laskea alas.
}


void I8031::togglePortPin(unsigned int const portAddress, unsigned int const pinNumber)
{
	if(internalDataMemory_[portAddress].isPort()&&pinNumber<8)
		internalDataMemory_[portAddress]^=static_cast<unsigned char>(1<<pinNumber);
}

int I8031::readPortPin(unsigned char const portAddress, unsigned char const pinNumber)
{
	if(internalDataMemory_[portAddress].isPort()&&pinNumber<8)
		return static_cast<int>(readInternalDataMemory(portAddress)&static_cast<unsigned char>(1<<pinNumber));
	return -1;
}

void I8031::writePortPin(unsigned char const portAddress, unsigned char const pinNumber, bool const value)
{
	if(internalDataMemory_[portAddress].isPort()&&pinNumber<8)
		if(value)
			internalDataMemory_[portAddress]|=static_cast<unsigned char>(1<<pinNumber);
		else
			internalDataMemory_[portAddress]&=~(static_cast<unsigned char>(1<<pinNumber));
}

IOPinList* I8031::readPins(void)
{
	return &pinList_;
}

void I8031::writeInputPin(string name, bool pinValue)
{
	pinList_.writeInputPin(name, pinValue, false);
}

void I8031::toggleInputPin(string name)
{
	pinList_.toggleInputPin(name, false);
}

void I8031::reset()
{
	using namespace I8031_NameS;
	//Reset arvot rekistereille
	programCounter_=0;
	pinList_.setPin(PINNAME_PSEN, true, false);
	pinList_.setPin(PINNAME_ALE, false, false);
	internalDataMemory_[ACC]=0;
	internalDataMemory_[B]=0;
	internalDataMemory_[IE]=0;
	internalDataMemory_[IP]=0;
	internalDataMemory_[PSW]=0;
	internalDataMemory_[P0]=0xff;
	internalDataMemory_[P1]=0xff;
	internalDataMemory_[P2]=0xff;
	internalDataMemory_[P3]=0xff;
	internalDataMemory_[SP]=0x7;
	internalDataMemory_[DPL]=0;
	internalDataMemory_[DPH]=0;
	internalDataMemory_[TCON]=0;
	internalDataMemory_[TMOD]=0;
	internalDataMemory_[TL0]=0;
	internalDataMemory_[TL1]=0;
	internalDataMemory_[TH0]=0;
	internalDataMemory_[TH1]=0;
	internalDataMemory_[SCON]=0;
	//internalDataMemory_[SBUF]=0; Indeterminate
	codeBufferStr_="RESETED";
	resetState=false;
	
	interruptStarted_=false;
	priority0IntON_=0;
	priority1IntON_=0;

	externalCycleList_->clear_Vecs(); //Nollataan cycle lista
	uart_->reset();
	timersCounters_->reset();

	reset_VirtualPart();
}

//Tehdään syväresetti, clearaa kaikki muistialueet
void I8031::powerOnReset(void)
{
	machineCycleCounter_=0;
	clockTicksCalculated_=0;
	clockTicksCounted_=0;
	reset();
	clearBitAddressableArea();
	clearRegisterBanks();
	clearScratchPadArea();
	clearExternalDataMemory();
	
	powerOnReset_VirtualPart();
}

bool I8031::isExternalDataMemoryChanged(unsigned int const address)
{
	return externalDataMemory_[address].isChanged();
}

bool I8031::isExternalDataMemoryChangedAfterClear(unsigned int const address)
{
	return externalDataMemory_[address].isChangedAfterClear();
}

unsigned int I8031::readExternalDataMemory(unsigned int const address)
{
	return externalDataMemory_[address].readValue();
}

void I8031::writeExternalDataMemory(unsigned int const address, unsigned char const value)
{
	externalDataMemory_[address]=value;
}

void I8031::clearExternalDataMemory()
{
	using namespace I8031_NameS;
	for(unsigned int n=MIN_EXTERNAL_DATA_MEMORY_ADDRESS;n<=MAX_EXTERNAL_DATA_MEMORY_ADDRESS;n++)
		externalDataMemory_[n].clear();
}

unsigned char I8031::readExternalProgramMemory(unsigned int const address) const
{
	return externalProgramMemory_[address];
}

unsigned char I8031::registerBankInUse(void)
{
	using namespace I8031_NameS;
	unsigned char v=internalDataMemory_[PSW].readValue();
	return ((v>>3)&0x03);
}

//Luetaan datpointteri. 
unsigned int I8031::readDPTRValue(void)
{
	using namespace I8031_NameS;
	unsigned int dptrAddress=internalDataMemory_[DPH].readValue();
	dptrAddress<<=8;
	dptrAddress|=internalDataMemory_[DPL].readValue();
	return dptrAddress;
}

//Luetaan timerin arvo 16 bittisenä.
//Annetaan luettavan timerin numero parametrillä
//Voidaan uudelleenmäärittää
unsigned int I8031::readTimerValue16(char const timerNumber)
{
	using namespace I8031_NameS;
	unsigned int timerValue;
	if(timerNumber==1)
	{
		timerValue=internalDataMemory_[TH1].readValue();
		timerValue<<=8;
		timerValue|=internalDataMemory_[TL1].readValue();
	}
	else
	{
		timerValue=internalDataMemory_[TH0].readValue();
		timerValue<<=8;
		timerValue|=internalDataMemory_[TL0].readValue();
	}
	return timerValue;
}

bool I8031::isInternalDataMemoryLocationChanged(unsigned int const address)
{
	return internalDataMemory_[address].isChanged();
}

bool I8031::isInternalDataMemoryLocationChangedAfterClear(unsigned int const address)
{
	return internalDataMemory_[address].isChangedAfterClear();
}

void I8031::writeInternalDataMemory(unsigned int const address, unsigned char const value)
{
	using namespace I8031_NameS;
	if(address<INTERNAL_DATA_MEMORY_SIZE)
		internalDataMemory_[address]=value;
}

unsigned char I8031::readInternalDataMemory(unsigned int const address)
{
	return internalDataMemory_[address].readValue();
}

bool I8031::isInternalDataMemoryLocationInUse(unsigned int const address)
{
	return internalDataMemory_[address].isInUse();
}

bool I8031::isSFRBitNamesInUse(unsigned char sfrAddress)
{
	return internalDataMemory_[sfrAddress].isBitNames();
}

bool I8031::isSFRRegisterIOPort(unsigned char const sfrAddress)
{
	return internalDataMemory_[sfrAddress].isPort();
}

void I8031::clearProgramMemory()
{
	using namespace I8031_NameS;
	for(unsigned int n=MIN_EXTERNAL_PROGRAM_MEMORY_ADDRESS;n<=MAX_EXTERNAL_PROGRAM_MEMORY_ADDRESS;n++)
		externalProgramMemory_[n]=0xFF; //Kaikki bitit päälle koska EPROM tms.
	programSize_=0;
}

void I8031::writeProgramMemory(unsigned int const address, unsigned char const value)
{
	externalProgramMemory_[address]=value;
	//Ohjelman koon laskeminen.
	if(address>programSize_)
		programSize_=address;
}

void I8031::formatInternalDataMemory(void)
{
	using namespace I8031_NameS;
	unsigned int address;
	//Rekisteripankkialue
	for(address=MIN_REGISTER_BANK_ADDRESS;address<=MAX_REGISTER_BANK_ADDRESS;address++)
	{
		internalDataMemory_[address].setInUse(true);
		internalDataMemory_[address].setIsBitAddressable(false);
		internalDataMemory_[address].setIsBitNames(false);
		internalDataMemory_[address].setIsDirect(true);
		internalDataMemory_[address].setIsIndirect(true);
		internalDataMemory_[address].setIsRegisterBank(true);
		internalDataMemory_[address].setIsSFR(false);
		internalDataMemory_[address].setIsPort(false);
	}

	//Bittialue
	for(address=MIN_BIT_AREA_ADDRESS;address<=MAX_BIT_AREA_ADDRESS;address++)
	{
		internalDataMemory_[address].setInUse(true);
		internalDataMemory_[address].setIsBitAddressable(true);
		internalDataMemory_[address].setIsBitNames(false);
		internalDataMemory_[address].setIsDirect(true);
		internalDataMemory_[address].setIsIndirect(true);
		internalDataMemory_[address].setIsRegisterBank(false);
		internalDataMemory_[address].setIsSFR(false);
		internalDataMemory_[address].setIsPort(false);
	}

	//Scrad pad alue
	for(address=MIN_SCRATCH_PAD_ADDRESS;address<=MAX_SCRATCH_PAD_ADDRESS;address++)
	{
		internalDataMemory_[address].setInUse(true);
		internalDataMemory_[address].setIsBitAddressable(false);
		internalDataMemory_[address].setIsBitNames(false);
		internalDataMemory_[address].setIsDirect(true);
		internalDataMemory_[address].setIsIndirect(true);
		internalDataMemory_[address].setIsRegisterBank(false);
		internalDataMemory_[address].setIsSFR(false);
		internalDataMemory_[address].setIsPort(false);
	}

	//SFR
	for(address=MIN_SFR_ADDRESS;address<=MAX_SFR_ADDRESS;address++)
	{
		internalDataMemory_[address].setInUse(false);//Alustetaan pois käytöstä.
		if((address%8)==0)
			internalDataMemory_[address].setIsBitAddressable(true);
		else
			internalDataMemory_[address].setIsBitAddressable(false);
		internalDataMemory_[address].setIsBitNames(false);
		internalDataMemory_[address].setIsDirect(true);
		internalDataMemory_[address].setIsIndirect(true);
		internalDataMemory_[address].setIsRegisterBank(false);
		internalDataMemory_[address].setIsSFR(true);
		internalDataMemory_[address].setIsPort(false);
	}

}

bool I8031::isSerialPortTaransmitOn(unsigned char const port)
{
	using namespace I8031_NameS;
	if(port==UART)
		return uart_->isTaransmitOn();
	return false;
}

int I8031::readSerialPortTransmittedValue(unsigned char const port)
{
	using namespace I8031_NameS;
	if(port==UART)
		return uart_->readTransmittedValue();
	return -1;
}

void I8031::charToSerialPortReceiveBuff(unsigned char const port, int const data)
{
	using namespace I8031_NameS;
	if(port==UART)
		uart_->charToReceiveBuff(data);
}

int I8031::readSerialPortRxBits(unsigned char const port)
{
	using namespace I8031_NameS;
	int rv=-1;
	if(port==UART)
	{
		unsigned int uval=uart_->readInputShiftReg(rv);
		rv<<=12;
		rv|=uval&0x0FFF;
	}
	return rv;
}

int I8031::readSerialPortTxBits(unsigned char const port)
{
	using namespace I8031_NameS;
	int rv=-1;
	if(port==UART)
	{
		unsigned int uval=uart_->readTransmitSbuf(rv);
		rv<<=12;
		rv|=uval&0x0FFF;
	}
	return rv;
}

void I8031::serialPortRxTxHandling(void)
{
	//uart_->stepRxTx(clockTicksCalculated_, timersCounters_->timer1Overflow());
	uart_->stepRxTx(clockTicksCounted_, timersCounters_->timer1Overflow());
}

void I8031::timersAndCountersHandling(void)
{
	//timersCounters_->step(clockTicksCalculated_);
	timersCounters_->step(clockTicksCounted_);
}

void I8031::pop_pc(void) //Tarvitaan keskeytysten käsittelyssä
{
	using namespace I8031_NameS;
	if(internalDataMemory_[SP].readValue()<MIN_SFR_ADDRESS)
	{
		programCounter_=internalDataMemory_[internalDataMemory_[SP].readValue()].readValue();
		programCounter_<<=8;
		--internalDataMemory_[SP];
		programCounter_|=internalDataMemory_[internalDataMemory_[SP].readValue()].readValue();
		--internalDataMemory_[SP];
	}
	else
	{
		addErrorToCodeBufferStr(":BAD Stack address!");
		string sapu;
		sapu=hexToString(internalDataMemory_[SP].readValue(),2);
		sapu+="H!";
		addErrorToCodeBufferStr(sapu);
	}
}


void I8031::push_pc(void)
{
	using namespace I8031_NameS;
	//Jos pysytään alueella voidaan kirjoitella
	if((internalDataMemory_[SP].readValue()+static_cast<unsigned int>(1))<MIN_SFR_ADDRESS)
	{
		++internalDataMemory_[SP];
		internalDataMemory_[internalDataMemory_[SP].readValue()]=static_cast<unsigned char>(programCounter_&0xFF);
		++internalDataMemory_[SP];
		internalDataMemory_[internalDataMemory_[SP].readValue()]=static_cast<unsigned char>((programCounter_>>8)&0xFF);
	}
	else
	{
		addErrorToCodeBufferStr(":BAD Stack address!");
		string sapu;
		sapu=hexToString(internalDataMemory_[SP].readValue(),2);
		sapu+="H!";
		addErrorToCodeBufferStr(sapu);
	}
}

void I8031::push(unsigned char dpush)
{
	using namespace I8031_NameS;
	//Jos pysytään alueella voidaan kirjoitella
	if(static_cast<unsigned int>(internalDataMemory_[SP].readValue()+1)<MIN_SFR_ADDRESS)
	{
		++internalDataMemory_[SP];
		internalDataMemory_[internalDataMemory_[SP].readValue()]=internalDataMemory_[dpush].readValue();
	}
	else
	{
		addErrorToCodeBufferStr(":BAD Stack address!");
		string sapu;
		sapu=hexToString(internalDataMemory_[SP].readValue(),2);
		sapu+="H!";
		addErrorToCodeBufferStr(sapu);
	}
}

void I8031::pop(unsigned char dpop)
{
	using namespace I8031_NameS;
	//Jos pysytään alueella voidaan kirjoitella
	if((internalDataMemory_[SP].readValue())<MIN_SFR_ADDRESS)
	{
		internalDataMemory_[dpop]=internalDataMemory_[internalDataMemory_[SP].readValue()].readValue();
		--internalDataMemory_[SP];
	}
	else
	{
		addErrorToCodeBufferStr(":BAD Stack address!");
		string sapu;
		sapu=hexToString(internalDataMemory_[SP].readValue(),2);
		sapu+="H!";
		addErrorToCodeBufferStr(sapu);
	}
}

//Tämä on virtuaalinen jonka perivä luokka voi muuttaa, jos eri ulkoisen keskytyksen pinnejä
bool I8031::readExternalIntPin(char const exInterrupt)
{
	using namespace I8031_NameS;
	bool pinValue=true;
	switch(exInterrupt)
	{
		case 0:
			if(!(internalDataMemory_[P3].readValue()&P3_INT0))
				pinValue=false;
			break;
		case 1:
			if(!(internalDataMemory_[P3].readValue()&P3_INT1))
				pinValue=false;
			break;
	}
	return pinValue;
}

//Tämänkin voi perivä muutta, jos enmmän ulkoisia keskytyksiä
void I8031::lookIfExternalInterrupts(void)
{
	using namespace I8031_NameS;
	bool externalInt0Pin=readExternalIntPin(0);
	bool externalInt1Pin=readExternalIntPin(1);
	//Jos sallittu keskytys 0
	if((internalDataMemory_[IE].readValue()&IE_EA)&&(internalDataMemory_[IE].readValue()&IE_EX0))
	{
		if(internalDataMemory_[TCON].readValue()&TCON_IT0) //Jos keskeytys 0 reuna-aktiivi laskeva
		{
			if(previousExternalInt0Pin_&&!externalInt0Pin)
				internalDataMemory_[TCON]|=static_cast<unsigned char>(TCON_IE0);
		}
		else                                          //Tasoaktiivi
		{
			if(!externalInt0Pin)
				internalDataMemory_[TCON]|=static_cast<unsigned char>(TCON_IE0);
		}
	}

	//Jos sallittu keskytys 1
	if((internalDataMemory_[IE].readValue()&IE_EA)&&(internalDataMemory_[IE].readValue()&IE_EX1))
	{
		if(internalDataMemory_[TCON].readValue()&TCON_IT1) //Jos keskeytys 1 reuna-aktiivi
		{
			if(previousExternalInt1Pin_&&!externalInt1Pin)
				internalDataMemory_[TCON]|=static_cast<unsigned char>(TCON_IE1);
		}
		else                                          //Tasoaktiivi
		{
			if(!externalInt1Pin)
				internalDataMemory_[TCON]|=static_cast<unsigned char>(TCON_IE1);
		}
	}

	//Pinnin tilat muistiin
	//INT0
	previousExternalInt0Pin_=externalInt0Pin;
	//INT1
	previousExternalInt1Pin_=externalInt1Pin;
}

unsigned int I8031::priority0IntCheck(void)
{
	using namespace I8031_NameS;
	if(!priority1IntON_) //Jos korkeampi prioriteetti ei käynnissä
	{
		//External0 prioriteetilla 0
		if((internalDataMemory_[TCON].readValue()&TCON_IE0))
			if(priority0IntON_<EXTINT0_INTBIT)
				if(!(internalDataMemory_[IP].readValue()&IP_PX0) && (internalDataMemory_[IE].readValue()&IE_EX0))
					return EXTINT0_P0_INTON;
		//Timer0 prioriteetilla 0
		if(internalDataMemory_[TCON].readValue()&TCON_TF0)
			if(priority0IntON_<TIMER0_INTBIT)
				if(!(internalDataMemory_[IP].readValue()&IP_PT0) && (internalDataMemory_[IE].readValue()&IE_ET0))
					return TIMER0_P0_INTON;
		//External1 prioriteetilla 0
		if(internalDataMemory_[TCON].readValue()&TCON_IE1)
			if(priority0IntON_<EXTINT1_INTBIT)
				if(!(internalDataMemory_[IP].readValue()&IP_PX1) && (internalDataMemory_[IE].readValue()&IE_EX1))
					return EXTINT1_P0_INTON;
		//Timer1 prioriteetilla 0
		if(internalDataMemory_[TCON].readValue()&TCON_TF1)
			if(priority0IntON_<TIMER1_INTBIT)
				if(!(internalDataMemory_[IP].readValue()&IP_PT1) && (internalDataMemory_[IE].readValue()&IE_ET1))
					return TIMER1_P0_INTON;
		//SERIAL Prioriteetilla 0
		if((internalDataMemory_[SCON].readValue()&SCON_RI)||(internalDataMemory_[SCON].readValue()&SCON_TI))
			if(priority0IntON_<SERIALPORT_INTBIT)
				if(!(internalDataMemory_[IP].readValue()&IP_PS) && (internalDataMemory_[IE].readValue()&IE_ES))
					return SERIALPORT_P0_INTON;
   }
	return 0;
}

unsigned int I8031::priority1IntCheck(void)
{
	using namespace I8031_NameS;
	//External0 prioriteetilla 1
	if(internalDataMemory_[TCON].readValue()&TCON_IE0)
		if(priority1IntON_<EXTINT0_INTBIT)
			if((internalDataMemory_[IP].readValue()&IP_PX0)&&(internalDataMemory_[IE].readValue()&IE_EX0))
				return EXTINT0_P1_INTON;
	//Timer0 prioriteetilla 1
	if(internalDataMemory_[TCON].readValue()&TCON_TF0)
		if(priority1IntON_<TIMER0_INTBIT)
			if((internalDataMemory_[IP].readValue()&IP_PT0) && (internalDataMemory_[IE].readValue()&IE_ET0))
				return TIMER0_P1_INTON;
	//External1 prioriteetilla 1
	if(internalDataMemory_[TCON].readValue()&TCON_IE1)
		if(priority1IntON_<EXTINT1_INTBIT)
			if((internalDataMemory_[IP].readValue()&IP_PX1) && (internalDataMemory_[IE].readValue()&IE_EX1))
				return EXTINT1_P1_INTON;
	//Timer1 prioriteetilla 1
	if(internalDataMemory_[TCON].readValue()&TCON_TF1)
		if(priority1IntON_<TIMER1_INTBIT)
			if((internalDataMemory_[IP].readValue()&IP_PT1) && (internalDataMemory_[IE].readValue()&IE_ET1))
				return TIMER1_P1_INTON;
	//SERIAL Prioriteetilla 1
	if((internalDataMemory_[SCON].readValue()&SCON_RI)||(internalDataMemory_[SCON].readValue()&SCON_TI))
		if(priority1IntON_<SERIALPORT_INTBIT)
			if((internalDataMemory_[IP].readValue()&IP_PS) && (internalDataMemory_[IE].readValue()&IE_ES))
				return SERIALPORT_P1_INTON;
	
	return 0;
}

bool I8031::putInterruptServiceOn(unsigned int const interruptToPutOn)
{
	using namespace I8031_NameS;
	switch(interruptToPutOn)
	{
		//Priority 0
		case EXTINT0_P0_INTON:
			priority0IntON_|=EXTINT0_INTBIT;
			programCounter_=EXTINT0_INTERRUPT_VECTOR;
			return true;
		case TIMER0_P0_INTON:
			priority0IntON_|=TIMER0_INTBIT;
			programCounter_=TIMER0_INTERRUPT_VECTOR;
			return true;
		case EXTINT1_P0_INTON:
			priority0IntON_|=EXTINT1_INTBIT;
			programCounter_=EXTINT1_INTERRUPT_VECTOR;
			return true;
		case TIMER1_P0_INTON:
			priority0IntON_|=TIMER1_INTBIT;
			programCounter_=TIMER1_INTERRUPT_VECTOR;
			return true;
		case SERIALPORT_P0_INTON:
			priority0IntON_|=SERIALPORT_INTBIT;
			programCounter_=SERIALPORT_INTERRUPT_VECTOR;
			return true;
		//Priority1
		case EXTINT0_P1_INTON:
			priority1IntON_|=EXTINT0_INTBIT;
			programCounter_=EXTINT0_INTERRUPT_VECTOR;
			return true;
		case TIMER0_P1_INTON:
			priority1IntON_|=TIMER0_INTBIT;
			programCounter_=TIMER0_INTERRUPT_VECTOR;
			return true;
		case EXTINT1_P1_INTON:
			priority1IntON_|=EXTINT1_INTBIT;
			programCounter_=EXTINT1_INTERRUPT_VECTOR;
			return true;
		case TIMER1_P1_INTON:
			priority1IntON_|=TIMER1_INTBIT;
			programCounter_=TIMER1_INTERRUPT_VECTOR;
			return true;
		case SERIALPORT_P1_INTON:
			priority1IntON_|=SERIALPORT_INTBIT;
			programCounter_=SERIALPORT_INTERRUPT_VECTOR;
			return true;
	}
	return false;
}

void I8031::handlingInterrupts(void)
{
	using namespace I8031_NameS;
	if(internalDataMemory_[IE].readValue()&IE_EA)      //Jos keskeytykset sallittu
	{
		interruptStarted_=false;
		unsigned int intti=priority1IntCheck();
		if(!intti)
			intti=priority0IntCheck();
		if(intti)
		{
			push_pc();
			interruptStarted_=putInterruptServiceOn(intti);
		}
	}
}

bool I8031::priority0IntReturn(void)
{
	using namespace I8031_NameS;
	if(priority0IntON_&EXTINT0_INTBIT) //Ext Int0 P0
	{
		internalDataMemory_[TCON]&=~TCON_IE0;
		priority0IntON_&=~EXTINT0_INTBIT;
		return true;
	}
	if(priority0IntON_&TIMER0_INTBIT) //Timer0 P0
	{
		internalDataMemory_[TCON]&=~TCON_TF0;
		priority0IntON_&=~TIMER0_INTBIT;
		return true;
	}
	if(priority0IntON_&EXTINT1_INTBIT) //Ext int1 P0
	{
		internalDataMemory_[TCON]&=~TCON_IE1;
		priority0IntON_&=~EXTINT1_INTBIT;
		return true;
	}
	if(priority0IntON_&TIMER1_INTBIT) //Timer1 P0
	{
		internalDataMemory_[TCON]&=~TCON_TF1;
		priority0IntON_&=~TIMER1_INTBIT;
		return true;
	}
	if(priority0IntON_&SERIALPORT_INTBIT) //Serial Rx P0
	{
		priority0IntON_&=~SERIALPORT_INTBIT;
		return true;
	}
	return false;
}
bool I8031::priority1IntReturn(void)
{
	using namespace I8031_NameS;
	if(priority1IntON_)
	{
		if(priority1IntON_&EXTINT0_INTBIT) //Ext Int0 P1
		{
			internalDataMemory_[TCON]&=~TCON_IE0;
			priority1IntON_&=~EXTINT0_INTBIT;
			return true;
		}
		if(priority1IntON_&TIMER0_INTBIT) //Timer0 P1
		{
			internalDataMemory_[TCON]&=~TCON_TF0;
			priority1IntON_&=~TIMER0_INTBIT;
			return true;
		}
		if(priority1IntON_&EXTINT1_INTBIT) //Ext int1 P1
		{
			internalDataMemory_[TCON]&=~TCON_IE1;
			priority1IntON_&=~EXTINT1_INTBIT;
			return true;
		}
		if(priority1IntON_&TIMER1_INTBIT) //Timer1 P1
		{
			internalDataMemory_[TCON]&=~TCON_TF1;
			priority1IntON_&=~TIMER1_INTBIT;
			return true;
		}
		if(priority1IntON_&SERIALPORT_INTBIT) //Serial  P1
		{
			priority1IntON_&=~SERIALPORT_INTBIT;
			return true;
		}
	}
	return false;
}

//Reti käskyllä paluu:
void I8031::handlingInterruptReturn(void) //Perivä luokka kirjoittaa
{
	if(!priority1IntReturn())
		priority0IntReturn();
	pop_pc();
}


unsigned char I8031::readInstructionDoCycle(bool const readOnlyInstruction)
{
	using namespace I8031_NameS;
	unsigned char retVal=externalProgramMemory_[programCounter_];
	if(!readOnlyInstruction)
	{
		internalDataMemory_[P0]=static_cast<unsigned char>(programCounter_&0x00FF);
		internalDataMemory_[P2]=static_cast<unsigned char>((programCounter_>>8)&0x00FF);

		//Lisätään external cycle listaa.
		alePulses_Type aPulses=externalCycleList_->appendToExternalCycle_ReadFromExternalCodeMemoryCycle(internalDataMemory_[P0].readValue(), internalDataMemory_[P2].readValue(), externalProgramMemory_[programCounter_], false);
		oneStepAlePulses_+=aPulses;
		addClockTicksAndHandlePeripherals(aPulses);
		
		internalDataMemory_[P0]=externalProgramMemory_[programCounter_]; //Portin tila sellaiseksi kun se on suorituksen jälkeen
		programCounter_++;
	}
	return retVal;
}

//-----------------------------------------------------------------------------
//Käskyjen käsittelyt: step kutsuu näitä
//-----------------------------------------------------------------------------
void I8031::add_A_RNx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	unsigned int apu=readDataFromRBRegisterDirectly(instruction);
	unsigned char bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()+apu);
	addSetFlags(internalDataMemory_[ACC].readValue(), apu, false);
	internalDataMemory_[ACC]=bapu;
}

void I8031::add_A_DIRECT(unsigned int const directAddress)
{
	using namespace I8031_NameS;
	//pitää tarkistaa onko direct osoite SBUF, jos on haetaan tieto sbufReceive:stä SBUF:iin
	if(directAddress==SBUF)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart_->readReceiveSbuf());

	unsigned char bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()+internalDataMemory_[directAddress].readValue());
	addSetFlags(internalDataMemory_[ACC].readValue(), internalDataMemory_[directAddress].readValue(), false);
	internalDataMemory_[ACC]=bapu;
}

void I8031::add_A_RIx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	unsigned int apu=readDataFromInternalDataMemoryIndirectlyUsingRBRegister(instruction);
	unsigned char bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()+apu);
	addSetFlags(internalDataMemory_[ACC].readValue(), apu, false);
	internalDataMemory_[ACC]=bapu;
}

void I8031::add_A_DATA(unsigned int const data)
{
	using namespace I8031_NameS;
	unsigned char bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()+data);
	addSetFlags(internalDataMemory_[ACC].readValue(), data, false);
	internalDataMemory_[ACC]=bapu;
}

void I8031::addc_A_RNx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	unsigned char bapu;
	unsigned int apu=readDataFromRBRegisterDirectly(instruction);
	if(internalDataMemory_[PSW].readValue()&PSW_CY)
		bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()+apu+1);
	else
		bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()+apu);

	addSetFlags(internalDataMemory_[ACC].readValue(), apu, true);
	internalDataMemory_[ACC]=bapu;
}

void I8031::addc_A_DIRECT(unsigned int const directAddress)
{
	using namespace I8031_NameS;
	unsigned char bapu;
	//pitää tarkistaa onko direct osoite SBUF, jos on haetaan tieto sbufReceive:stä SBUF:iin
	if(directAddress==SBUF)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart_->readReceiveSbuf());
	if(internalDataMemory_[PSW].readValue()&PSW_CY)
		bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()+1+internalDataMemory_[directAddress].readValue());
	else
		bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()+internalDataMemory_[directAddress].readValue());
	addSetFlags(internalDataMemory_[ACC].readValue(), internalDataMemory_[directAddress].readValue(), true);
	internalDataMemory_[ACC]=bapu;
}

void I8031::addc_A_RIx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	unsigned char bapu;
	unsigned int apu=readDataFromInternalDataMemoryIndirectlyUsingRBRegister(instruction);
	if(internalDataMemory_[PSW].readValue()&PSW_CY)
		bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()+1+apu);
	else
		bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()+apu);

	addSetFlags(internalDataMemory_[ACC].readValue(), apu, true);
	internalDataMemory_[ACC]=bapu;
}

void I8031::addc_A_DATA(unsigned int const data)
{
	using namespace I8031_NameS;
	unsigned char bapu;
	if(internalDataMemory_[PSW].readValue()&PSW_CY)
		bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()+1+data);
	else
		bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()+data);

	addSetFlags(internalDataMemory_[ACC].readValue(), data, true);
	internalDataMemory_[ACC]=bapu;
}

void I8031::subb_A_RNx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	unsigned char bapu;
	unsigned int apu=readDataFromRBRegisterDirectly(instruction);
	if(internalDataMemory_[PSW].readValue()&PSW_CY)
		bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()-1-apu);
	else
		bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()-apu);

	subbSetFlags(internalDataMemory_[ACC].readValue(), apu);
	internalDataMemory_[ACC]=bapu;
}

void I8031::subb_A_DIRECT(unsigned int const directAddress)
{
	using namespace I8031_NameS;
	unsigned char bapu;
	//pitää tarkistaa onko direct osoite SBUF, jos on haetaan tieto sbufReceive:stä SBUF:iin
	if(directAddress==SBUF)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart_->readReceiveSbuf());

	if(internalDataMemory_[PSW].readValue()&PSW_CY)
		bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()-1-internalDataMemory_[directAddress].readValue());
	else
		bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()-internalDataMemory_[directAddress].readValue());
	subbSetFlags(internalDataMemory_[ACC].readValue(), internalDataMemory_[directAddress].readValue());
	internalDataMemory_[ACC]=bapu;
}

void I8031::subb_A_RIx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	unsigned char bapu;
	unsigned int apu=readDataFromInternalDataMemoryIndirectlyUsingRBRegister(instruction);
	if(internalDataMemory_[PSW].readValue()&PSW_CY)
		bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()-1-apu);
	else
		bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()-apu);

	subbSetFlags(internalDataMemory_[ACC].readValue(), apu);
	internalDataMemory_[ACC]=bapu;
}

void I8031::subb_A_DATA(unsigned int const data)
{
	using namespace I8031_NameS;
	unsigned char bapu;
	if(internalDataMemory_[PSW].readValue()&PSW_CY)
		bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()-1-data);
	else
		bapu=static_cast<unsigned char>(internalDataMemory_[ACC].readValue()-data);

	subbSetFlags(internalDataMemory_[ACC].readValue(), data);
	internalDataMemory_[ACC]=bapu;
}

void I8031::inc_A(void)
{
	using namespace I8031_NameS;
	++internalDataMemory_[ACC];
}

void I8031::inc_RNx(unsigned char const instruction)
{
	unsigned int apu=readDataFromRBRegisterDirectly(instruction);
	apu++;
	writeDataToRBRegisterDirectly(instruction, apu);
}

void I8031::inc_DIRECT(unsigned int const directAddress)
{
	using namespace I8031_NameS;
	//pitää tarkistaa onko direct osoite SBUF, jos on haetaan tieto sbufReceive:stä SBUF:iin
	if(directAddress==SBUF)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart_->readReceiveSbuf());
	//Tässä SBUFFAUS tämä on kyllä iso??

	++internalDataMemory_[directAddress];

	//Niin sitten sen pitäs käynnistää lähetys koska kirjotus tapahtuu SBUFFIIN
	if(directAddress==SBUF)
		uart_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);
}

void I8031::inc_RIx(unsigned char const instruction)
{
	unsigned int apu=readDataFromInternalDataMemoryIndirectlyUsingRBRegister(instruction);
	apu++;
	writeDataToInternalDataMemoryIndirectlyUsingRBRegister(instruction, apu);
}

void I8031::inc_DPTR(void)
{
	using namespace I8031_NameS;
	unsigned int apu=internalDataMemory_[DPL].readValue();
	apu++;
	if(apu>0xFF)                           //Jos ylivuoto lisätään myös yläosaa DPTR:a
		++internalDataMemory_[DPH];
	internalDataMemory_[DPL]=apu&0x00FF;
}

void I8031::dec_A(void)
{
	using namespace I8031_NameS;
	--internalDataMemory_[ACC];
}

void I8031::dec_RNx(unsigned char const instruction)
{
	unsigned int apu=readDataFromRBRegisterDirectly(instruction);
	apu--;
	writeDataToRBRegisterDirectly(instruction, apu);
}

void I8031::dec_DIRECT(unsigned int const directAddress)
{
	using namespace I8031_NameS;
	//pitää tarkistaa onko direct osoite SBUF, jos on haetaan tieto sbufReceive:stä SBUF:iin
	if(directAddress==SBUF)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart_->readReceiveSbuf());
	//Tässä SBUFFAUS tämä on kyllä iso??

	--internalDataMemory_[directAddress];

	//pitää tarkistaa onko direct osoite SBUF, jos on kirjotetaan tieto myös sbufTransmittiin
	if(directAddress==SBUF)
		uart_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);
}

void I8031::dec_RIx(unsigned char const instruction)
{
	unsigned int apu=readDataFromInternalDataMemoryIndirectlyUsingRBRegister(instruction);
	apu--;
	writeDataToInternalDataMemoryIndirectlyUsingRBRegister(instruction, apu);
}

void I8031::mul_AB(void)
{
	using namespace I8031_NameS;
	unsigned int apu=internalDataMemory_[ACC].readValue()*internalDataMemory_[B].readValue();
	if(apu>0xFF)                                    //Asetetaan Overflow
		internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_OV);
	else
		internalDataMemory_[PSW]&=~PSW_OV;

	internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY); //Carry clearataan aina
	internalDataMemory_[ACC]=static_cast<unsigned char>(apu);
	apu>>=8;
	internalDataMemory_[B]=static_cast<unsigned char>(apu);
}

void I8031::div_AB(void)
{
	using namespace I8031_NameS;
	internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY); //Carry clearataan aina
	if((internalDataMemory_[B].readValue())!=0x00)
	{
		unsigned char apu=internalDataMemory_[ACC].readValue()/internalDataMemory_[B].readValue();
		unsigned char bapu=internalDataMemory_[ACC].readValue()%internalDataMemory_[B].readValue();
		internalDataMemory_[ACC]=apu;
		internalDataMemory_[B]=bapu;
		internalDataMemory_[PSW]&=~PSW_OV;            //Clearataan OV
	}
	else
	{
		internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_OV);         //OV päälle koska jakaja 0
	}
}

void I8031::da_A(void)
{
	using namespace I8031_NameS;
	unsigned int apu=internalDataMemory_[ACC].readValue();
	//Jos akun alaosa >9 tai AC=1
	if(((apu&0x0F)>9)||(internalDataMemory_[PSW].readValue()&PSW_AC))
		apu+=0x06;//Lisätään kuusi

	//Jos ylivuoto,
	if(apu>0xFF)
		internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY);//CY päälle

	//Jos carry tai akun yläosa yli yheksän
	if((apu>0xFF) || (((apu&0x00F0)>>4)>9))
		apu+=0x60; //Lisätään akun yläosaa kuudella

	//Jos ylivuoto
	if(apu>0xFF)
		internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY);

	internalDataMemory_[ACC]=static_cast<unsigned char>(apu);
}

void I8031::anl_A_RNx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	unsigned int apu=readDataFromRBRegisterDirectly(instruction);
	internalDataMemory_[ACC]&=static_cast<unsigned char>(apu);
}

void I8031::anl_A_DIRECT(unsigned int const directAddress)
{
	using namespace I8031_NameS;
	//pitää tarkistaa onko direct osoite SBUF, jos on haetaan tieto sbufReceive:stä SBUF:iin
	if(directAddress==SBUF)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart_->readReceiveSbuf());

	internalDataMemory_[ACC]&=internalDataMemory_[directAddress].readValue();
}

void I8031::anl_A_RIx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	unsigned int apu=readDataFromInternalDataMemoryIndirectlyUsingRBRegister(instruction);
	internalDataMemory_[ACC]&=static_cast<unsigned char>(apu);
}

void I8031::anl_A_DATA(unsigned int const data)
{
	using namespace I8031_NameS;
	internalDataMemory_[ACC]&=static_cast<unsigned char>(data);
}

void I8031::anl_DIRECT_A(unsigned int const directAddress)
{
	using namespace I8031_NameS;
	internalDataMemory_[directAddress]&=internalDataMemory_[ACC].readValue();

	//pitää tarkistaa onko direct osoite SBUF, jos on kirjotetaan tieto sbufTransmittiin
	//Toimiikohan oikea prosu näin: en ole koskaan kokeillut
	if(directAddress==SBUF)
		uart_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);
}

void I8031::anl_DIRECT_DATA(unsigned int const directAddress, unsigned int const data)
{
	using namespace I8031_NameS;
	internalDataMemory_[directAddress]&=static_cast<unsigned char>(data);
	//pitää tarkistaa onko direct osoite SBUF, jos on kirjotetaan tieto myös sbufTransmittiin
	if(directAddress==SBUF)
		uart_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);
}

void I8031::orl_A_RNx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	unsigned int apu=readDataFromRBRegisterDirectly(instruction);
	internalDataMemory_[ACC]|=static_cast<unsigned char>(apu);
}

void I8031::orl_A_DIRECT(unsigned int const directAddress)
{
	using namespace I8031_NameS;
	//pitää tarkistaa onko direct osoite SBUF, jos on haetaan tieto sbufReceive:stä SBUF:iin
	if(directAddress==SBUF)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart_->readReceiveSbuf());
	
	internalDataMemory_[ACC]|=internalDataMemory_[directAddress].readValue();
}

void I8031::orl_A_RIx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	unsigned int apu=readDataFromInternalDataMemoryIndirectlyUsingRBRegister(instruction);
	internalDataMemory_[ACC]|=static_cast<unsigned char>(apu);
}

void I8031::orl_A_DATA(unsigned int const data)
{
	using namespace I8031_NameS;
	internalDataMemory_[ACC]|=static_cast<unsigned char>(data);
}

void I8031::orl_DIRECT_A(unsigned int const directAddress)
{
	using namespace I8031_NameS;
	internalDataMemory_[directAddress]|=internalDataMemory_[ACC].readValue();
	//pitää tarkistaa onko direct osoite SBUF, jos on kirjotetaan tieto sbufTransmittiin
	if(directAddress==SBUF)
		uart_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);
}

void I8031::orl_DIRECT_DATA(unsigned int const directAddress, unsigned int const data)
{
	using namespace I8031_NameS;
	internalDataMemory_[directAddress]|=static_cast<unsigned char>(data);
	//pitää tarkistaa onko direct osoite SBUF, jos on kirjotetaan tieto sbufTransmittiin
	if(directAddress==SBUF)
		uart_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);
}

void I8031::xrl_A_RNx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	unsigned int apu=readDataFromRBRegisterDirectly(instruction);
	internalDataMemory_[ACC]^=static_cast<unsigned char>(apu);
}

void I8031::xrl_A_DIRECT(unsigned int const directAddress)
{
	using namespace I8031_NameS;
	//pitää tarkistaa onko direct osoite SBUF, jos on haetaan tieto sbufReceive:stä SBUF:iin
	if(directAddress==SBUF)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart_->readReceiveSbuf());

	internalDataMemory_[ACC]^=internalDataMemory_[directAddress].readValue();
}

void I8031::xrl_A_RIx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	unsigned int apu=readDataFromInternalDataMemoryIndirectlyUsingRBRegister(instruction);
	internalDataMemory_[ACC]^=static_cast<unsigned char>(apu);
}

void I8031::xrl_A_DATA(unsigned int const data)
{
	using namespace I8031_NameS;
	internalDataMemory_[ACC]^=static_cast<unsigned char>(data);
}

void I8031::xrl_DIRECT_A(unsigned int const directAddress)
{
	using namespace I8031_NameS;
	internalDataMemory_[directAddress]^=internalDataMemory_[ACC].readValue();
	//pitää tarkistaa onko direct osoite SBUF, jos on kirjotetaan tieto sbufTransmittiin
	if(directAddress==SBUF)
		uart_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);
}

void I8031::xrl_DIRECT_DATA(unsigned int const directAddress, unsigned int const data)
{
	using namespace I8031_NameS;
	internalDataMemory_[directAddress]^=static_cast<unsigned char>(data);
	//pitää tarkistaa onko direct osoite SBUF, jos on kirjotetaan tieto sbufTransmittiin
	if(directAddress==SBUF)
		uart_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);
}

void I8031::clr_A(void)
{
	using namespace I8031_NameS;
	internalDataMemory_[ACC]=0x00;
}

void I8031::cpl_A(void)
{
	using namespace I8031_NameS;
	internalDataMemory_[ACC]^=0xFF;         //Käännetään bitit
}

void I8031::rl_A(void)
{
	using namespace I8031_NameS;
	unsigned int apu=internalDataMemory_[ACC].readValue()<<1;
	if(apu>0xFF)                                 //Jos 1 tuli yli b7 niin heitetään se b0 paikalle
		apu|=1;

	internalDataMemory_[ACC]=static_cast<unsigned char>(apu);
}

void I8031::rlc_A(void)
{
	using namespace I8031_NameS;
	unsigned int apu=internalDataMemory_[ACC].readValue()<<1;
	if(internalDataMemory_[PSW].readValue()&PSW_CY)          //Jos Carryssä oli niin b0 paikalle
		apu|=1;

	if(apu>0xFF)                                 //Jos 1 tuli yli b7 niin heitetään se Carryyn
		internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY);
	else
		internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY); //Muutoin nollataan carry

	internalDataMemory_[ACC]=static_cast<unsigned char>(apu);
}

void I8031::rr_A(void)
{
	using namespace I8031_NameS;
	unsigned int apu=internalDataMemory_[ACC].readValue()>>1;
	if(internalDataMemory_[ACC].readValue()&BIT_0)      //Jos nollas bitti i niin heitä se kasiin
		apu|=BIT_7;
	internalDataMemory_[ACC]=static_cast<unsigned char>(apu);
}

void I8031::rrc_A(void)
{
	using namespace I8031_NameS;
	unsigned int apu=internalDataMemory_[ACC].readValue();
	if(internalDataMemory_[PSW].readValue()&PSW_CY)    //Jos Carry, heitä se kasiin
		apu|=BIT_8;

	if(apu&BIT_0)   //Jos b0 niin heitä se Carryyn
		internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY);
	else
		internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY); //Muutoin nollataan carry

	apu>>=1;
	internalDataMemory_[ACC]=static_cast<unsigned char>(apu);
}

void I8031::swap_A(void)
{
	using namespace I8031_NameS;
	unsigned int apu=internalDataMemory_[ACC].readValue()>>4;
	internalDataMemory_[ACC]<<=4;
	internalDataMemory_[ACC]|=apu&0x0F;
}

void I8031::mov_A_RNx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	internalDataMemory_[ACC]=readDataFromRBRegisterDirectly(instruction);
}

void I8031::mov_A_DIRECT(unsigned int const directAddress)
{
	using namespace I8031_NameS;
	//pitää tarkistaa onko direct osoite SBUF, jos on haetaan tieto sbufReceive:stä SBUF:iin
	if(directAddress==SBUF)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart_->readReceiveSbuf());

	internalDataMemory_[ACC]=internalDataMemory_[directAddress].readValue();
}

void I8031::mov_A_RIx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	internalDataMemory_[ACC]=readDataFromInternalDataMemoryIndirectlyUsingRBRegister(instruction);
}

void I8031::mov_A_DATA(unsigned int const data)
{
	using namespace I8031_NameS;
	internalDataMemory_[ACC]=static_cast<unsigned char>(data);
}

void I8031::mov_RNx_A(unsigned char const instruction)
{
	using namespace I8031_NameS;
	writeDataToRBRegisterDirectly(instruction, internalDataMemory_[ACC].readValue());
}

void I8031::mov_RNx_DIRECT(unsigned char const instruction, unsigned int const directAddress)
{
	using namespace I8031_NameS;
	//pitää tarkistaa onko direct osoite SBUF, jos on haetaan tieto sbufReceive:stä SBUF:iin
	if(directAddress==SBUF)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart_->readReceiveSbuf());

	writeDataToRBRegisterDirectly(instruction, internalDataMemory_[directAddress].readValue());
}

void I8031::mov_RNx_DATA(unsigned char const instruction, unsigned int const data)
{
	writeDataToRBRegisterDirectly(instruction, data);
}

void I8031::mov_DIRECT_A(unsigned int const directAddress)
{
	using namespace I8031_NameS;
	internalDataMemory_[directAddress]=internalDataMemory_[ACC].readValue();
	//pitää tarkistaa onko direct osoite SBUF, jos on kirjotetaan tieto sbufTransmittiin
	if(directAddress==SBUF)
		uart_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);
}

void I8031::mov_DIRECT_RNx(unsigned char const instruction, unsigned int const directAddress)
{
	using namespace I8031_NameS;
	internalDataMemory_[directAddress]=readDataFromRBRegisterDirectly(instruction);
	//pitää tarkistaa onko direct osoite SBUF, jos on kirjotetaan tieto sbufTransmittiin
	if(directAddress==SBUF)
		uart_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);
}

void I8031::mov_DIRECT_DIRECT(unsigned int const srcDirectAddress, unsigned int const dstDirectAddress)
{
	using namespace I8031_NameS;
	//pitää tarkistaa onko direct src osoite SBUF, jos on haetaan tieto sbufReceive:stä SBUF:iin
	if(srcDirectAddress==SBUF)
		internalDataMemory_[srcDirectAddress]=static_cast<unsigned char>(uart_->readReceiveSbuf());

	internalDataMemory_[dstDirectAddress]=internalDataMemory_[srcDirectAddress].readValue();
	//pitää tarkistaa onko direct osoite SBUF, jos on kirjotetaan tieto sbufTransmittiin
	if(dstDirectAddress==SBUF)
		uart_->startTransmit(internalDataMemory_[dstDirectAddress].readValue(), clockTicksCounted_);
}

void I8031::mov_DIRECT_RIx(unsigned char const instruction, unsigned int const directAddress)
{
	using namespace I8031_NameS;
	internalDataMemory_[directAddress]=readDataFromInternalDataMemoryIndirectlyUsingRBRegister (instruction);
	//pitää tarkistaa onko direct osoite SBUF, jos on kirjotetaan tieto sbufTransmittiin
	if(directAddress==SBUF)
		uart_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);
}

void I8031::mov_DIRECT_DATA(unsigned int const directAddress, unsigned int const data)
{
	using namespace I8031_NameS;
	internalDataMemory_[directAddress]=static_cast<unsigned char>(data);
	//pitää tarkistaa onko direct osoite SBUF, jos on kirjotetaan tieto sbufTransmittiin
	if(directAddress==SBUF)
		uart_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);
}

void I8031::mov_RIx_A(unsigned char const instruction)
{
	using namespace I8031_NameS;
	writeDataToInternalDataMemoryIndirectlyUsingRBRegister(instruction, internalDataMemory_[ACC].readValue());
}

void I8031::mov_RIx_DIRECT(unsigned char const instruction, unsigned int const directAddress)
{
	using namespace I8031_NameS;
	//pitää tarkistaa onko direct src osoite SBUF, jos on haetaan tieto sbufReceive:stä SBUF:iin
	if(directAddress==SBUF)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart_->readReceiveSbuf());

	writeDataToInternalDataMemoryIndirectlyUsingRBRegister(instruction, internalDataMemory_[directAddress].readValue());
}

void I8031::mov_RIx_DATA(unsigned char const instruction, unsigned int const data)
{
	writeDataToInternalDataMemoryIndirectlyUsingRBRegister(instruction, data);
}

void I8031::mov_DPTR_DATA16(unsigned char const dph, unsigned char const dpl)
{
	using namespace I8031_NameS;
	internalDataMemory_[DPH]=dph;
	internalDataMemory_[DPL]=dpl;
}

void I8031::movc_A_A_DPTR(void)
{
	using namespace I8031_NameS;
	programCounter_Type pcValue=programCounter_;
	programCounter_=readDPTRValue();      //Haetaan DPTR arvo
	programCounter_+=internalDataMemory_[ACC].readValue();      //Lisätään akun arvo
	internalDataMemory_[ACC]=readInstructionDoCycle(DO_CYCLE_ALSO);//Luetaan ohjelmamuistista
	programCounter_=pcValue;
}

void I8031::movc_A_A_PC(void)
{
	using namespace I8031_NameS;
	programCounter_Type pcValue=programCounter_;
	programCounter_+=internalDataMemory_[ACC].readValue();
	internalDataMemory_[ACC]=readInstructionDoCycle(DO_CYCLE_ALSO); //Luetaan ojelmamuistista.
	programCounter_=pcValue;
}

void I8031::movx_A_RIx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	//Testataan mikä pankki käytössä ja sen mukaan haetaan data epäsuorasti ExRAM:sta R0:lla tai R1:llä
	unsigned char registerBankAddress=static_cast<unsigned char>(8*((internalDataMemory_[PSW].readValue()>>3)&0x03));
	//Osoite porttiin:
	internalDataMemory_[P0]=internalDataMemory_[REGISTER_BANK0_ADDRESS+registerBankAddress+(instruction&BIT_0)].readValue();
	//Data akkuun muistista
	internalDataMemory_[ACC]=externalDataMemory_[internalDataMemory_[P0].readValue()].readValue(); //P0:sta ext osoite.
	addressInP0AfterALE_=internalDataMemory_[P0].readValue(); //Talletetaan portin 0 ale jälkeinen tila

	//Lisätään cycle listaa. Osoite saadaan P0:sta. Tässä 8 bitin osoite, joten P2 voi olla 0.
	alePulses_Type aPulses=externalCycleList_->appendToExternalCycle_ReadFromDataMemoryCycle(internalDataMemory_[P0].readValue(), 0, internalDataMemory_[ACC].readValue(), true);
	oneStepAlePulses_+=aPulses;
	addClockTicksAndHandlePeripherals(aPulses);

	internalDataMemory_[P0]=internalDataMemory_[ACC].readValue(); //Portin tila suorituksen jälkeiseen tilaan.
	internalDataMemory_[P3]|=(P3_WR|P3_RD);   //WR ja RD jää ylös
}

void I8031::movx_RIx_A(unsigned char const instruction)
{
	using namespace I8031_NameS;
	//Testataan mikä pankki käytössä ja sen mukaan haetaan data epäsuorasti ExRAM:sta R0:lla tai R1:llä
	unsigned char registerBankAddress=static_cast<unsigned char>(8*((internalDataMemory_[PSW].readValue()>>3)&0x03));
	//Osoite porttiin:
	internalDataMemory_[P0]=internalDataMemory_[REGISTER_BANK0_ADDRESS+registerBankAddress+(instruction&BIT_0)].readValue();
	//Data akusta muistiin
	externalDataMemory_[internalDataMemory_[P0].readValue()]=internalDataMemory_[ACC].readValue();//P0:sta ext osoite
	movx8bAWriteCycle_=true;
	addressInP0AfterALE_=internalDataMemory_[P0].readValue(); //Talletetaan portin 0 ale jälkeinen tila

	//Lisätään cycle listaa. Osoite saadaan P0:sta. Tässä 8 bitin osoite, joten P2 voi olla 0.
	alePulses_Type aPulses=externalCycleList_->appendToExternalCycle_WriteToDataMemoryCycle(internalDataMemory_[P0].readValue(), 0, internalDataMemory_[ACC].readValue(), true);
	oneStepAlePulses_+=aPulses;
	addClockTicksAndHandlePeripherals(aPulses);

	internalDataMemory_[P0]=internalDataMemory_[ACC].readValue(); //Portin tila suorituksen jälkeisen tilaan
	internalDataMemory_[P3]|=(P3_WR|P3_RD);   //WR ja RD jää ylös
}

void I8031::movx_A_DPTR(void)
{
	using namespace I8031_NameS;
	unsigned int apu=readDPTRValue();      //Haetaan DPTR arvo
	//Osoite portteihin:
	internalDataMemory_[P0]=static_cast<unsigned char>(apu&0x00FF);      internalDataMemory_[P2]=static_cast<unsigned char>((apu>>8)&0x00FF);
	//Data muistista akkuun
	internalDataMemory_[ACC]=externalDataMemory_[apu].readValue();
	addressInP0AfterALE_=internalDataMemory_[P0].readValue(); //Talletetaan portin 0 ale jälkeinen tila
	addressInP2AfterALE_=internalDataMemory_[P2].readValue(); //Talletetaan portin 2 ale jälkeinen tila

	//Lisätään cycle listaa. Osoite saadaan P0:sta ja P2:sta. Akusta portin tila.
	alePulses_Type aPulses=externalCycleList_->appendToExternalCycle_ReadFromDataMemoryCycle(internalDataMemory_[P0].readValue(), internalDataMemory_[P2].readValue(), internalDataMemory_[ACC].readValue(), false);
	oneStepAlePulses_+=aPulses;
	addClockTicksAndHandlePeripherals(aPulses);

	internalDataMemory_[P0]=internalDataMemory_[ACC].readValue(); //Portin tila suorituksen jälkeisen tilaan
	internalDataMemory_[P3]|=(P3_WR|P3_RD);   //WR ja RD jää ylös
}

void I8031::movx_DPTR_A(void)
{
	using namespace I8031_NameS;
	unsigned int apu=readDPTRValue();      //Haetaan DPTR arvo
	//Osoite portteihin:
	internalDataMemory_[P0]=static_cast<unsigned char>(apu&0x00FF);
	internalDataMemory_[P2]=static_cast<unsigned char>((apu>>8)&0x00FF);
	//Data akusta muistialueelle
	externalDataMemory_[apu]=internalDataMemory_[ACC].readValue();
	movx16bAWriteCycle_=true;
	addressInP0AfterALE_=internalDataMemory_[P0].readValue(); //Talletetaan portin 0 ale jälkeinen tila
	addressInP2AfterALE_=internalDataMemory_[P2].readValue(); //Talletetaan portin 2 ale jälkeinen tila

	//Lisätään cycle listaa. Osoite saadaan P0:sta ja P2:sta. Akusta portin tila.
	alePulses_Type aPulses=externalCycleList_->appendToExternalCycle_WriteToDataMemoryCycle(internalDataMemory_[P0].readValue(), internalDataMemory_[P2].readValue(), internalDataMemory_[ACC].readValue(), false);
	oneStepAlePulses_+=aPulses;
	addClockTicksAndHandlePeripherals(aPulses);

	internalDataMemory_[P0]=internalDataMemory_[ACC].readValue(); //Portin tila suorituksen jälkeisen tilaan
	internalDataMemory_[P3]|=(P3_WR|P3_RD);   //WR ja RD jää ylös
}

void I8031::pop_DIRECT(unsigned int const directAddress)
{
	using namespace I8031_NameS;
	//SP:n popataan eri tavalla
	if(directAddress==SP)
		internalDataMemory_[SP]=internalDataMemory_[internalDataMemory_[SP].readValue()].readValue();
	else
		pop(static_cast<unsigned char>(directAddress));
	//pitää tarkistaa onko direct osoite SBUF, jos on kirjotetaan tieto sbufTransmittiin
	if(directAddress==SBUF)
		uart_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);
}

void I8031::push_DIRECT(unsigned int const directAddress)
{
	using namespace I8031_NameS;
	//pitää tarkistaa onko direct src osoite SBUF, jos on haetaan tieto sbufReceive:stä SBUF:iin
	if(directAddress==SBUF)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart_->readReceiveSbuf());

	push(static_cast<unsigned char>(directAddress));
}

void I8031::xch_A_RNx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	unsigned char apu=readDataFromRBRegisterDirectly(instruction);
	writeDataToRBRegisterDirectly(instruction, internalDataMemory_[ACC].readValue());
	internalDataMemory_[ACC]=apu;
}

void I8031::xch_A_DIRECT(unsigned int const directAddress)
{
	using namespace I8031_NameS;
	//pitää tarkistaa onko direct src osoite SBUF, jos on haetaan tieto sbufReceive:stä SBUF:iin
	if(directAddress==SBUF)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart_->readReceiveSbuf());

	unsigned char bapu=internalDataMemory_[directAddress].readValue();
	internalDataMemory_[directAddress]=internalDataMemory_[ACC].readValue();
	internalDataMemory_[ACC]=bapu;
}

void I8031::xch_A_RIx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	unsigned char apu=readDataFromInternalDataMemoryIndirectlyUsingRBRegister(instruction);
	writeDataToInternalDataMemoryIndirectlyUsingRBRegister(instruction, internalDataMemory_[ACC].readValue());
	internalDataMemory_[ACC]=apu;
}

void I8031::xchd_A_RIx(unsigned char const instruction)
{
	using namespace I8031_NameS;
	unsigned char apu=readDataFromInternalDataMemoryIndirectlyUsingRBRegister(instruction);
	unsigned char bapu=apu&0xF0;
	bapu=static_cast<unsigned char>(bapu|(internalDataMemory_[ACC].readValue()&0x0F));
	writeDataToInternalDataMemoryIndirectlyUsingRBRegister(instruction, bapu);
	internalDataMemory_[ACC]&=0xF0;
	internalDataMemory_[ACC]|=apu&0x0F;
}

void I8031::clr_C(void)
{
	using namespace I8031_NameS;
	internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY);
}

void I8031::clr_BIT(unsigned char const bitAddress)
{
	using namespace I8031_NameS;
	manipulateDirectBitWithBitAddress(bitAddress, CLR_BIT);
}

void I8031::setb_C(void)
{
	using namespace I8031_NameS;
	internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY);
}

void I8031::setb_BIT(unsigned char bitAddress)
{
	using namespace I8031_NameS;
	manipulateDirectBitWithBitAddress(bitAddress, SETB_BIT);
}

void I8031::cpl_C(void)
{
	using namespace I8031_NameS;
	internalDataMemory_[PSW]^=static_cast<unsigned char>(PSW_CY);
}

void I8031::cpl_BIT(unsigned char bitAddress)
{
	using namespace I8031_NameS;
	manipulateDirectBitWithBitAddress(bitAddress, CPL_BIT);
}

void I8031::anl_C_BIT(unsigned char bitAddress)
{
	using namespace I8031_NameS;
	manipulateCarryWithBitAddress(bitAddress, ANL_C_BIT);
}

void I8031::anl_C_NBIT(unsigned char bitAddress)
{
	using namespace I8031_NameS;
	manipulateCarryWithBitAddress(bitAddress, ANL_C_NBIT);
}

void I8031::orl_C_BIT(unsigned char bitAddress)
{
	using namespace I8031_NameS;
	manipulateCarryWithBitAddress(bitAddress, ORL_C_BIT);
}

void I8031::orl_C_NBIT(unsigned char bitAddress)
{
	using namespace I8031_NameS;
	manipulateCarryWithBitAddress(bitAddress, ORL_C_NBIT);
}

void I8031::mov_C_BIT(unsigned char bitAddress)
{
	using namespace I8031_NameS;
	manipulateCarryWithBitAddress(bitAddress, MOV_C_BIT);
}

void I8031::mov_BIT_C(unsigned char bitAddress)
{
	using namespace I8031_NameS;
	manipulateCarryWithBitAddress(bitAddress, MOV_BIT_C);
}

void I8031::jc_REL(char const relAddress)
{
	using namespace I8031_NameS;
	if(internalDataMemory_[PSW].readValue()&PSW_CY)
		programCounter_+=relAddress;
}

void I8031::jnc_REL(char const relAddress)
{
	using namespace I8031_NameS;
	if(!(internalDataMemory_[PSW].readValue()&PSW_CY))
		programCounter_+=relAddress;
}

void I8031::jb_BIT_REL(unsigned char const bitAddress, char const relAddress)
{
	using namespace I8031_NameS;
	jumpBitManipulation(bitAddress, relAddress, JB_BIT_REL);
}

void I8031::jnb_BIT_REL(unsigned char const bitAddress, char const relAddress)
{
	using namespace I8031_NameS;
	jumpBitManipulation(bitAddress, relAddress, JNB_BIT_REL);
}

void I8031::jbc_BIT_REL(unsigned char const bitAddress, char const relAddress)
{
	using namespace I8031_NameS;
	jumpBitManipulation(bitAddress, relAddress, JBC_BIT_REL);
}

void I8031::acall_x00(unsigned char const instruction, unsigned int addressLow)
{
	push_pc();
	unsigned int apu=instruction&0xE0;            //Maskataan käskyosa pois
	apu<<=3;         //Liitetään osoitteet yhteen
	apu|=addressLow;
	programCounter_=apu;
}

void I8031::lcall(unsigned char const addressHigh, unsigned char const addressLow)
{
	push_pc();
	unsigned int apu=addressHigh;
	apu<<=8;         //Liitostetaan
	apu|=addressLow;
	programCounter_=apu;
}

void I8031::ret(void)
{
	pop_pc();
}

void I8031::reti(void)
{
	handlingInterruptReturn();
}

void I8031::ajmp_x00(unsigned char const instruction, unsigned int const addressLow)
{
	unsigned int apu=instruction&0xE0;      //Maskataan käskyosa pois
	apu<<=3;                     //Liitetään osoitteet yhteen
	apu|=addressLow;
	programCounter_=apu;
}

void I8031::ljmp(unsigned char const addressHigh, unsigned char const addressLow)
{
	unsigned int apu=addressHigh;
	apu<<=8;         //Liitostetaan
	apu|=addressLow;
	programCounter_=apu;
}

void I8031::sjmp(char const relAddress)
{
	programCounter_+=relAddress;      //Käytetään etumerkillistä, niin saadaan rel osoite
}

void I8031::jmp_A_DPTR(void)
{
	using namespace I8031_NameS;
	unsigned int apu=readDPTRValue();      //Haetaan DPTR arvo
	programCounter_=internalDataMemory_[ACC].readValue()+apu;
}

void I8031::jz_REL(char const relAddress)
{
	using namespace I8031_NameS;
	if(!internalDataMemory_[ACC].readValue())
		programCounter_+=relAddress;
}

void I8031::jnz_REL(char const relAddress)
{
	using namespace I8031_NameS;
	if(internalDataMemory_[ACC].readValue())
		programCounter_+=relAddress;
}

void I8031::cjne_A_DIRECT_REL(unsigned int const directAddress, char const relAddress)
{
	using namespace I8031_NameS;
	//pitää tarkistaa onko direct src osoite SBUF, jos on haetaan tieto sbufReceive:stä SBUF:iin
	if(directAddress==SBUF)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart_->readReceiveSbuf());

	if((internalDataMemory_[ACC].readValue()) != (internalDataMemory_[directAddress].readValue()))
		programCounter_+=relAddress;

	if((internalDataMemory_[ACC].readValue()) < (internalDataMemory_[directAddress].readValue()))
		internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY);
	else
		internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY);
}

void I8031::cjne_A_DATA_REL(unsigned char const data, char const relAddress)
{
	using namespace I8031_NameS;
	if(internalDataMemory_[ACC].readValue() != data)
		programCounter_+=relAddress;

	if(internalDataMemory_[ACC].readValue() < data)
		internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY);
	else
		internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY);
}

void I8031::cjne_RNx_DATA_REL(unsigned char const instruction, unsigned char const data, char const relAddress)
{
	using namespace I8031_NameS;
	unsigned int apu=readDataFromRBRegisterDirectly(instruction);
	if(apu != data)
		programCounter_+=relAddress;

	if(apu < data)
		internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY);
	else
		internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY);
}

void I8031::cjne_RIx_DATA_REL(unsigned char const instruction, unsigned char const data, char const relAddress)
{
	using namespace I8031_NameS;
	unsigned int apu=readDataFromInternalDataMemoryIndirectlyUsingRBRegister(instruction);
	if(apu != data)
		programCounter_+=relAddress;

	if(apu < data)
		internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY);
	else
		internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY);
}

void I8031::djnz_RNx_REL(unsigned char const instruction, char const relAddress)
{
	unsigned int apu=readDataFromRBRegisterDirectly(instruction);
	apu--;
	writeDataToRBRegisterDirectly(instruction, apu);
	if(apu)
		programCounter_+=relAddress;
}

void I8031::djnz_DIRECT_REL(unsigned int const directAddress, char const relAddress)
{
	using namespace I8031_NameS;
	//Toimiikohen SBUFFAUS tässä näin???
	//pitää tarkistaa onko direct src osoite SBUF, jos on haetaan tieto sbufReceive:stä SBUF:iin
	if(directAddress==SBUF)
		internalDataMemory_[directAddress]=static_cast<unsigned char>(uart_->readReceiveSbuf());

	unsigned int apu=internalDataMemory_[directAddress].readValue();
	apu--;
	internalDataMemory_[directAddress]=static_cast<unsigned char>(apu);
	//pitää tarkistaa onko direct osoite SBUF, jos on kirjotetaan tieto sbufTransmittiin
	//Käynnistää lähetyksen, koska kirjotus tapahtuu SBUF rekkariin, vai käynnistääkö???
	if(directAddress==SBUF)
		uart_->startTransmit(internalDataMemory_[directAddress].readValue(), clockTicksCounted_);

	if(apu)
		programCounter_+=relAddress; //Tästä etumerkillinen
}

//_____________________________________________________________________________
//Käskyjen käsittely END.
//_____________________________________________________________________________

//Käskyn nouto ohjelmamuistista, tulkinta ja toiminta
//Ohjelmaan on lisätty myös disassemblointioptio:
//Ei suorita käskyjä disassemble moodissa ellei run ole päällä
//Uudellenmääritettävä jotta voidaan lisät periviin luokkiin juttuja.
//Kutsuttava perivässä luokass jotta steppaa.
void I8031::step(bool const disassemble, bool const run)
{
	doStep(disassemble, run);
	//Resetoidaan koko systeemi jos resetti asettunut.
	resetState=false; //Tutkii resetin resettipinnillä.
	if(pinList_.readValue(pinRESET_Index_, true)&&run)
	{
		reset();
		resetState=true;
	}
}

//Laskee seuraavana olevan käskyn koejaksot.
//Voi käyttää synkronointiin muiden processorien kanssa.
I8031_NameS::machineCycles_Type I8031::calculateNextStepMachineCycles(void)
{
	using namespace I8031_NameS;
	machineCycles_Type retVal=instructions_->readMachineCycle(readInstructionDoCycle(READ_ONLY_INSTRUCTION));
	//programCounter_--;
	return retVal;
}

unsigned int I8031::doStep(bool const disassemble, bool const run)
{
	using namespace I8031_NameS;
	unsigned int apu, bapu;
	codeBufferStr_="";
	disassembleIsOn_=disassemble;
	externalCycleList_->clear_Vecs(); //Nollataan cycle lista

	lastExecutionProgramCounterValue_=programCounter_;
	lastExecutionMachineCycleCounterValue_=machineCycleCounter_;
	oneStepMachineCycles_=calculateNextStepMachineCycles();
	oneStepAlePulses_=0;

	movx16bAWriteCycle_=false;
	movx8bAWriteCycle_=false;
	interruptDone_=false; //Reti käskyn jälkeen päällä yhden stepin
	unsigned char instruction=readInstructionDoCycle(DO_CYCLE_ALSO);
	if(disassemble)
	{
		codeBufferStr_+=hexToString(programCounter_-1,4);
		codeBufferStr_+=':';
		codeBufferStr_+=hexToString(instruction,2);
	}
	switch(instruction)
	{
	//Arithmetic operations
		case ADD_A_RN0:
		case ADD_A_RN1:
		case ADD_A_RN2:
		case ADD_A_RN3:
		case ADD_A_RN4:
		case ADD_A_RN5:
		case ADD_A_RN6:
		case ADD_A_RN7:
			if(disassemble)
			{
				codeBufferStr_+="     add A,R";
				codeBufferStr_+=hexToAsc(instruction&0x07);
				codeBufferStr_+=" ";
			}
			if(run)
				add_A_RNx(instruction);
			break;
		case ADD_A_DIRECT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   add A,";
				if(apu>=MIN_SFR_ADDRESS)
				{
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				}
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H ";
				}
				//Tarkistetaan onko kyseinen rekkari edes olemassa kyseiselle proskulle.
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				add_A_DIRECT(apu);
			break;
		case ADD_A_RI0:
		case ADD_A_RI1:
			if(disassemble)
			{
				codeBufferStr_+="     add A,@R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=" ";
			}
			if(run)
				add_A_RIx(instruction);
			break;
		case ADD_A_DATA:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct data muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   add A,#";
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				add_A_DATA(apu);
			break;
		case ADDC_A_RN0:
		case ADDC_A_RN1:
		case ADDC_A_RN2:
		case ADDC_A_RN3:
		case ADDC_A_RN4:
		case ADDC_A_RN5:
		case ADDC_A_RN6:
		case ADDC_A_RN7:
			if(disassemble)
			{
				codeBufferStr_+="     addc A,R";
				codeBufferStr_+=hexToAsc(instruction&0x07);
				codeBufferStr_+=" ";
			}
			if(run)
				addc_A_RNx(instruction);
			break;
		case ADDC_A_DIRECT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   addc A,";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=" ";
				//Virhe jos rekkaria ei löydy
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				addc_A_DIRECT(apu);
			break;
		case ADDC_A_RI0:
		case ADDC_A_RI1:
			if(disassemble)
			{
				codeBufferStr_+="     addc A,@R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=" ";
			}
			if(run)
				addc_A_RIx(instruction);
			break;
		case ADDC_A_DATA:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct data muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   addc A,#";
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				addc_A_DATA(apu);
			break;
		case SUBB_A_RN0:
		case SUBB_A_RN1:
		case SUBB_A_RN2:
		case SUBB_A_RN3:
		case SUBB_A_RN4:
		case SUBB_A_RN5:
		case SUBB_A_RN6:
		case SUBB_A_RN7:
			if(disassemble)
			{
				codeBufferStr_+="     subb A,R";
				codeBufferStr_+=hexToAsc(instruction&0x07);
				codeBufferStr_+=" ";
			}
			if(run)
				subb_A_RNx(instruction);
			break;
		case SUBB_A_DIRECT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   subb A,";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=" ";
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				subb_A_DIRECT(apu);
			break;
		case SUBB_A_RI0:
		case SUBB_A_RI1:
			if(disassemble)
			{
				codeBufferStr_+="     subb A,@R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=" ";
			}
			if(run)
				subb_A_RIx(instruction);
			break;
		case SUBB_A_DATA:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct data muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   subb A,#";
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				subb_A_DATA(apu);
			break;
		case INC_A:
			if(disassemble)
			{
				codeBufferStr_+="     inc A";
			}
			if(run)
				inc_A();
			break;
		case INC_RN0:
		case INC_RN1:
		case INC_RN2:
		case INC_RN3:
		case INC_RN4:
		case INC_RN5:
		case INC_RN6:
		case INC_RN7:
			if(disassemble)
			{
				codeBufferStr_+="     inc R";
				codeBufferStr_+=hexToAsc(instruction&0x7);
				codeBufferStr_+=" ";
			}
			if(run)
				inc_RNx(instruction);
			break;
		case INC_DIRECT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   inc ";
				if(apu>=MIN_SFR_ADDRESS)
				{
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				}
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=" ";
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				inc_DIRECT(apu);
			break;
		case INC_RI0:
		case INC_RI1:
			if(disassemble)
			{
				codeBufferStr_+="     inc @R";
				codeBufferStr_+=hexToAsc(instruction&0x1);
				codeBufferStr_+=" ";
			}
			if(run)
				inc_RIx(instruction);
			break;
		case INC_DPTR:
			if(disassemble)
				codeBufferStr_+="     inc DPTR ";
			if(run)
				inc_DPTR();
			break;
		case DEC_A:
			if(disassemble)
				codeBufferStr_+="     dec A ";
			if(run)
				dec_A();
			break;
		case DEC_RN0:
		case DEC_RN1:
		case DEC_RN2:
		case DEC_RN3:
		case DEC_RN4:
		case DEC_RN5:
		case DEC_RN6:
		case DEC_RN7:
			if(disassemble)
			{
				codeBufferStr_+="     dec R";
				codeBufferStr_+=hexToAsc(instruction&0x07);
				codeBufferStr_+=" ";
			}
			if(run)
				dec_RNx(instruction);
			break;
		case DEC_DIRECT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   dec ";
				if(apu>=MIN_SFR_ADDRESS)
				{
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				}
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=" ";
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				dec_DIRECT(apu);
			break;
		case DEC_RI0:
		case DEC_RI1:
			if(disassemble)
			{
				codeBufferStr_+="     dec @R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=" ";
			}
			if(run)
				dec_RIx(instruction);
			break;
		case MUL_AB:
			if(disassemble)
				codeBufferStr_+="     mul AB ";
			if(run)
				mul_AB();
			break;
		case DIV_AB:
			if(disassemble)
				codeBufferStr_+="     div AB ";
			if(run)
				div_AB();
			break;
		case DA_A:
			if(disassemble)
				codeBufferStr_+="     da A ";
			if(run)
				da_A();
			break;
	//Logical operations
		case ANL_A_RN0:
		case ANL_A_RN1:
		case ANL_A_RN2:
		case ANL_A_RN3:
		case ANL_A_RN4:
		case ANL_A_RN5:
		case ANL_A_RN6:
		case ANL_A_RN7:
			if(disassemble)
			{
				codeBufferStr_+="     anl A,R";
				codeBufferStr_+=hexToAsc(instruction&0x07);
				codeBufferStr_+=" ";
			}
			if(run)
				anl_A_RNx(instruction);
			break;
		case ANL_A_DIRECT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   anl A,";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=" ";
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				anl_A_DIRECT(apu);
			break;
		case ANL_A_RI0:
		case ANL_A_RI1:
			if(disassemble)
			{
				codeBufferStr_+="     anl A,@R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=" ";
			}
			if(run)
				anl_A_RIx(instruction);
			break;
		case ANL_A_DATA:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct data muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   anl A,#";
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				anl_A_DATA(apu);
			break;
		case ANL_DIRECT_A:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   anl ";
				if(apu>=MIN_SFR_ADDRESS)
				{
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				}
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=",A ";
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				anl_DIRECT_A(apu);
			break;
		case ANL_DIRECT_DATA:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct data muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+=" anl ";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=",#";
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				anl_DIRECT_DATA(apu, bapu);
			break;
		case ORL_A_RN0:
		case ORL_A_RN1:
		case ORL_A_RN2:
		case ORL_A_RN3:
		case ORL_A_RN4:
		case ORL_A_RN5:
		case ORL_A_RN6:
		case ORL_A_RN7:
			if(disassemble)
			{
				codeBufferStr_+="     orl A,R";
				codeBufferStr_+=hexToAsc(instruction&0x07);
				codeBufferStr_+=" ";
			}
			if(run)
				orl_A_RNx(instruction);
			break;
		case ORL_A_DIRECT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   orl A,";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=" ";
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				orl_A_DIRECT(apu);
			break;
		case ORL_A_RI0:
		case ORL_A_RI1:
			if(disassemble)
			{
				codeBufferStr_+="     orl A,@R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=" ";
			}
			if(run)
				orl_A_RIx(instruction);
			break;
		case ORL_A_DATA:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct data muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   orl A,#";
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				orl_A_DATA(apu);
			break;
		case ORL_DIRECT_A:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   orl ";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+",A ";
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				orl_DIRECT_A(apu);
			break;
		case ORL_DIRECT_DATA:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct data muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+=" orl ";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=",#";
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				orl_DIRECT_DATA(apu, bapu);
			break;
		case XRL_A_RN0:
		case XRL_A_RN1:
		case XRL_A_RN2:
		case XRL_A_RN3:
		case XRL_A_RN4:
		case XRL_A_RN5:
		case XRL_A_RN6:
		case XRL_A_RN7:
			if(disassemble)
			{
				codeBufferStr_+="     xrl A,R";
				codeBufferStr_+=hexToAsc(instruction&0x07);
				codeBufferStr_+=" ";
			}
			if(run)
				xrl_A_RNx(instruction);
			break;
		case XRL_A_DIRECT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   xrl A,";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=" ";
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				xrl_A_DIRECT(apu);
			break;
		case XRL_A_RI0:
		case XRL_A_RI1:
			if(disassemble)
			{
				codeBufferStr_+="     xrl A,@R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=" ";
			}
			if(run)
				xrl_A_RIx(instruction);
			break;
		case XRL_A_DATA:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct data muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=" xrl A,#";
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				xrl_A_DATA(apu);
			break;
		case XRL_DIRECT_A:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   xrl ";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=",A ";
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				xrl_DIRECT_A(apu);
			break;
		case XRL_DIRECT_DATA:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct data muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+=" xrl ";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=",#";
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				xrl_DIRECT_DATA(apu, bapu);
			break;
		case CLR_A:
			if(disassemble)
				codeBufferStr_+="     clr A ";
			if(run)
				clr_A();
			break;
		case CPL_A:
			if(disassemble)
				codeBufferStr_+="     cpl A ";
			if(run)
				cpl_A();
			break;
		case RL_A:
			if(disassemble)
				codeBufferStr_+="     rl A ";
			if(run)
				rl_A();
			break;
		case RLC_A:
			if(disassemble)
				codeBufferStr_+="     rlc A ";
			if(run)
				rlc_A();
			break;
		case RR_A:
			if(disassemble)
				codeBufferStr_+="     rr A ";
			if(run)
				rr_A();
			break;
		case RRC_A:
			if(disassemble)
				codeBufferStr_+="     rrc A ";
			if(run)
				rrc_A();
			break;
		case SWAP_A:
			if(disassemble)
				codeBufferStr_+="     swap A ";
			if(run)
				swap_A();
			break;
	//Data transfer
		case MOV_A_RN0:
		case MOV_A_RN1:
		case MOV_A_RN2:
		case MOV_A_RN3:
		case MOV_A_RN4:
		case MOV_A_RN5:
		case MOV_A_RN6:
		case MOV_A_RN7:
			if(disassemble)
			{
				codeBufferStr_+="     mov A,R";
				codeBufferStr_+=hexToAsc(instruction&0x07);
				codeBufferStr_+=" ";
			}
			if(run)
				mov_A_RNx(instruction);
			break;
		case MOV_A_DIRECT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   mov A,";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H ";
				}
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				mov_A_DIRECT(apu);
			break;
		case MOV_A_RI0:
		case MOV_A_RI1:
			if(disassemble)
			{
				codeBufferStr_+="     mov A,@R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=" ";
			}
			if(run)
				mov_A_RIx(instruction);
			break;
		case MOV_A_DATA:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct data muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   mov A,#";
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				mov_A_DATA(apu);
			break;
		case MOV_RN0_A:
		case MOV_RN1_A:
		case MOV_RN2_A:
		case MOV_RN3_A:
		case MOV_RN4_A:
		case MOV_RN5_A:
		case MOV_RN6_A:
		case MOV_RN7_A:
			if(disassemble)
			{
				codeBufferStr_+="     mov R";
				codeBufferStr_+=hexToAsc(instruction&0x07);
				codeBufferStr_+=",A ";
			}
			if(run)
				mov_RNx_A(instruction);
			break;
		case MOV_RN0_DIRECT:
		case MOV_RN1_DIRECT:
		case MOV_RN2_DIRECT:
		case MOV_RN3_DIRECT:
		case MOV_RN4_DIRECT:
		case MOV_RN5_DIRECT:
		case MOV_RN6_DIRECT:
		case MOV_RN7_DIRECT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   mov R";
				codeBufferStr_+=hexToAsc(instruction&0x07);
				codeBufferStr_+=",";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H ";
				}
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				mov_RNx_DIRECT(instruction, apu);
			break;
		case MOV_RN0_DATA:
		case MOV_RN1_DATA:
		case MOV_RN2_DATA:
		case MOV_RN3_DATA:
		case MOV_RN4_DATA:
		case MOV_RN5_DATA:
		case MOV_RN6_DATA:
		case MOV_RN7_DATA:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct data muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   mov R";
				codeBufferStr_+=hexToAsc(instruction&0x07);
				codeBufferStr_+=",#";
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				mov_RNx_DATA(instruction, apu);
			break;
		case MOV_DIRECT_A:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   mov ";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=",A ";
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				mov_DIRECT_A(apu);
			break;
		case MOV_DIRECT_RN0:
		case MOV_DIRECT_RN1:
		case MOV_DIRECT_RN2:
		case MOV_DIRECT_RN3:
		case MOV_DIRECT_RN4:
		case MOV_DIRECT_RN5:
		case MOV_DIRECT_RN6:
		case MOV_DIRECT_RN7:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   mov ";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=",R";
				codeBufferStr_+=hexToAsc(instruction&0x07);
				codeBufferStr_+=" ";
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				mov_DIRECT_RNx(instruction, apu);
			break;
		case MOV_DIRECT_DIRECT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan src direct osoite muistista
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan dest direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+=" mov ";
				if(bapu>=MIN_SFR_ADDRESS)            //Ensiksi dest koska muistissa ne on toisinpäin
					codeBufferStr_+=sfrNameTable_[bapu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(bapu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=",";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H ";
				}
				isSFROn(static_cast<unsigned char>(apu));
				isSFROn(static_cast<unsigned char>(bapu));
			}
			if(run)
				mov_DIRECT_DIRECT(apu, bapu);
			break;
		case MOV_DIRECT_RI0:
		case MOV_DIRECT_RI1:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   mov ";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=",@R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=" ";
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				mov_DIRECT_RIx(instruction, apu);
			break;
		case MOV_DIRECT_DATA:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan src direct osoite muistista
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan dest direct data muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+=" mov ";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=",#";
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				mov_DIRECT_DATA(apu, bapu);
			break;
		case MOV_RI0_A:
		case MOV_RI1_A:
			if(disassemble)
			{
				codeBufferStr_+="     mov @R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=",A ";
			}
			if(run)
				mov_RIx_A(instruction);
			break;
		case MOV_RI0_DIRECT:
		case MOV_RI1_DIRECT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   mov @R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=",";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H ";
				}
				//Tarkistetaan onko kyseinen rekkari edes olemassa kyseiselle proskulle.
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				mov_RIx_DIRECT(instruction, apu);
			break;
		case MOV_RI0_DATA:
		case MOV_RI1_DATA:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct data muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   mov @R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=",#";
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				mov_RIx_DATA(instruction, apu);
			break;
		case MOV_DPTR_DATA16:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+=" mov DPTR,#";
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				mov_DPTR_DATA16(apu, bapu);
			break;
		case MOVC_A_A_DPTR:
			if(disassemble)
				codeBufferStr_+="     mov A,@A+DPTR ";
			if(run)
				movc_A_A_DPTR();
			break;
		case MOVC_A_A_PC:
			if(disassemble)
				codeBufferStr_+="     mov A,@A+PC ";
			if(run)
				movc_A_A_PC();
			break;
		case MOVX_A_RI0:
		case MOVX_A_RI1:
			if(disassemble)
			{
				codeBufferStr_+="     movx A,@R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=" ";
			}
			if(run)
				movx_A_RIx(instruction);
			break;
		case MOVX_RI0_A:
		case MOVX_RI1_A:
			if(disassemble)
			{
				codeBufferStr_+="     movx @R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=",A ";
			}
			if(run)
				movx_RIx_A(instruction);
			break;
		case MOVX_A_DPTR:
			if(disassemble)
				codeBufferStr_+="     movx A,@DPTR ";
			if(run)
				movx_A_DPTR();
			break;
		case MOVX_DPTR_A:
			if(disassemble)
				codeBufferStr_+="     movx @DPTR,A ";
			if(run)
				movx_DPTR_A();
			break;
		case POP_DIRECT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   pop ";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H ";
				}
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				pop_DIRECT(apu);
			break;
		case PUSH_DIRECT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   push ";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H ";
				}
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				push_DIRECT(apu);
			break;
		case XCH_A_RN0:
		case XCH_A_RN1:
		case XCH_A_RN2:
		case XCH_A_RN3:
		case XCH_A_RN4:
		case XCH_A_RN5:
		case XCH_A_RN6:
		case XCH_A_RN7:
			if(disassemble)
			{
				codeBufferStr_+="     xch A,R";
				codeBufferStr_+=hexToAsc(instruction&0x07);
				codeBufferStr_+=" ";
			}
			if(run)
				xch_A_RNx(instruction);
			break;
		case XCH_A_DIRECT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   xch A,";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H ";
				}
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				xch_A_DIRECT(apu);
			break;
		case XCH_A_RI0:
		case XCH_A_RI1:
			if(disassemble)
			{
				codeBufferStr_+="     xch A,@R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=" ";
			}
			if(run)
				xch_A_RIx(instruction);
			break;
		case XCHD_A_RI0:
		case XCHD_A_RI1:
			if(disassemble)
			{
				codeBufferStr_+="     xchd A,@R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=" ";
			}
			if(run)
				xchd_A_RIx(instruction);
			break;
	//Boolean variable manipulation
		case CLR_C:
			if(disassemble)
				codeBufferStr_+="     clr C";
			if(run)
				clr_C();
			break;
		case CLR_BIT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan bit osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   clr ";
				codeBufferStr_+=readBitAddressableBitName(static_cast<unsigned char>(apu));
				codeBufferStr_+=" ";
				//Onko SFR alueen bitti olemassa ko. proskulle
				isSFRBitOn(static_cast<unsigned char>(apu));
			}
			if(run)
				clr_BIT(static_cast<unsigned char>(apu));
			break;
		case SETB_C:
			if(disassemble)
				codeBufferStr_+="     setb C ";
			if(run)
				setb_C();
			break;
		case SETB_BIT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan bit osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   setb ";
				codeBufferStr_+=readBitAddressableBitName(static_cast<unsigned char>(apu));
				codeBufferStr_+=" ";
				//Onko SFR alueen bitti olemassa ko. proskulle
				isSFRBitOn(static_cast<unsigned char>(apu));
			}
			if(run)
				setb_BIT(static_cast<unsigned char>(apu));
			break;
		case CPL_C:
			if(disassemble)
				codeBufferStr_+="     cpl C ";
			if(run)
				cpl_C();
			break;
		case CPL_BIT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan bit osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   cpl ";
				codeBufferStr_+=readBitAddressableBitName(static_cast<unsigned char>(apu));
				codeBufferStr_+=" ";
			}
			if(run)
				cpl_BIT(static_cast<unsigned char>(apu));
			break;
		case ANL_C_BIT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan bit osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   anl C,";
				codeBufferStr_+=readBitAddressableBitName(static_cast<unsigned char>(apu));
				codeBufferStr_+=" ";
			}
			if(run)
				anl_C_BIT(static_cast<unsigned char>(apu));
			break;
		case ANL_C_NBIT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan bit osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   anl C,/";
				codeBufferStr_+=readBitAddressableBitName(static_cast<unsigned char>(apu));
				codeBufferStr_+=" ";
			}
			if(run)
				anl_C_NBIT(static_cast<unsigned char>(apu));
			break;
		case ORL_C_BIT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan bit osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   orl C,";
				codeBufferStr_+=readBitAddressableBitName(static_cast<unsigned char>(apu));
				codeBufferStr_+=" ";
			}
			if(run)
				orl_C_BIT(static_cast<unsigned char>(apu));
			break;
		case ORL_C_NBIT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan bit osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   orl C,/";
				codeBufferStr_+=readBitAddressableBitName(static_cast<unsigned char>(apu));
				codeBufferStr_+=" ";
			}
			if(run)
				orl_C_NBIT(static_cast<unsigned char>(apu));
			break;
		case MOV_C_BIT:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan bit osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   mov C,";
				codeBufferStr_+=readBitAddressableBitName(static_cast<unsigned char>(apu));
				codeBufferStr_+=" ";
			}
			if(run)
				mov_C_BIT(static_cast<unsigned char>(apu));
			break;
		case MOV_BIT_C:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan bit osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   mov ";
				codeBufferStr_+=readBitAddressableBitName(static_cast<unsigned char>(apu));
				codeBufferStr_+=",C ";
			}
			if(run)
				mov_BIT_C(static_cast<unsigned char>(apu));
			break;
		case JC_REL:
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan rel osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="   jc ";
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				jc_REL(static_cast<char>(bapu));
			break;
		case JNC_REL:
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan rel osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="   jnc ";
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				jnc_REL(static_cast<char>(bapu));
			break;
		case JB_BIT_REL:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan bit osoite muistista
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan rel osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+=" jb ";
				codeBufferStr_+=readBitAddressableBitName(static_cast<unsigned char>(apu));
				codeBufferStr_+=",";
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
				//Onko SFR alueen bitti olemassa ko. proskulle
				isSFRBitOn(static_cast<unsigned char>(apu));
			}
			if(run)
				jb_BIT_REL(static_cast<unsigned char>(apu), static_cast<char>(bapu));
			break;
		case JNB_BIT_REL:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan bit osoite muistista
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan rel osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+=" jnb ";
				codeBufferStr_+=readBitAddressableBitName(static_cast<unsigned char>(apu));
				codeBufferStr_+=",";
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				jnb_BIT_REL(static_cast<unsigned char>(apu), static_cast<char>(bapu));
			break;
		case JBC_BIT_REL:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan bit osoite muistista
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan rel osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+=" jbc ";
				codeBufferStr_+=readBitAddressableBitName(static_cast<unsigned char>(apu));
				codeBufferStr_+=",";
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				jbc_BIT_REL(static_cast<unsigned char>(apu), static_cast<char>(bapu));
			break;
	//Program branching
		case ACALL_100:
		case ACALL_200:
		case ACALL_300:
		case ACALL_400:
		case ACALL_500:
		case ACALL_600:
		case ACALL_700:
		case ACALL_800:
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan hyppyosoitteen alaosa
			if(disassemble)
			{
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="   acall ";
				codeBufferStr_+=hexToAsc((instruction>>5)&0x07);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				acall_x00(instruction, bapu);
			break;
		case LCALL:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan hyppyosoitteen yläosa
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan hyppyosoitteen alaosa
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+=" lcall ";
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				lcall(static_cast<unsigned char>(apu), static_cast<unsigned char>(bapu));
			break;
		case RET:
			if(disassemble)
				codeBufferStr_+="     ret ";
			if(run)
				ret();
			break;
		case RETI:
			if(disassemble)
				codeBufferStr_+="     reti ";
			if(run)
			{
				reti();
				interruptDone_=true;
			}
			break;
		case AJMP_100:
		case AJMP_200:
		case AJMP_300:
		case AJMP_400:
		case AJMP_500:
		case AJMP_600:
		case AJMP_700:
		case AJMP_800:
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan hyppyosoitteen alaosa
			if(disassemble)
			{
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="   ajmp ";
				codeBufferStr_+=hexToAsc((instruction>>5)&0x07);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				ajmp_x00(instruction, bapu);
			break;
		case LJMP:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan hyppyosoitteen yläosa
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan hyppyosoitteen alaosa
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+=" ljmp ";
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				ljmp(static_cast<unsigned char>(apu), static_cast<unsigned char>(bapu));
			break;
		case SJMP:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan rel osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="   sjmp ";
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				sjmp(static_cast<char>(apu));
			break;
		case JMP_A_DPTR:
			if(disassemble)
				codeBufferStr_+="     jmp @A+DPTR ";
			if(run)
				jmp_A_DPTR();
			break;
		case JZ_REL:
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan rel osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="   jz ";
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				jz_REL(static_cast<char>(bapu));
			break;
		case JNZ_REL:
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan rel osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="   jnz ";
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				jnz_REL(static_cast<char>(bapu));
			break;
		case CJNE_A_DIRECT_REL:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan rel osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+=" cjne A,";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu,2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=",";
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
				//Tarkistetaan onko kyseinen rekkari edes olemassa kyseiselle proskulle.
				isSFROn(static_cast<unsigned char>(apu));
			}
			if(run)
				cjne_A_DIRECT_REL(apu, static_cast<unsigned char>(bapu));
			break;
		case CJNE_A_DATA_REL:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan data muistista
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan rel osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+=" cjne A,#";
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="H,";
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				cjne_A_DATA_REL(static_cast<unsigned char>(apu), static_cast<char>(bapu));
			break;
		case CJNE_RN0_DATA_REL:
		case CJNE_RN1_DATA_REL:
		case CJNE_RN2_DATA_REL:
		case CJNE_RN3_DATA_REL:
		case CJNE_RN4_DATA_REL:
		case CJNE_RN5_DATA_REL:
		case CJNE_RN6_DATA_REL:
		case CJNE_RN7_DATA_REL:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan data muistista
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan rel osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+=" cjne R";
				codeBufferStr_+=hexToAsc(instruction&0x07);
				codeBufferStr_+=",#";
				codeBufferStr_+=hexToString(apu, 2);
				codeBufferStr_+="H,";
				codeBufferStr_+=hexToString(bapu, 2);
				codeBufferStr_+="H ";
			}
			if(run)
				cjne_RNx_DATA_REL(static_cast<unsigned char>(instruction), static_cast<unsigned char>(apu), static_cast<char>(bapu));
			break;
		case CJNE_RI0_DATA_REL:
		case CJNE_RI1_DATA_REL:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan data muistista
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan rel osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu, 2);
				codeBufferStr_+=hexToString(bapu, 2);
				codeBufferStr_+=" cjne @R";
				codeBufferStr_+=hexToAsc(instruction&0x01);
				codeBufferStr_+=",#";
				codeBufferStr_+=hexToString(apu,2);
				codeBufferStr_+="H,";
				codeBufferStr_+=hexToString(bapu, 2);
				codeBufferStr_+="H ";
			}
			if(run)
				cjne_RIx_DATA_REL(static_cast<unsigned char>(instruction), static_cast<unsigned char>(apu), static_cast<char>(bapu));
			break;
		case DJNZ_RN0_REL:
		case DJNZ_RN1_REL:
		case DJNZ_RN2_REL:
		case DJNZ_RN3_REL:
		case DJNZ_RN4_REL:
		case DJNZ_RN5_REL:
		case DJNZ_RN6_REL:
		case DJNZ_RN7_REL:
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan rel osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(bapu, 2);
				codeBufferStr_+="   djnz R";
				codeBufferStr_+=hexToAsc(instruction&0x07);
				codeBufferStr_+=",";
				codeBufferStr_+=hexToString(bapu, 2);
				codeBufferStr_+="H ";
			}
			if(run)
				djnz_RNx_REL(instruction, static_cast<char>(bapu));
			break;
		case DJNZ_DIRECT_REL:
			apu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan direct osoite muistista
			bapu=readInstructionDoCycle(DO_CYCLE_ALSO);//haetaan rel osoite muistista
			if(disassemble)
			{
				codeBufferStr_+=hexToString(apu, 2);
				codeBufferStr_+=hexToString(bapu, 2);
				codeBufferStr_+=" djnz ";
				if(apu>=MIN_SFR_ADDRESS)
					codeBufferStr_+=sfrNameTable_[apu-MIN_SFR_ADDRESS];
				else
				{
					codeBufferStr_+=hexToString(apu, 2);
					codeBufferStr_+="H";
				}
				codeBufferStr_+=",";
				codeBufferStr_+=hexToString(bapu,2);
				codeBufferStr_+="H ";
			}
			if(run)
				djnz_DIRECT_REL(apu, static_cast<char>(bapu));
			break;
	//NOP
		case NOP:
			if(disassemble)
				codeBufferStr_+="     nop ";
			break;
		default:
			addErrorToCodeBufferStr(":BAD CODE!");
	}

	if(run)
	{
		machineCycleCounter_+=oneStepMachineCycles_;
		externalCycleAddsEndOfStep();

		setParityFlag();
		
		lookIfExternalInterrupts();
		if(!interruptDone_)      //Jos juuri palattu keskytyksestä. Ei käsitellä keskytyksiä(SINGLE STEP)
			handlingInterrupts();
		
		clockTicksCalculated_=machineCycleCounter_*clockCyclesPerMachineCycle_;
	}

	if(programCounter_-1>programSize_)
		codeBufferStr_+=":PC Over Program Size!";

	if(programCounter_>MAX_EXTERNAL_PROGRAM_MEMORY_ADDRESS)
	{
		codeBufferStr_=":END OF PROGRAM MEMORY!";
		programCounter_=0;
	}

	return oneStepMachineCycles_; //Palautusarvona käytetyt machinecyclet
}

void I8031::addClockTicksAndHandlePeripherals(I8031_NameS::alePulses_Type ales)
{
	for(int n=0; n<ales; n++)
	{
		clockTicksCounted_+=clockCyclesClockTicksCounted_;
		timersAndCountersHandling();
		serialPortRxTxHandling();
		handlePeripherals_VirtualPart();
	}
}

void I8031::externalCycleAddsEndOfStep(void)
{
	externalCycleList_->correctCycle(this, oneStepMachineCycles_, oneStepAlePulses_); //Korjaa external cycleä.
	externalCycleList_->addSerialPortMode0Transmit(uart_, oneStepMachineCycles_);
}

string I8031::readSFRName(unsigned char const sfrAddress)
{
	using namespace I8031_NameS;
	//Jos on käytössä oleva SFR osoite
	if((internalDataMemory_[sfrAddress].isInUse()) && (internalDataMemory_[sfrAddress].isSFR()) && (sfrAddress>=MIN_SFR_ADDRESS))
		return sfrNameTable_[sfrAddress-MIN_SFR_ADDRESS];
	else
		return string(":Not a valid SFR Register!");
}

string I8031::readSFRBitName(unsigned char const sfrAddress, unsigned char const bitNum)
{
	using namespace I8031_NameS;
	//Jos on käytössä oleva SFR bitti
	if((internalDataMemory_[sfrAddress].isInUse()) && (internalDataMemory_[sfrAddress].isBitNames()) && (sfrAddress>=MIN_SFR_ADDRESS) && (bitNum<8))
		return sfrBitNameTable_[(sfrAddress-MIN_SFR_ADDRESS)*8+bitNum];
	else
		return string(":Not a valid SFR Bit!");
}

//Luetaan SFR:n osoite nimellä
int I8031::readSFRAddress(string const name)
{
	using namespace I8031_NameS;
	int rv=-1;
	for(unsigned int n=MIN_SFR_ADDRESS;n<=MAX_SFR_ADDRESS;n++)
	{
		if(sfrNameTable_[n-MIN_SFR_ADDRESS]==name)
		{
			rv=n;
			break;
		}
	}
	return rv;
}

void I8031::setParityFlag(void)
{
	using namespace I8031_NameS;
	char n,m=0;
	for(n=0;n<8;n++)
	{
		if((internalDataMemory_[ACC].readValue()>>n)&BIT_0)
			m++;
	}

	if(m%2)
		internalDataMemory_[PSW]&=~PSW_P;      //Jos jakojäännöstä pariton määrä ykkösiä
	else
		internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_P);
}

//Asetetaan C,OV ja AC liput ADD operaatioissa
void I8031::addSetFlags(unsigned int a, unsigned int b, bool on_c)
{
	using namespace I8031_NameS;
	if((a+b)>0xFF)
	{
		internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY);
		if((a&0x7F)+(b&0x7F) > 0x7F)
			internalDataMemory_[PSW]&=~PSW_OV;
		else
			internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_OV);
	}
	else
	{
		internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY);
		if((a&0x7F)+(b&0x7F) > 0x7F)
			internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_OV);
		else
			internalDataMemory_[PSW]&=~PSW_OV;
	}
	if((a&0x0F)+(b&0x0F) > 0x0F)
		internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_AC);
	else
		internalDataMemory_[PSW]&=~PSW_AC;

	//Jos Carry mukana laskennassaa, käsky ADDC ja se päällä, muutoin edelliset jää voimaan.
	if(((on_c)&&(internalDataMemory_[PSW].readValue()&!PSW_CY))==PSW_CY)
	{
		if((a+b+1)>0xFF)
		{
			internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY);
			if((a&0x7F)+(b&0x7F)+1 > 0x7F)
				internalDataMemory_[PSW]&=~PSW_OV;
			else
				internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_OV);
		}
		else
		{
			internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY);
			if((a&0x7F)+(b&0x7F)+1 > 0x7F)
				internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_OV);
			else
				internalDataMemory_[PSW]&=~PSW_OV;
		}
		if((a&0x0F)+(b&0x0F)+1 > 0x0F)
			internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_AC);
		else
			internalDataMemory_[PSW]&=~PSW_AC;
	}
}

void I8031::subbSetFlags(unsigned int a, unsigned int b)
{
	using namespace I8031_NameS;
	char apu;
	if(internalDataMemory_[PSW].readValue()&PSW_CY)      //CArryn arvo mukaan laskentaan
		apu=1;
	else
		apu=0;
	if((a-apu-b)>0xFF)
	{
		internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY);
		if((a&0x7F)-(b&0x7F) -apu > 0x7F)
			internalDataMemory_[PSW]&=~PSW_OV;
		else
			internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_OV);
	}
	else
	{
		internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY);
		if((a&0x7F)-(b&0x7F)-apu > 0x7F)
			internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_OV);
		else
			internalDataMemory_[PSW]&=~PSW_OV;
	}
	if((a&0x0F)+(b&0x0F)-apu > 0x0F)
		internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_AC);
	else
		internalDataMemory_[PSW]&=~PSW_AC;
}

unsigned int I8031::readDataFromRBRegisterDirectly(unsigned int r)
{
	using namespace I8031_NameS;
	//Testataan mikä pankki käytössä ja haetaan data sen rekkarista
	int bankAddress=8*(((internalDataMemory_[PSW].readValue())>>3)&0x03);
	return internalDataMemory_[REGISTER_BANK0_ADDRESS+bankAddress+(r&0x07)].readValue();//0x07=maski 0000 0111
}


void I8031::writeDataToRBRegisterDirectly(unsigned int const r, unsigned int const val)
{
	using namespace I8031_NameS;
	//Testataan mikä pankki käytössä ja kirjotetaan data sen rekkariin
	int bankAddress=8*((internalDataMemory_[PSW].readValue()>>3)&0x03);
	internalDataMemory_[REGISTER_BANK0_ADDRESS+bankAddress+(r&0x07)]=static_cast<unsigned char>(val);//0x07=maski 0000 0111
}

unsigned int I8031::readDataFromInternalDataMemoryIndirectlyUsingRBRegister(unsigned int const r)
{
	using namespace I8031_NameS;
	//Testataan mikä pankki käytössä ja sen mukaan haetaan data epäsuorasti R0:lla tai R1:llä
	int bankAddress=8*((internalDataMemory_[PSW].readValue()>>3)&0x03);
	//Jos osoite menee yli epäsuorasti osoitettavan alueen annetaan virheilmoitus
	if(internalDataMemory_[REGISTER_BANK0_ADDRESS+bankAddress+(r&BIT_0)].readValue() >= MIN_SFR_ADDRESS)
	{
		addErrorToCodeBufferStr(":BAD indirect address ");
		string sapu;
		sapu=hexToString(internalDataMemory_[REGISTER_BANK0_ADDRESS+bankAddress+(r&BIT_0)].readValue(),2);
		sapu+="H!";
		addErrorToCodeBufferStr(sapu);
	}
	//Sallitaan kuitenkin haku
	unsigned char internalAddress=internalDataMemory_[REGISTER_BANK0_ADDRESS+bankAddress+(r&BIT_0)].readValue();
	return internalDataMemory_[internalAddress].readValue();
}

void I8031::writeDataToInternalDataMemoryIndirectlyUsingRBRegister(unsigned int r, unsigned int val)
{
	using namespace I8031_NameS;
	//Testataan mikä pankki käytössä ja kirjotetaan data epäsuorasti sen pankin rekkarin R0 tai R1
	//osoitteella sisäiseen muistiin
	int bankAddress=8*((internalDataMemory_[PSW].readValue()>>3)&0x03);
	//Jos osoite menee yli epäsuorasti osoitettavan alueen annetaan virheilmoitus
	if(internalDataMemory_[REGISTER_BANK0_ADDRESS+bankAddress+(r&BIT_0)].readValue()>=MIN_SFR_ADDRESS)
	{
		codeBufferStr_+=":BAD indirect address ";
		codeBufferStr_+=hexToString(internalDataMemory_[REGISTER_BANK0_ADDRESS+bankAddress+(r&BIT_0)].readValue(),2);
		codeBufferStr_+="H!";
	}
	else
	{
		internalDataMemory_[internalDataMemory_[REGISTER_BANK0_ADDRESS+bankAddress+(r&BIT_0)].readValue()]=static_cast<unsigned char>(val);
	}
}

//Antaa bittiosoitteella SFR osoitteen 80H-FFH
//Jos 0
unsigned char I8031::bitAddressToSFRAddress(unsigned char const bitAddress)
{
	using namespace I8031_NameS;
	if(bitAddress>=MIN_SFR_ADDRESS)
		return bitAddress&0xF0; //Maskataan vain alaosa pois
	else
		return 0;
}

void I8031::manipulateDirectBitWithBitAddress(unsigned char bitAddress, unsigned char manipulation)
{
	using namespace I8031_NameS;
	unsigned int apu,bapu;
	apu=bitAddress/8;
	bapu=bitAddress%8;
	//bapu=(unsigned int)pow(double(2), double (bapu));
	bapu=1<<bapu;
	if(bitAddress<MIN_SFR_ADDRESS)
	{
		switch (manipulation)
		{
			case CLR_BIT:
				internalDataMemory_[MIN_BIT_AREA_ADDRESS+apu]&=static_cast<unsigned char>(~bapu);
				break;
			case SETB_BIT:
				internalDataMemory_[MIN_BIT_AREA_ADDRESS+apu]|=static_cast<unsigned char>(bapu);
				break;
			case CPL_BIT:
				internalDataMemory_[MIN_BIT_AREA_ADDRESS+apu]^=static_cast<unsigned char>(bapu);
				break;
			default:;
		}
	}
	else
	{
		if(bitAddress>=MIN_SFR_ADDRESS)//&&(bitAddress<=MAX_SFR_ADDRESS))
		{
			switch (manipulation)
			{
				case CLR_BIT:
					internalDataMemory_[apu*8]&=static_cast<unsigned char>(~bapu);
					break;
				case SETB_BIT:
					internalDataMemory_[apu*8]|=static_cast<unsigned char>(bapu);
					break;
				case CPL_BIT:
					internalDataMemory_[apu*8]^=static_cast<unsigned char>(bapu);
					break;
				default:;
			}
		}
	}
}

void I8031::manipulateCarryWithBitAddress(unsigned char const bitAddress, unsigned char const manipulation)
{
	using namespace I8031_NameS;
	unsigned int apu,bapu;
	apu=bitAddress/8;
	bapu=bitAddress%8;
	bapu=1<<bapu;
	if(bitAddress<MIN_SFR_ADDRESS)
	{
		switch(manipulation)
		{
			case ANL_C_BIT:
				if(!(internalDataMemory_[MIN_BIT_AREA_ADDRESS+apu].readValue()&bapu)) //Jos bitti = 0
					internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY);                        //nollaa carry
				break;
			case ANL_C_NBIT:
				if(!((internalDataMemory_[MIN_BIT_AREA_ADDRESS+apu].readValue()^0xFF)&bapu)) //Jos notbitti = 0
					internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY);                        //nollaa carry
				break;
			case ORL_C_BIT:
				if(internalDataMemory_[MIN_BIT_AREA_ADDRESS+apu].readValue()&bapu) //Jos bitti = 1
					internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY);                       //aseta carry
				break;
			case ORL_C_NBIT:
				if(!(internalDataMemory_[MIN_BIT_AREA_ADDRESS+apu].readValue()&bapu)) //Jos notbitti = 1
					internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY);                        //aseta carry
				break;
			case MOV_C_BIT:
				if(internalDataMemory_[MIN_BIT_AREA_ADDRESS+apu].readValue()&bapu) //Jos bitti = 1
					internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY);         //aseta carry
				else
					internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY);         //muutoin nollaa
				break;
			case MOV_BIT_C:
				if(internalDataMemory_[PSW].readValue()&PSW_CY) //Jos carry = 1
					internalDataMemory_[MIN_BIT_AREA_ADDRESS+apu]|=static_cast<unsigned char>(bapu);         //aseta bitti
				else
					internalDataMemory_[MIN_BIT_AREA_ADDRESS+apu]&=static_cast<unsigned char>(~bapu);         //muutoin nollaa
				break;
			default:;
		}
	}
	else
	{
		if(bitAddress>=MIN_SFR_ADDRESS)//&&(bitAddress<=MAX_SFR_ADDRESS))
		{
			switch (manipulation)
			{
				case ANL_C_BIT:
					if(!(internalDataMemory_[apu*8].readValue()&bapu)) //Jos bitti = 0
						internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY); //nollaa carry
					break;
				case ANL_C_NBIT:
					if(!((internalDataMemory_[apu*8].readValue()^0xFF)&bapu)) //Jos bitti = 0
						internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY); //nollaa carry
					break;
				case ORL_C_BIT:
					if(internalDataMemory_[apu*8].readValue()&bapu) //Jos bitti = 1
						internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY); //aseta carry
					break;
				case ORL_C_NBIT:
					if(!(internalDataMemory_[apu*8].readValue()&bapu)) //Jos notbitti = 1
						internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY); //aseta carry
					break;
				case MOV_C_BIT:
					if(internalDataMemory_[apu*8].readValue()&bapu) //Jos bitti = 1
						internalDataMemory_[PSW]|=static_cast<unsigned char>(PSW_CY); //aseta carry
					else
						internalDataMemory_[PSW]&=static_cast<unsigned char>(~PSW_CY); //muutoin nollaa
					break;
				case MOV_BIT_C:
					if(internalDataMemory_[PSW].readValue()&PSW_CY) //Jos carry = 1
						internalDataMemory_[apu*8]|=static_cast<unsigned char>(bapu); //aseta bitti
					else
						internalDataMemory_[apu*8]&=static_cast<unsigned char>(~bapu); //muutoin nollaa bitti
					break;
				default:;
			}
		}
	}
}

void I8031::jumpBitManipulation(unsigned char const bitAddress, char const relAddress, unsigned char const manip)
{
	using namespace I8031_NameS;
	unsigned int apu,bapu;
	apu=bitAddress/8;
	bapu=bitAddress%8;
	//bapu=(unsigned int)pow(double(2), double (bapu));
	bapu=1<<bapu;
	if(bitAddress<MIN_SFR_ADDRESS)
	{
		switch(manip)
		{
			case JB_BIT_REL:
			case JBC_BIT_REL:
				if(internalDataMemory_[MIN_BIT_AREA_ADDRESS+apu].readValue()&bapu)
					programCounter_+=relAddress;
				if(manip==JBC_BIT_REL)
					internalDataMemory_[MIN_BIT_AREA_ADDRESS+apu]&=static_cast<unsigned char>(~bapu);
				break;
			case JNB_BIT_REL:
				if(!(internalDataMemory_[MIN_BIT_AREA_ADDRESS+apu].readValue()&bapu))
					programCounter_+=relAddress;
				break;
		}
	}
	else
	{
		if(bitAddress>=MIN_SFR_ADDRESS)
		{
			switch(manip)
			{
				case JB_BIT_REL:
				case JBC_BIT_REL:
					if(internalDataMemory_[apu*8].readValue()&bapu)
						programCounter_+=relAddress;
					if(manip==JBC_BIT_REL)
						internalDataMemory_[apu*8]&=static_cast<unsigned char>(~bapu);
					break;
				case JNB_BIT_REL:
					if(!(internalDataMemory_[apu*8].readValue()&bapu))
						programCounter_+=relAddress;
					break;
			}
		}
	}
}

void I8031::isSFROn(unsigned char const regAddress)
{
	using namespace I8031_NameS;
	if(!(internalDataMemory_[regAddress].isInUse()) && (internalDataMemory_[regAddress].isSFR()) && (regAddress>=MIN_SFR_ADDRESS))
		codeBufferStr_+=":No SFR Register!";
}

void I8031::isSFRBitOn(unsigned char const bit)
{
	using namespace I8031_NameS;
	if(!(internalDataMemory_[bit&0xF8].isInUse()) && (bit>=MIN_SFR_ADDRESS))
		codeBufferStr_+=":No SFR Bit!";
}

//Asetetaan SFR rekisteri käyttöön
void I8031::setSFRInUse(unsigned char const reg, char const* name, bool const isPort)
{
	using namespace I8031_NameS;
	unsigned int regA=reg-MIN_SFR_ADDRESS;
	if(regA<SFRNAMETABLESIZE)
	{
		sfrNameTable_[regA]=name;
		internalDataMemory_[reg].setInUse(true);
		internalDataMemory_[reg].setIsPort(isPort);
	}
}

//Asetetaan SFR rekisterin bittinimi käyttöön
void I8031::setSFRBitNameInUse(unsigned char const reg, unsigned char const bit, char const* bitname)
{
	using namespace I8031_NameS;
	unsigned int bitA=(reg-MIN_SFR_ADDRESS)*8+bit;
	if(bitA<SFRBITNAMETABLESIZE)
	{
		sfrBitNameTable_[bitA]=bitname;
		internalDataMemory_[reg].setIsBitNames(true);
	}
}

void I8031::createSFRNameTable(void)
{
	using namespace I8031_NameS;
	//Tyhjätään alue ensiksi
	for(unsigned int n=MIN_SFR_ADDRESS-MIN_SFR_ADDRESS;n<=MAX_SFR_ADDRESS-MIN_SFR_ADDRESS;n++)
		sfrNameTable_[n]="";
	//Laitetaan SFR nimet Ja asetetaan päälle
	setSFRInUse(P0, "P0", true);
	setSFRInUse(SP, "SP", false);
	setSFRInUse(DPL, "DPL", false);
	setSFRInUse(DPH, "DPH", false);
	setSFRInUse(TCON, "TCON", false);
	setSFRInUse(TMOD, "TMOD", false);
	setSFRInUse(TL0, "TL0", false);
	setSFRInUse(TL1, "TL1", false);
	setSFRInUse(TH0, "TH0", false);
	setSFRInUse(TH1, "TH1", false);
	setSFRInUse(P1, "P1", true);
	setSFRInUse(SCON, "SCON", false);
	setSFRInUse(SBUF, "SBUF", false);
	setSFRInUse(P2, "P2", true);
	setSFRInUse(IE, "IE", false);
	setSFRInUse(P3, "P3", true);
	setSFRInUse(IP, "IP", false);
	setSFRInUse(PSW, "PSW", false);
	setSFRInUse(ACC, "ACC", false);
	setSFRInUse(B, "B", false);
}

void I8031::createSFRBitNameTable(void)
{
	using namespace I8031_NameS;
	unsigned int n,m;
	//Bittiosoitettava alue
	for(n=0;n<MIN_SFR_ADDRESS;n++)
	{
		bitAreaNameTable_[n]=decToString(n,1,'0');
	}
	//SFR
	//Tyhjätään ensin alue
	for(n=MIN_SFR_ADDRESS;n<=MAX_SFR_ADDRESS;n++)
	{
		for(m=0;m<8;m++)
		{
			sfrBitNameTable_[n+m-MIN_SFR_ADDRESS]="";
		}
	}
	//P0:n bitit
	setSFRBitNameInUse(P0, BIT_NUMBER_0, "P0.0");
	setSFRBitNameInUse(P0, BIT_NUMBER_1, "P0.1");
	setSFRBitNameInUse(P0, BIT_NUMBER_2, "P0.2");
	setSFRBitNameInUse(P0, BIT_NUMBER_3, "P0.3");
	setSFRBitNameInUse(P0, BIT_NUMBER_4, "P0.4");
	setSFRBitNameInUse(P0, BIT_NUMBER_5, "P0.5");
	setSFRBitNameInUse(P0, BIT_NUMBER_6, "P0.6");
	setSFRBitNameInUse(P0, BIT_NUMBER_7, "P0.7");
	//TCON:n bitit
	setSFRBitNameInUse(TCON, BIT_NUMBER_0, "IT0");
	setSFRBitNameInUse(TCON, BIT_NUMBER_1, "IE0");
	setSFRBitNameInUse(TCON, BIT_NUMBER_2, "IT1");
	setSFRBitNameInUse(TCON, BIT_NUMBER_3, "IE1");
	setSFRBitNameInUse(TCON, BIT_NUMBER_4, "TR0");
	setSFRBitNameInUse(TCON, BIT_NUMBER_5, "TF0");
	setSFRBitNameInUse(TCON, BIT_NUMBER_6, "TR1");
	setSFRBitNameInUse(TCON, BIT_NUMBER_7, "TF1");
	//TMOD:n bitit
	setSFRBitNameInUse(TMOD, BIT_NUMBER_0, "T0_M0");
	setSFRBitNameInUse(TMOD, BIT_NUMBER_1, "T0_M1");
	setSFRBitNameInUse(TMOD, BIT_NUMBER_2, "TO_C/T");
	setSFRBitNameInUse(TMOD, BIT_NUMBER_3, "T0GATE");
	setSFRBitNameInUse(TMOD, BIT_NUMBER_4, "T1_M0");
	setSFRBitNameInUse(TMOD, BIT_NUMBER_5, "T1_M1");
	setSFRBitNameInUse(TMOD, BIT_NUMBER_6, "T1_C/T");
	setSFRBitNameInUse(TMOD, BIT_NUMBER_7, "T1GATE");
	//P1:n bitit
	setSFRBitNameInUse(P1, BIT_NUMBER_0, "P1.0");
	setSFRBitNameInUse(P1, BIT_NUMBER_1, "P1.1");
	setSFRBitNameInUse(P1, BIT_NUMBER_2, "P1.2");
	setSFRBitNameInUse(P1, BIT_NUMBER_3, "P1.3");
	setSFRBitNameInUse(P1, BIT_NUMBER_4, "P1.4");
	setSFRBitNameInUse(P1, BIT_NUMBER_5, "P1.5");
	setSFRBitNameInUse(P1, BIT_NUMBER_6, "P1.6");
	setSFRBitNameInUse(P1, BIT_NUMBER_7, "P1.7");
	//SCON:n bitit
	setSFRBitNameInUse(SCON, BIT_NUMBER_0, "RI");
	setSFRBitNameInUse(SCON, BIT_NUMBER_1, "TI");
	setSFRBitNameInUse(SCON, BIT_NUMBER_2, "RB8");
	setSFRBitNameInUse(SCON, BIT_NUMBER_3, "TB8");
	setSFRBitNameInUse(SCON, BIT_NUMBER_4, "REN");
	setSFRBitNameInUse(SCON, BIT_NUMBER_5, "SM2");
	setSFRBitNameInUse(SCON, BIT_NUMBER_6, "SM1");
	setSFRBitNameInUse(SCON, BIT_NUMBER_7, "SM0");
	//P2:n bitit
	setSFRBitNameInUse(P2, BIT_NUMBER_0, "P2.0");
	setSFRBitNameInUse(P2, BIT_NUMBER_1, "P2.1");
	setSFRBitNameInUse(P2, BIT_NUMBER_2, "P2.2");
	setSFRBitNameInUse(P2, BIT_NUMBER_3, "P2.3");
	setSFRBitNameInUse(P2, BIT_NUMBER_4, "P2.4");
	setSFRBitNameInUse(P2, BIT_NUMBER_5, "P2.5");
	setSFRBitNameInUse(P2, BIT_NUMBER_6, "P2.6");
	setSFRBitNameInUse(P2, BIT_NUMBER_7, "P2.7");
	//IE:n bitit
	setSFRBitNameInUse(IE, BIT_NUMBER_0, "EX0");
	setSFRBitNameInUse(IE, BIT_NUMBER_1, "ET0");
	setSFRBitNameInUse(IE, BIT_NUMBER_2, "EX1");
	setSFRBitNameInUse(IE, BIT_NUMBER_3, "ET1");
	setSFRBitNameInUse(IE, BIT_NUMBER_4, "ES");
	setSFRBitNameInUse(IE, BIT_NUMBER_7, "EA");
	//P3:n bitit
	setSFRBitNameInUse(P3, BIT_NUMBER_0, "RXD");
	setSFRBitNameInUse(P3, BIT_NUMBER_1, "TXD");
	setSFRBitNameInUse(P3, BIT_NUMBER_2, "/INT0");
	setSFRBitNameInUse(P3, BIT_NUMBER_3, "/INT1");
	setSFRBitNameInUse(P3, BIT_NUMBER_4, "T0");
	setSFRBitNameInUse(P3, BIT_NUMBER_5, "T1");
	setSFRBitNameInUse(P3, BIT_NUMBER_6, "/WR");
	setSFRBitNameInUse(P3, BIT_NUMBER_7, "/RD");
	//IP:n bitit
	setSFRBitNameInUse(IP, BIT_NUMBER_0, "PX0");
	setSFRBitNameInUse(IP, BIT_NUMBER_1, "PT0");
	setSFRBitNameInUse(IP, BIT_NUMBER_2, "PX1");
	setSFRBitNameInUse(IP, BIT_NUMBER_3, "PT1");
	setSFRBitNameInUse(IP, BIT_NUMBER_4, "PS");
	//PSW:n bitit
	setSFRBitNameInUse(PSW, BIT_NUMBER_0, "P");
	setSFRBitNameInUse(PSW, BIT_NUMBER_1, "F1");
	setSFRBitNameInUse(PSW, BIT_NUMBER_2, "OV");
	setSFRBitNameInUse(PSW, BIT_NUMBER_3, "RS0");
	setSFRBitNameInUse(PSW, BIT_NUMBER_4, "RS1");
	setSFRBitNameInUse(PSW, BIT_NUMBER_5, "F0");
	setSFRBitNameInUse(PSW, BIT_NUMBER_6, "AC");
	setSFRBitNameInUse(PSW, BIT_NUMBER_7, "CY");
	//ACC:n bitit
	setSFRBitNameInUse(ACC, BIT_NUMBER_0, "ACC.0");
	setSFRBitNameInUse(ACC, BIT_NUMBER_1, "ACC.1");
	setSFRBitNameInUse(ACC, BIT_NUMBER_2, "ACC.2");
	setSFRBitNameInUse(ACC, BIT_NUMBER_3, "ACC.3");
	setSFRBitNameInUse(ACC, BIT_NUMBER_4, "ACC.4");
	setSFRBitNameInUse(ACC, BIT_NUMBER_5, "ACC.5");
	setSFRBitNameInUse(ACC, BIT_NUMBER_6, "ACC.6");
	setSFRBitNameInUse(ACC, BIT_NUMBER_7, "ACC.7");
	//B:n bitit
	setSFRBitNameInUse(B, BIT_NUMBER_0, "B.0");
	setSFRBitNameInUse(B, BIT_NUMBER_1, "B.1");
	setSFRBitNameInUse(B, BIT_NUMBER_2, "B.2");
	setSFRBitNameInUse(B, BIT_NUMBER_3, "B.3");
	setSFRBitNameInUse(B, BIT_NUMBER_4, "B.4");
	setSFRBitNameInUse(B, BIT_NUMBER_5, "B.5");
	setSFRBitNameInUse(B, BIT_NUMBER_6, "B.6");
	setSFRBitNameInUse(B, BIT_NUMBER_7, "B.7");
}

string I8031::readBitAddressableBitName(unsigned char const bitAddress)
{
	using namespace I8031_NameS;
	if(bitAddress<MIN_SFR_ADDRESS)      //Bit addressable area 0-7F
		return bitAreaNameTable_[bitAddress];
	else
	{
		if(bitAddress>=MIN_SFR_ADDRESS)      //SFR Bit addressable 80-FF
		{
			unsigned char bit=bitAddress%8;
			return sfrBitNameTable_[(bitAddress-bit-MIN_SFR_ADDRESS)*8+bit];
		}
		else
			return string("?");
	}
}

void I8031::clearBitAddressableArea(void)
{
	using namespace I8031_NameS;
	for(unsigned int n=MIN_BIT_AREA_ADDRESS;n<=MAX_BIT_AREA_ADDRESS;n++)
		internalDataMemory_[n].clear();
}

void I8031::clearRegisterBanks(void)
{
	using namespace I8031_NameS;
	for(unsigned int n=MIN_REGISTER_BANK_ADDRESS;n<=MAX_REGISTER_BANK_ADDRESS;n++)
		internalDataMemory_[n].clear();
}

void I8031::clearScratchPadArea(void)
{
	using namespace I8031_NameS;
	for(unsigned int n=MIN_SCRATCH_PAD_ADDRESS;n<=MAX_SCRATCH_PAD_ADDRESS;n++)
		internalDataMemory_[n].clear();
}


void I8031::saveWorkDataToFile(QDataStream& saveFileStream)
{
	using namespace I8031_NameS;

	timersCounters_->saveWorkDataToFile(saveFileStream);
	uart_->saveWorkDataToFile(saveFileStream);
	//Yksittäiset IO pinnit
	pinList_.saveWorkDataToFile(saveFileStream);

	unsigned long address;
	for(address=0;address<INTERNAL_DATA_MEMORY_SIZE;address++)
	{
		saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)isInternalDataMemoryLocationChangedAfterClear(address);
		saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)readInternalDataMemory(address);
		saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
	}

	for(address=0;address<EXTERNAL_DATA_MEMORY_SIZE;address++)
	{
		saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)isExternalDataMemoryChangedAfterClear(address);
		saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)readExternalDataMemory(address);
		saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
	}

	//Data
	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)clockSpeed_;
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)programCounter_;
	saveFileStream<<(Q_UINT16)2<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)clockTicksCalculated_;
	saveFileStream<<(Q_UINT16)3<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)clockTicksCounted_;
	saveFileStream<<(Q_UINT16)4<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)machineCycleCounter_;
	saveFileStream<<(Q_UINT16)5<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)oneStepAlePulses_;

	saveFileStream<<(Q_UINT16)6<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)previousExternalInt0Pin_;
	saveFileStream<<(Q_UINT16)7<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)previousExternalInt1Pin_;
	saveFileStream<<(Q_UINT16)8<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)priority0IntON_;
	saveFileStream<<(Q_UINT16)9<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)priority1IntON_;
	saveFileStream<<(Q_UINT16)10<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)interruptStarted_;
	saveFileStream<<(Q_UINT16)11<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)interruptDone_;
	saveFileStream<<(Q_UINT16)12<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)resetState;

	saveFileStream<<(Q_UINT16)13<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)lastExecutionProgramCounterValue_;
	saveFileStream<<(Q_UINT16)14<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)lastExecutionMachineCycleCounterValue_;

	QString qstr=codeBufferStr_;
	saveFileStream<<(Q_UINT16)15<<(Q_UINT8)KSimu51::QSTRING<<qstr;

	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void I8031::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	using namespace I8031_NameS;

	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	Q_UINT32 value32;
	Q_UINT8 value8Bool;
	Q_UINT8 value8Data;

	timersCounters_->readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);
	uart_->readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);
	//Yksittäiset IO pinnit
	pinList_.readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);

	unsigned long address;
	for(address=0;address<INTERNAL_DATA_MEMORY_SIZE;address++)
	{
		readFileStream>>indexReadData>>typeReadData>>value8Bool;      //Muuttunut bitti
		readFileStream>>indexReadData>>typeReadData>>value8Data;
		readFileStream>>indexReadData; //INDEX_SAVEDWORKDATA_END
		if(value8Bool)
			writeInternalDataMemory(address,value8Data);
	}

	for(address=0;address<EXTERNAL_DATA_MEMORY_SIZE;address++)
	{
		readFileStream>>indexReadData>>typeReadData>>value8Bool;   //Muuttunut bitti
		readFileStream>>indexReadData>>typeReadData>>value8Data;
		readFileStream>>indexReadData; //INDEX_SAVEDWORKDATA_END
		if(value8Bool)
			writeExternalDataMemory(address,value8Data);
	}

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case 0:
				readFileStream>>typeReadData>>value32;
				clockSpeed_=value32;
				break;
			case 1:
				readFileStream>>typeReadData>>value32;
				programCounter_=value32;
				break;
			case 2:
				readFileStream>>typeReadData>>value32;
				clockTicksCalculated_=value32;
				break;
			case 3:
				readFileStream>>typeReadData>>value32;
				clockTicksCounted_=value32;
				break;
			case 4:
				readFileStream>>typeReadData>>value32;
				machineCycleCounter_=value32;
				break;
			case 5:
				readFileStream>>typeReadData>>value32;
				oneStepAlePulses_=value32;
				break;
			case 6:
				readFileStream>>typeReadData>>value8Bool;
				previousExternalInt0Pin_=(bool)value8Bool;
				break;
			case 7:
				readFileStream>>typeReadData>>value8Bool;
				previousExternalInt1Pin_=(bool)value8Bool;
				break;
			case 8:
				readFileStream>>typeReadData>>value32;
				priority0IntON_=value32;
				break;
			case 9:
				readFileStream>>typeReadData>>value32;
				priority1IntON_=value32;
				break;
			case 10:
				readFileStream>>typeReadData>>value8Bool;
				interruptStarted_=(bool)value8Bool;
				break;
			case 11:
				readFileStream>>typeReadData>>value8Bool;
				interruptDone_=(bool)value8Bool;
				break;
			case 12:
				readFileStream>>typeReadData>>value8Bool;
				resetState=(bool)value8Bool;
				break;
			case 13:
				readFileStream>>typeReadData>>value32;
				lastExecutionProgramCounterValue_=value32;
				break;
			case 14:
				readFileStream>>typeReadData>>value32;
				lastExecutionMachineCycleCounterValue_=value32;
				break;
			case 15:
			{
				QString qstr;
				readFileStream>>typeReadData>>qstr;
				string apu(qstr.ascii());
				codeBufferStr_=apu;
			}
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

