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
#ifndef MEMORYCOMPONENTS_H
#define MEMORYCOMPONENTS_H

#include "../Components.h"

#include "ExternalRAMW.h"

class Components;
class ExternalRAMW;

class MemoryComponents : public Components
{
	public:
		MemoryComponents();
		~MemoryComponents();

		void addMenu(QWidget* parent, KPopupMenu* popupMenu) override;

		#define COMPONENTGROUPNAME_MEMORY "RAM"
		enum ramType
		{
			RAM256X8   = 256,
			RAM512X8   = 512,
			RAM1KX8   = 1024,
			RAM2KX8   = 2048,
			RAM4KX8   = 4096,
			RAM8KX8   = 8192,
			RAM16KX8 = 16384,
			RAM32KX8 = 32768,
			RAM64KX8 = 65536,
		};
	protected:
		ExternalComponentBaseW* addComponent_VirtualPart(QWidget * parent, int const  type, int const  componentIndexCounter, QString& indexString) override;
};

#endif

