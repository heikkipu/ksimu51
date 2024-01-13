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
#include "InternalXRAMW.h"
#include "I51BaseW.h"

#include <string>

InternalXRAMW::InternalXRAMW( QWidget* parent, char const * name, int wflags, unsigned int const  startAddress, unsigned int const  size, bool const  charColumn)
	: InternalRAMW( parent, name, wflags, startAddress, size, charColumn)
{

	pointerSPMark_.load ( "pics/KS51hi8x16DPTRMark.png", 0, KPixmap::Auto );
	registerR0Mark_.load ( "pics/KS51hi8x16R0Mark.png", 0, KPixmap::Auto );
	registerR1Mark_.load ( "pics/KS51hi8x16R1Mark.png", 0, KPixmap::Auto );
	pointerSPRegisterR0Mark_.load("pics/KS51hi8x16DPTRR0Mark.png", 0, KPixmap::Auto);
	pointerSPRegisterR1Mark_.load("pics/KS51hi8x16DPTRR1Mark.png", 0, KPixmap::Auto);
	registerR0RegisterR1Mark_.load("pics/KS51hi8x16R0R1Mark.png", 0, KPixmap::Auto);
	pointerSPRegisterR0RegisterR1Mark_.load("pics/KS51hi8x16DPTRR0R1Mark.png", 0, KPixmap::Auto);

}

InternalXRAMW::~InternalXRAMW(void)
{
}

