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
#include "TimersCounters.h"
#include "../ksimu51.h"


TimersCounters::TimersCounters(InternalRAM* const pToTL0, InternalRAM* const pToTH0, InternalRAM* const pToTL1, InternalRAM* const pToTH1, InternalRAM* const pToTMOD, InternalRAM* const pToTCON, InternalRAM* const pToIE, InternalRAM* const pToIOPort)
{
	pToTL0_=pToTL0;
	pToTH0_=pToTH0;
	pToTL1_=pToTL1;
	pToTH1_=pToTH1;
	pToTMOD_=pToTMOD;
	pToTCON_=pToTCON;
	pToIE_=pToIE;
	pToIOPort_=pToIOPort;
	
	timer0ClockDivider_=12;   //T0:n kellojakaja tässä processorissa 12. Jossain perivässä jotain muuta.
	timer1ClockDivider_=12; //T1:n kellojakaja tässä processorissa 12. Jossain perivässä jotain muuta.
}

TimersCounters::~TimersCounters()
{
}

void TimersCounters::reset(void)
{
	timer0Overflow_=false;
	timer1Overflow_=false;
	lastTx0pin_=true;
	lastTx1pin_=true;
	timer0ClockTicksCounter_=0;
	timer0Mode3ClockTicksCounter_=0;
	timer1ClockTicksCounter_=0;
	clockTicksLastValue_=0;
}

bool TimersCounters::countM0_13Bit(InternalRAM *pToTL, InternalRAM *pToTH, InternalRAM *pToTCON, unsigned char const bitTF, bool const intEnabled)
{
	bool overflow=false;
	unsigned int timerValueNow=pToTL->readValue()&0x1F;
	timerValueNow++;
	if(timerValueNow > 0x1F)
	{
		++*pToTH; //internalDataMemory_[THx]++
		if((pToTH->readValue()==0) && ((timerValueNow&0x1F)==0))
		{ //Interruptaa, jos sallittu
			if(intEnabled)
				*pToTCON|=bitTF;
			overflow=true;
		}
		timerValueNow=0;
	}
	*pToTL=static_cast<unsigned char>(timerValueNow&0x1F);
	return overflow;
}

bool TimersCounters::countM1_16Bit(InternalRAM *pToTL, InternalRAM *pToTH, InternalRAM *pToTCON, unsigned char const bitTF, bool const intEnabled)
{
	bool overflow=false;
	unsigned int timerValueNow=pToTL->readValue();
	timerValueNow++;
	if(timerValueNow > 0xFF)
	{
		++*pToTH;	//internalDataMemory_[THx]++;
		timerValueNow=0;
	}
	*pToTL=static_cast<unsigned char>(timerValueNow);
	if(((pToTH->readValue())==0) && ((pToTL->readValue())==0))
	{ //Interruptaa, jos sallittu
		if(intEnabled)
			*pToTCON|=bitTF;
		overflow=true;
	}
	return overflow;
}

bool TimersCounters::countM2_8BitReload(InternalRAM *pToTL, InternalRAM *pToTH, InternalRAM *pToTCON, unsigned char const bitTF, bool const intEnabled)
{
	bool overflow=false;
	unsigned int timerValueNow=pToTL->readValue();
	timerValueNow++;
	*pToTL=static_cast<unsigned char>(timerValueNow);
	if(timerValueNow > 0xFF)
	{
		*pToTL=*pToTH;
		if(intEnabled)
			*pToTCON|=bitTF;
		overflow=true;
	}
	return overflow;
}

bool TimersCounters::countM3_8Bit(InternalRAM *pToTx, InternalRAM *pToTCON, unsigned char const bitTF, bool const intEnabled)
{
	bool overflow=false;
	unsigned int timerValueNow=pToTx->readValue();
	timerValueNow++;
	*pToTx=static_cast<unsigned char>(timerValueNow);
	if(timerValueNow > 0xFF)
	{
		if(intEnabled)
			*pToTCON|=bitTF;
		overflow=true;
	}
	return overflow;
}

