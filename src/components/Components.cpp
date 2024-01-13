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
#include "Components.h"

#include <qwidget.h>
#include <kpopupmenu.h>


Components::Components()
{
	iconKPixmap.load("pics/KS51hi32Memory.png", 0, KPixmap::Auto );
}

Components::~Components()
{
}

ExternalComponentBaseW* Components::addComponent(QWidget * parent, int const  type, int const  componentIndexCounter)
{
	QString indexQStr;
	indexQStr.setNum(componentIndexCounter,10);

	ExternalComponentBaseW* pointerToEDMCW=addComponent_VirtualPart(parent, type, componentIndexCounter, indexQStr);
	if(pointerToEDMCW)
	{
		pointerToEDMCW->setIcon(iconKPixmap);
		pointerToEDMCW->show();
		indexQStr=*pointerToEDMCW->readName();
		pointerToEDMCW->setPlainCaption(indexQStr);
		return pointerToEDMCW;
	}
	return 0;
}

