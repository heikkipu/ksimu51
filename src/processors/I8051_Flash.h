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
#ifndef I8051_FLASH_H
#define I8051_FLASH_H

#include "I8051.h"

class I8051_Flash : virtual public I8051
{
	public:
		explicit I8051_Flash(unsigned long const flashSize);
		~I8051_Flash();

		bool isFlashMemoryValueChanged(unsigned int const );
		bool isFlashMemoryValueChanged(unsigned int& , unsigned int& );

	protected:
	private:
		I8051_Flash(){;};
};

#endif
