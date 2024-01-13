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
#include "AtmelP.h"

#include <qwidget.h>
#include <kpopupmenu.h>


AtmelProcessors::AtmelProcessors() : ProcessorsWC()
{
}

AtmelProcessors::~AtmelProcessors()
{
}

void AtmelProcessors::addMenu(QWidget* parent, KPopupMenu* popupMenu)
{
	KPopupMenu* menuProcessorAtmel;
	menuProcessorAtmel=new KPopupMenu(parent);

	menuProcessorAtmel->setCheckable( TRUE );
	int id = menuProcessorAtmel->insertItem("T89C51RD2_32", parent,  SLOT(addProcessorFromGroup_Atmel(int)));
	menuProcessorAtmel->setItemParameter( id, ATMEL_T89C51RD2_32);
	id = menuProcessorAtmel->insertItem("T89C51RD2_48", parent,  SLOT(addProcessorFromGroup_Atmel(int)));
	menuProcessorAtmel->setItemParameter( id, ATMEL_T89C51RD2_48);

	popupMenu->insertItem(i18n("&Atmel "), menuProcessorAtmel);
}

I51BaseW* AtmelProcessors::addProcessorW_VirtualPart(QWidget* workspaceToAdd, int const  index, int const  type)
{
	I51BaseW* pointerToProcessorW;
	switch(type)
	{
		case ATMEL_T89C51RD2_32:
		pointerToProcessorW=new AtmelT89C51RD2_32W(workspaceToAdd, PROCESSORWGROUPNAME_ATMEL, QWidget::WDestructiveClose|QWidget::WMouseNoMask, index, type);
			break;
		case ATMEL_T89C51RD2_48:
			pointerToProcessorW=new AtmelT89C51RD2_48W(workspaceToAdd, PROCESSORWGROUPNAME_ATMEL, QWidget::WDestructiveClose|QWidget::WMouseNoMask, index, type);
			break;
		default:
			return 0;
	}
	return pointerToProcessorW;
}

