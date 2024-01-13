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
#include "MAX765x_TimersCounters.h"


MAX765x_TimersCounters::MAX765x_TimersCounters(InternalRAM* const pToTL0, InternalRAM* const pToTH0, InternalRAM* const pToTL1, InternalRAM* const pToTH1, InternalRAM* const pToTMOD, InternalRAM* const pToTCON, InternalRAM* const pToIE, InternalRAM* const pToIOPort, InternalRAM* const pToTL2, InternalRAM* const pToTH2, InternalRAM* const pToRCAP2L, InternalRAM* const pToRCAP2H, InternalRAM* const pToT2CON, InternalRAM* const pToT2IOPort, InternalRAM* const pToCKCON) : TimersCounters2(pToTL0, pToTH0, pToTL1, pToTH1, pToTMOD,  pToTCON, pToIE, pToIOPort, pToTL2, pToTH2, pToRCAP2L, pToRCAP2H, pToT2CON, pToT2IOPort)
{
	pToCKCON_=pToCKCON;
}

MAX765x_TimersCounters::~MAX765x_TimersCounters()
{
}

void MAX765x_TimersCounters::step(MAX765x_NameS::clockTicks_Type const clockTicks)
{
	using namespace MAX765x_NameS;
	
	//Timer 0 jakaja
	unsigned char ckconValue=pToCKCON_->readValue();
	if(ckconValue&CKCON_T0M)
		timer0ClockDivider_=4;
	else
		timer0ClockDivider_=12;
	//Timer 1 jakaja
	if(ckconValue&CKCON_T1M)
		timer1ClockDivider_=4;
	else
		timer1ClockDivider_=12;
	//Timer 2 jakaja
	if(ckconValue&CKCON_T2M)
		timer2ClockDivider_=4;
	else
		timer2ClockDivider_=12;
		
	TimersCounters2::step(clockTicks);
}
