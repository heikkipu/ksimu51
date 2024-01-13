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
#include "TexasInstrumentsP.h"

#include <qwidget.h>
#include <kpopupmenu.h>


TexasInstrumentsProcessors::TexasInstrumentsProcessors() : ProcessorsWC()
{
}

TexasInstrumentsProcessors::~TexasInstrumentsProcessors()
{
}

void TexasInstrumentsProcessors::addMenu(QWidget* parent, KPopupMenu* popupMenu)
{
	KPopupMenu* menuProcessorTI;
	menuProcessorTI=new KPopupMenu(parent);

	menuProcessorTI->setCheckable(TRUE);
	int id = menuProcessorTI->insertItem("CC2430", parent,  SLOT(addProcessorFromGroup_TexasInstruments(int)));
	menuProcessorTI->setItemParameter( id, CHIPCON_CC2430);

	popupMenu->insertItem(i18n("&TexasInstruments "),menuProcessorTI);
}

I51BaseW* TexasInstrumentsProcessors::addProcessorW_VirtualPart(QWidget* workspaceToAdd, int const  index, int const  type)
{
	I51BaseW* pointerToProcessorW;
	switch(type)
	{
		case CHIPCON_CC2430:
			//pointerToProcessorW=new CC2430Window(workspaceToAdd, PROCESSORWGROUPNAME_TEXASINSTRUMENTS, WDestructiveClose|WMouseNoMask, processorIndex);
			return 0; //Ota tää pois ku luokka valmis
		default:
			return 0;
	}
	return pointerToProcessorW;
}

