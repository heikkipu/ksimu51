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
#ifndef T89C51RD2_48_NAMES_H
#define T89C51RD2_48_NAMES_H

namespace T89C51RD2_48_NameS
{
	unsigned long const  INTERNAL_PROGRAM_MEMORY_SIZE=0x10000; //64K
	unsigned int const  INTERNAL_XRAM_MEMORY_SIZE=0x400;
	unsigned int const  INTERNAL_XRAM_START_ADDRESS=0x0;

	//SFR rekkarit
	//P4
	unsigned char const  P4=0xC0;
	//P4:n bitti
	unsigned char const  P4_0=BIT_0,
		P4_1=BIT_1,
		P4_2=BIT_2,
		P4_3=BIT_3,
		P4_4=BIT_4,
		P4_5=BIT_5,
		P4_6=BIT_6,
		P4_7=BIT_7;

	//P5
	unsigned char const  P5=0xE8;
	//P5:n bitti
	unsigned char const
		P5_0=BIT_0,
		P5_1=BIT_1,
		P5_2=BIT_2,
		P5_3=BIT_3,
		P5_4=BIT_4,
		P5_5=BIT_5,
		P5_6=BIT_6,
		P5_7=BIT_7;

};

#endif