void TimersCounters::step(I8031_NameS::clockTicks_Type const clockTicks)
{
	using namespace I8031_NameS;
	timer0Overflow_=false;      //Nollataan ylivuodot joka kierolla
	timer1Overflow_=false;      //On päällä vain yhden kierroksen
	//Timer0: Toimintaheto
	unsigned char modeT0=(pToTMOD_->readValue()&TMOD_T0M1) | (pToTMOD_->readValue()&TMOD_T0M0);
	bool intEnabled=pToIE_->readValue()&IE_ET0;
	if((pToTCON_->readValue()&TCON_TR0) && (!(pToTMOD_->readValue()&TMOD_T0GATE) || (pToTMOD_->readValue()&TMOD_T0GATE && pToIOPort_->readValue()&P3_INT0)))
	{
		bool count=false;
		if(pToTMOD_->readValue()&TMOD_T0CT)      //Jos counter
		{
			if(lastTx0pin_ && !(pToIOPort_->readValue()&P3_T0)) //Jos laskeva reuna
				count=true;
		}
		else   //Timer0
		{
			timer0ClockTicksCounter_+=clockTicks-clockTicksLastValue_;
			if(timer0ClockTicksCounter_ >=timer0ClockDivider_)
			{
				timer0ClockTicksCounter_-=timer0ClockDivider_;
				count=true;
			}
		}
		if(count)
		{
			switch(modeT0)
			{
				case 0: //CounterTimer0: Mode 0 13bit
					timer0Overflow_=countM0_13Bit(pToTL0_, pToTH0_, pToTCON_, TCON_TF0, intEnabled);
					break;
				case 1: //CounterTimer0: Mode 1 16bit
					timer0Overflow_=countM1_16Bit(pToTL0_, pToTH0_, pToTCON_, TCON_TF0, intEnabled);
					break;
				case 2: //CounterTimer0: Mode 2 8bit reload
					timer0Overflow_=countM2_8BitReload(pToTL0_, pToTH0_, pToTCON_, TCON_TF0, intEnabled);
					break;
				case 3: //CounterTimer0:Mode3 timer 0 = 8 bit counter TL0
					timer0Overflow_=countM3_8Bit(pToTL0_, pToTCON_, TCON_TF0, intEnabled);
					break;
				default:
					break;
			}
		}
	}
	
	//Timer 0 mode3 TR1 kontrolloitu toinen 8 bitin counteri TH0
	if((modeT0==3)&&(pToTCON_->readValue()&TCON_TR1))
	{
		timer0Mode3ClockTicksCounter_+=clockTicks-clockTicksLastValue_;
		if(timer0Mode3ClockTicksCounter_ >=timer0ClockDivider_)
		{
			timer0Mode3ClockTicksCounter_-=timer0ClockDivider_;
			bool noUsableOverflow=countM3_8Bit(pToTH0_, pToTCON_, TCON_TF1, pToIE_->readValue()&IE_ET1);
		}
	}
	
	// Timer1
	unsigned char modeT1=((pToTMOD_->readValue()&TMOD_T1M1)|(pToTMOD_->readValue()&TMOD_T1M0))>>4;
	intEnabled=pToIE_->readValue()&IE_ET1;
	if(modeT0==3)//Timerin0 mode 3 diasbloi kesksytyksen
		intEnabled=false;
	//Toimintaehto
	if(((pToTCON_->readValue()&TCON_TR1) || ((modeT0==3)&&(modeT1!=3))) && (!(pToTMOD_->readValue()&TMOD_T1GATE) || ((pToTMOD_->readValue()&TMOD_T1GATE) && (pToIOPort_->readValue()&P3_INT1))))
	{
		bool count=false;
		if(pToTMOD_->readValue()&TMOD_T1CT)      //Jos counter1
		{
			if(lastTx1pin_ && !(pToIOPort_->readValue()&P3_T1)) //Jos laskeva reuna
				count=true;
		}
		else //Timer1
		{
			timer1ClockTicksCounter_+=clockTicks-clockTicksLastValue_;
			if(timer1ClockTicksCounter_ >=timer1ClockDivider_)
			{
				timer1ClockTicksCounter_-=timer1ClockDivider_;
				count=true;
			}
		}
		if(count)
		{
			switch(modeT1)
			{
				case 0: //CounterTimer1: Mode 0 13bit
					timer1Overflow_=countM0_13Bit(pToTL1_, pToTH1_, pToTCON_, TCON_TF1, intEnabled);
					break;
				case 1: //CounterTimer1: Mode 1 16bit
					timer1Overflow_=countM1_16Bit(pToTL1_, pToTH1_, pToTCON_, TCON_TF1, intEnabled);
					break;
				case 2: //CounterTimer1: Mode 2 8bit reload
					timer1Overflow_=countM2_8BitReload(pToTL1_, pToTH1_, pToTCON_, TCON_TF1, intEnabled);
					break;
				case 3: //CounterTimer1:Mode3 CounterTimer STOP
				default:
					break;
			}
		}
	}

	if(pToIOPort_->readValue()&P3_T0)
		lastTx0pin_=true;
	else
		lastTx0pin_=false;

	if(pToIOPort_->readValue()&P3_T1)
		lastTx1pin_=true;
	else
		lastTx1pin_=false;

	clockTicksLastValue_=clockTicks;
}

//Tietojen tallennus. Ei ANSI mukainen vaan QT
//Pitää määritellä perivissä, jos tallenetaan niiden ominaisuuksien tiloja, mutta kutsuttava myös tätä.
void TimersCounters::saveWorkDataToFile(QDataStream& saveFileStream)
{
	saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)timer0ClockDivider_;
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)timer1ClockDivider_;
	saveFileStream<<(Q_UINT16)2<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)timer0Overflow_;
	saveFileStream<<(Q_UINT16)3<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)timer1Overflow_;
	saveFileStream<<(Q_UINT16)4<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)lastTx0pin_;
	saveFileStream<<(Q_UINT16)5<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)lastTx1pin_;
	saveFileStream<<(Q_UINT16)6<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)timer0ClockTicksCounter_;
	saveFileStream<<(Q_UINT16)7<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)timer1ClockTicksCounter_;
	saveFileStream<<(Q_UINT16)8<<(Q_UINT8)KSimu51::QUINT32<<(Q_UINT32)clockTicksLastValue_;

	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void TimersCounters::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	Q_UINT32 value32;
	Q_UINT8 value8;

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case 0:
				readFileStream>>typeReadData>>value32;
				timer0ClockDivider_=value32;
				break;
			case 1:
				readFileStream>>typeReadData>>value32;
				timer1ClockDivider_=value32;
				break;
			case 2:
				readFileStream>>typeReadData>>value8;
				timer0Overflow_=(bool)value8;
				break;
			case 3:
				readFileStream>>typeReadData>>value8;
				timer1Overflow_=(bool)value8;
				break;
			case 4:
				readFileStream>>typeReadData>>value8;
				lastTx0pin_=(bool)value8;
				break;
			case 5:
				readFileStream>>typeReadData>>value8;
				lastTx1pin_=(bool)value8;
				break;
			case 6:
				readFileStream>>typeReadData>>value32;
				timer0ClockTicksCounter_=(int)value32;
				break;
			case 7:
				readFileStream>>typeReadData>>value32;
				timer1ClockTicksCounter_=(int)value32;
				break;
			case 8:
				readFileStream>>typeReadData>>value32;
				clockTicksLastValue_=value32;
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
