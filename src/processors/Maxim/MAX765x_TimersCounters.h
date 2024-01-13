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
#ifndef MAX765X_TIMERSCOUNTERS_H
#define MAX765X_TIMERSCOUNTERS_H

#include "../TimersCounters2.h"
#include "MAX765x_NameS.h"

class MAX765x_TimersCounters : public TimersCounters2
{
	public:
		struct RegCKCONBits
		{
			unsigned char bitT0M;
			unsigned char bitT1M;
			unsigned char bitT2M;
		};
		MAX765x_TimersCounters(InternalRAM* const pToTL0, InternalRAM* const pToTH0, InternalRAM* const pToTL1, InternalRAM* const pToTH1, InternalRAM* const pToTMOD, InternalRAM* const pToTCON, InternalRAM* const pToIE, InternalRAM* const pToIOPort, InternalRAM* const pToTL2, InternalRAM* const pToTH2, InternalRAM* const pToRCAP2L, InternalRAM* const pToRCAP2H, InternalRAM* const pToT2CON, InternalRAM* const pToT2IOPort, InternalRAM* const pToCKCON);
		~MAX765x_TimersCounters();
		
		virtual void step(MAX765x_NameS::clockTicks_Type const clockTicks);
		
	protected:
	private:
		//MAX765x_TimersCounters(){};

		InternalRAM *pToCKCON_;
		RegCKCONBits bitsToCKCON_;
};

#endif
