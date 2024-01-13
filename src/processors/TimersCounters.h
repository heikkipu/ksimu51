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
#ifndef TIMERSCOUNTERS_H
#define TIMERSCOUNTERS_H

#include "InternalRAM.h"
#include <qdatastream.h>
#include "I8031_NameS.h"

class TimersCounters
{
	public:
		
		TimersCounters(InternalRAM* const pToTL0, InternalRAM* const pToTH0, InternalRAM* const pToTL1, InternalRAM* const pToTH1, InternalRAM* const pToTMOD, InternalRAM* const pToTCON, InternalRAM* const pToIE, InternalRAM* const pToIOPort);
		~TimersCounters();

		virtual void reset(void);
		virtual void step(I8031_NameS::clockTicks_Type const clockTicks);
		
		inline bool timer0Overflow(void){return timer0Overflow_;};
		inline bool timer1Overflow(void){return timer1Overflow_;};


		//Tietojen tallennus. Ei ANSI mukainen vaan QT
		//Pitää määritellä perivissä, jos tallenetaan niiden ominaisuuksien tiloja, mutta kutsuttava myös tätä.
		virtual void saveWorkDataToFile(QDataStream& saveFileStream);
		virtual void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead);
	
	protected:
		InternalRAM *pToTL0_;
		InternalRAM *pToTH0_;
		InternalRAM *pToTL1_;
		InternalRAM *pToTH1_;
		InternalRAM *pToTMOD_;
		InternalRAM *pToTCON_;
		InternalRAM *pToIE_;
		InternalRAM *pToIOPort_;
		
		unsigned int timer0ClockDivider_;   //T0:n kellojakaja 
		unsigned int timer1ClockDivider_; //T1:n kellojakaja

		//Perivien käytössä olevat laskentamodet. Counttaa++.
		bool countM0_13Bit(InternalRAM *pToTL, InternalRAM *pToTH, InternalRAM *pToTCON, unsigned char const bitTF, bool const intEnabled);
		bool countM1_16Bit(InternalRAM *pToTL, InternalRAM *pToTH, InternalRAM *pToTCON, unsigned char const bitTF, bool const intEnabled);
		bool countM2_8BitReload(InternalRAM *pToTL, InternalRAM *pToTH, InternalRAM *pToTCON, unsigned char const bitTF, bool const intEnabled);
		bool countM3_8Bit(InternalRAM *pToTx, InternalRAM *pToTCON, unsigned char const bitTF, bool const intEnabled);
	
	private:
		TimersCounters(){};

		bool timer0Overflow_;         //Merkki ylivuodosta
		bool timer1Overflow_;         //Merkki ylivuodosta. Lasketaan lähetysnopeus timerimoodissa.

		//Counter moden edellisen konejakson inputpinnin tilatiedot
		bool lastTx0pin_;
		bool lastTx1pin_;
		
		int timer0ClockTicksCounter_;
		int timer0Mode3ClockTicksCounter_;
		int timer1ClockTicksCounter_;
		 
		unsigned long clockTicksLastValue_; //

};

#endif

