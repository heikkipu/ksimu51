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
#include "MaximP.h"

#include <qwidget.h>
#include <kpopupmenu.h>


MaximProcessors::MaximProcessors() : ProcessorsWC()
{
}

MaximProcessors::~MaximProcessors()
{
}

void MaximProcessors::addMenu(QWidget* parent, KPopupMenu* popupMenu)
{
	KPopupMenu* menuProcessorMaxim;
	menuProcessorMaxim=new KPopupMenu(parent);

	menuProcessorMaxim->setCheckable(TRUE);
	int id = menuProcessorMaxim->insertItem("MAX7651", parent,  SLOT(addProcessorFromGroup_Maxim(int)));
	menuProcessorMaxim->setItemParameter( id, MAXIM_MAX7651);
	id = menuProcessorMaxim->insertItem("MAX7652", parent,  SLOT(addProcessorFromGroup_Maxim(int)));
	menuProcessorMaxim->setItemParameter( id, MAXIM_MAX7652);

	popupMenu->insertItem(i18n("&Maxim "), menuProcessorMaxim);
}

I51BaseW* MaximProcessors::addProcessorW_VirtualPart(QWidget* workspaceToAdd, int const  index, int const  type)
{
	I51BaseW* pointerToProcessorW;
	switch(type)
	{
		case MAXIM_MAX7651:
			pointerToProcessorW=new MaximMAX7651W(workspaceToAdd, PROCESSORWGROUPNAME_MAXIM, QWidget::WDestructiveClose|QWidget::WMouseNoMask, index, type);
			break;
		case MAXIM_MAX7652:
			pointerToProcessorW=new MaximMAX7652W(workspaceToAdd, PROCESSORWGROUPNAME_MAXIM, QWidget::WDestructiveClose|QWidget::WMouseNoMask, index, type);
			break;
		default:
			return 0;
	}
	return pointerToProcessorW;
}

