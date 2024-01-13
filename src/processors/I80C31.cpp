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
#include "I80C31.h"
#include "../ksimu51.h"


//Muodostin
I80C31::I80C31():I8031()
{
	processorName_="80C31";
	createSFRBitNameTable();
	createSFRNameTable();
}

//Hajotin
I80C31::~I80C31()
{
}

void I80C31::reset_VirtualPart()
{
	using namespace I80C31_NameS;
	internalDataMemory_[PCON]=0x0;
	idleMode_=false;
	powerDownMode_=false;
}

//Uudelleenmääritelty steppi
//Lisätty Power Save modet
void I80C31::step(bool const disassemble, bool const run)
{
	using namespace I80C31_NameS;
	unsigned char ieRekkariPDTilassa;
	if(run)
	{
		if((internalDataMemory_[PCON].readValue()&PCON_IDL)&&!idleMode_)   //Kun idleMode_ menee päälle
			idleMode_=true;
		if((internalDataMemory_[PCON].readValue()&PCON_PD)&&!powerDownMode_)   //Kun PowerDownMode menee päälle
			powerDownMode_=true;
		if(idleMode_||powerDownMode_)
			programCounter_=programCounterValueInPowerSaveMode_;
		else
			programCounterValueInPowerSaveMode_=programCounter_;
		//Kielletään keskeytykset processorilta stepin ajaksi jos PD mode. Ettei synny sähellystä
		if(powerDownMode_)
		{
			ieRekkariPDTilassa=internalDataMemory_[IE].readValue();
			internalDataMemory_[IE] &= static_cast<unsigned char>(~IE_EA);
		}
	}
	//Kutsutaan kantaluokan ohjelma jos ei powerdownissa.
	if((!powerDownMode_&&run)||disassemble)
		I8031::step(disassemble,run);
	if(run)
	{
		if(idleMode_||powerDownMode_)
		{
			//Jos idle mode luetaan muodostunut lista ja korjataan se jatkuvaksi ALE cycleksi.
			bool a8bit, alePin, psenPin, sampleP0Inputs, samplePxInputs;
			unsigned char p3RDPin, p3WRPin;
			int p0, p2;
			//externalCycleList_.readCycle(0, a8bit, alePin, psenPin, sampleP0Inputs, samplePxInputs, p3RDPin, p3WRPin, p0, p2);
			externalCycleList_->clear_Vecs();
			externalCycleList_->appendToExternalCycle_ReadFromExternalCodeMemoryCycle(static_cast<unsigned char>(p0), static_cast<unsigned char>(p2), static_cast<unsigned char>(p0), true);
		}
		//Palautetaan IE rekkarin tila ennen steppiä jos PD mode. Jotta tila päivittyy ennalleen
		if(powerDownMode_)
			internalDataMemory_[IE]=ieRekkariPDTilassa;

		if(interruptStarted_&&idleMode_)//Jos idle mode päällä ja tulee keskeytys
		{
			internalDataMemory_[PCON]&=(PCON_IDL^0xFF);
			idleMode_=false;
		}
	}
}

void I80C31::createSFRBitNameTable(void)
{
	using namespace I80C31_NameS;
	setSFRBitNameInUse(PCON, BIT_NUMBER_0, "IDL");
	setSFRBitNameInUse(PCON, BIT_NUMBER_1, "STOP");
	setSFRBitNameInUse(PCON, BIT_NUMBER_2, "GF0");
	setSFRBitNameInUse(PCON, BIT_NUMBER_3, "GF1");
	setSFRBitNameInUse(PCON, BIT_NUMBER_7, "SMOD");
}

void I80C31::createSFRNameTable(void)
{
	using namespace I80C31_NameS;
	setSFRInUse(PCON, "PCON", false);
}


//Ohjaa sarjaportin lähetystä eri moodeissa
//Muutos: baudinopeudet kantaluokkaan nähden SMOD bitin vuoksi
void I80C31::serialPortRxTxHandling(void)
{
	using namespace I80C31_NameS;
	//Määritellään jakajat uusiksi
	if(internalDataMemory_[PCON].readValue()&PCON_SMOD)
	{
		uart_->setMode1Mode3TimerDivider(16);
		uart_->setMode2ClockDivider(32);
	}
	else
	{
		uart_->setMode1Mode3TimerDivider(32);
		uart_->setMode2ClockDivider(64);
	}
	I8031::serialPortRxTxHandling(); //Kutsutaan kantaluokan ohjelma
}

void I80C31::saveWorkDataToFile(QDataStream& saveFileStream)
{
	I8031::saveWorkDataToFile(saveFileStream); //Käydään kantaluokassa

	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)idleMode_;
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)powerDownMode_;
	saveFileStream<<(Q_UINT16)2<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)programCounterValueInPowerSaveMode_;

	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void I80C31::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	I8031::readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);   //Kantaluokassa käynti

	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	Q_UINT8 value8Bool;
	Q_UINT32 value32;

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case 0:
				readFileStream>>typeReadData>>value8Bool;
				idleMode_=(bool)value8Bool;
				break;
			case 1:
				readFileStream>>typeReadData>>value8Bool;
				powerDownMode_=(bool)value8Bool;
				break;
			case 2:
				readFileStream>>typeReadData>>value32;
				programCounterValueInPowerSaveMode_=value32;
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

