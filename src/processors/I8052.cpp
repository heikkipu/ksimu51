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
#include "I8052.h"

I8052::I8052() : I8051(), I8032()
{
	processorName_="8052";
	delete internalProgramMem;      //Deletoidaan kantaluokan sama
	internalProgramMem=new MemoryTable(INTERNAL_PROGRAM_MEMORY_SIZE);
	clearProgramMemory();
}

I8052::~I8052()
{
	//delete internalProgramMem;
}


void I8052::reset_VirtualPart()
{
	//kutsutaan kantaluokan resettiä
	I8051::reset_VirtualPart();
	I8032::reset_VirtualPart();
}

