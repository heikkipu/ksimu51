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
#ifndef I80C31_NAMES_H
#define I80C31_NAMES_H

#include "I8031_NameS.h"

namespace I80C31_NameS
{
	using namespace I8031_NameS;
	//SFR Registers
	unsigned char const  PCON=0x87;
	//SFR Registers Bits
	//PCON
	unsigned char const
		PCON_IDL=BIT_0,
		PCON_PD=BIT_1,
		PCON_GF0=BIT_2,
		PCON_GF1=BIT_3,
		PCON_SMOD=BIT_7;

};


#endif
