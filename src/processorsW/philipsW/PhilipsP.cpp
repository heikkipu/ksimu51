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
#include "PhilipsP.h"

#include <qwidget.h>
#include <kpopupmenu.h>


PhilipsProcessors::PhilipsProcessors() : ProcessorsWC()
{
}

PhilipsProcessors::~PhilipsProcessors()
{
}

void PhilipsProcessors::addMenu(QWidget* parent, KPopupMenu* popupMenu)
{
	KPopupMenu* menuProcessorPhilips;
	menuProcessorPhilips=new KPopupMenu(parent);

	menuProcessorPhilips->setCheckable(TRUE);
	int id = menuProcessorPhilips->insertItem("P8031", parent,  SLOT(addProcessorFromGroup_Philips(int)));
	menuProcessorPhilips->setItemParameter( id, PHILIPS_P8031);

	popupMenu->insertItem(i18n("&Philips "), menuProcessorPhilips);
}

I51BaseW* PhilipsProcessors::addProcessorW_VirtualPart(QWidget* workspaceToAdd, int const  index, int const  type)
{
	I51BaseW* pointerToProcessorW;
	switch(type)
	{
		case PHILIPS_P8031:
			//pointerToProcessorW=new PhilipsP8031Window(workspaceToAdd, PROCESSORWGROUPNAME_PHILIPS, WDestructiveClose|WMouseNoMask, processorIndex);
			return 0; //Ota tää pois ku luokka valmis
			break;

		default:
			return 0;
	}
	return pointerToProcessorW;
}

