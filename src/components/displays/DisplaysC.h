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
#ifndef DISPLAYSCOMPONENTS_H
#define DISPLAYSCOMPONENTS_H

#include "../Components.h"

#include "./Maxim/Icm7211mW.h"
#include "./Maxim/Icm7211W.h"
#include "./Hitachi/Hd44780W.h"

class Components;

class Icm7211mW;
class Icm7211W;
class Hd44780W;

class DisplaysComponents : public Components
{
	public:
		DisplaysComponents();
		~DisplaysComponents();

		void addMenu(QWidget* parent, KPopupMenu* popupMenu) override;

		#define COMPONENTGROUPNAME_DISPLAYS "displaysComponents"
		enum displaysType
		{
			HITACHI_HD44780,
			MAXIM_ICM7211M,
			MAXIM_ICM7211,
			SEIKO_C550001,
			POWERTRIP_PC0802A,
		};


	protected:
		ExternalComponentBaseW* addComponent_VirtualPart(QWidget * parent, int const  type, int const  componentIndexCounter, QString& indexString) override;
};

#endif

