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
#ifndef TIMERSCOUNTERS2_H
#define TIMERSCOUNTERS2_H

#include "InternalRAM.h"
#include "TimersCounters.h"
#include "I8032_NameS.h"

class TimersCounters2 : public TimersCounters
{
	public:
		TimersCounters2(InternalRAM* const pToTL0, InternalRAM* const pToTH0, InternalRAM* const pToTL1, InternalRAM* const pToTH1, InternalRAM* const pToTMOD, InternalRAM* const pToTCON, InternalRAM* const pToIE, InternalRAM* const pToIOPort, InternalRAM* const pToTL2, InternalRAM* const pToTH2, InternalRAM* const pToRCAP2L, InternalRAM* const pToRCAP2H, InternalRAM* const pToT2CON, InternalRAM* const pToT2IOPort);
		~TimersCounters2();
		
		void reset(void) override;
		void step(I8032_NameS::clockTicks_Type const clockTicks) override;
		
		inline bool timer2Overflow(void){return timer2Overflow_;};

		//Tietojen tallennus. Ei ANSI mukainen vaan QT
		//Pitää määritellä perivissä, jos tallenetaan niiden ominaisuuksien tiloja, mutta kutsuttava myös tätä.
		void saveWorkDataToFile(QDataStream& saveFileStream) override;
		void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead) override;
	
	protected:
		InternalRAM *pToTL2_;
		InternalRAM *pToTH2_;
		InternalRAM *pToRCAP2L_;
		InternalRAM *pToRCAP2H_;
		InternalRAM *pToT2CON_;
		InternalRAM *pToT2IOPort_;
		
		unsigned int timer2ClockDivider_; //T2:n kellojakaja
		unsigned int timer2ClockDividerBaudRateGen_;

	private:
		//TimersCounters2(){};
		
		int timer2ClockTicksCounter_;
		bool lastTx2pin_;
		bool lastTx2Expin_;
		bool timer2Overflow_;

		unsigned long clockTicksLastValue_; //Käytetään oskun tikitysten laskuun

};

#endif

