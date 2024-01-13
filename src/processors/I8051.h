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
#ifndef I8051_H
#define I8051_H

#include "I8031.h"
#include "I8051_ExternalCycle.h"
#include "I8051_NameS.h"

class I8051 : virtual public I8031
{
	public:
		I8051();
		~I8051();
		void clearProgramMemory(void) override;
		void writeProgramMemory(unsigned int const address, unsigned char const value) override;
		unsigned char readInternalProgramMemoryValue(unsigned int const  address);
		unsigned long readInternalProgramMemorySize(void);

		unsigned char readInstructionDoCycle(bool const readOnlyInstruction) override;
	
	protected:
		void reset_VirtualPart(void) override;

		MemoryTable* internalProgramMem;

	private:
		static unsigned int const  INTERNAL_PROGRAM_MEMORY_SIZE=0x1000;

};

#endif
