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
//Johdettu luokaista i8031 -> i8032
#ifndef I8052_H
#define I8052_H

#include "I8032.h"
#include "I8051.h"

class I8052: virtual public I8051, virtual public I8032
{
	public:
		I8052();
		~I8052();

	protected:
		void reset_VirtualPart(void) override;
	
	private:
		static unsigned long const  INTERNAL_PROGRAM_MEMORY_SIZE=0x2000;

};

#endif
