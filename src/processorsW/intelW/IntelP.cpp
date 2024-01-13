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
#include "IntelP.h"

#include <qwidget.h>
#include <kpopupmenu.h>


IntelProcessors::IntelProcessors() : ProcessorsWC()
{
}

IntelProcessors::~IntelProcessors()
{
}

void IntelProcessors::addMenu(QWidget* parent, KPopupMenu* popupMenu)
{
	KPopupMenu* menuProcessorIntel;
	menuProcessorIntel=new KPopupMenu(parent);

	menuProcessorIntel->setCheckable(TRUE);
	int id = menuProcessorIntel->insertItem("8031", parent,  SLOT(addProcessorFromGroup_Intel(int)));
	menuProcessorIntel->setItemParameter( id, INTEL_8031);
	id = menuProcessorIntel->insertItem("80C31", parent, SLOT(addProcessorFromGroup_Intel(int)));
	menuProcessorIntel->setItemParameter( id, INTEL_80C31);
	id = menuProcessorIntel->insertItem("8032", parent, SLOT(addProcessorFromGroup_Intel(int)));
	menuProcessorIntel->setItemParameter( id, INTEL_8032);
	id = menuProcessorIntel->insertItem("80C32", parent, SLOT(addProcessorFromGroup_Intel(int)));
	menuProcessorIntel->setItemParameter( id, INTEL_80C32);
	id = menuProcessorIntel->insertItem("8051", parent, SLOT(addProcessorFromGroup_Intel(int)));
	menuProcessorIntel->setItemParameter( id, INTEL_8051);
	id = menuProcessorIntel->insertItem("80C51", parent, SLOT(addProcessorFromGroup_Intel(int)));
	menuProcessorIntel->setItemParameter( id, INTEL_80C51);
	id = menuProcessorIntel->insertItem("8052", parent, SLOT(addProcessorFromGroup_Intel(int)));
	menuProcessorIntel->setItemParameter( id, INTEL_8052);
	id = menuProcessorIntel->insertItem("80C52", parent, SLOT(addProcessorFromGroup_Intel(int)));
	menuProcessorIntel->setItemParameter( id, INTEL_80C52);

	popupMenu->insertItem(i18n("&Intel "), menuProcessorIntel);
}

I51BaseW* IntelProcessors::addProcessorW_VirtualPart(QWidget* workspaceToAdd, int const  index, int const  type)
{
	I51BaseW* pointerToProcessorW;
	switch(type)
	{
		case INTEL_8031:
			pointerToProcessorW=new Intel8031W(workspaceToAdd, PROCESSORWGROUPNAME_INTEL, QWidget::WDestructiveClose|QWidget::WMouseNoMask, index, type);
			break;
		case INTEL_80C31:
			pointerToProcessorW=new Intel80C31W(workspaceToAdd, PROCESSORWGROUPNAME_INTEL, QWidget::WDestructiveClose|QWidget::WMouseNoMask, index, type);
			break;
		case INTEL_8032:
			pointerToProcessorW=new Intel8032W(workspaceToAdd, PROCESSORWGROUPNAME_INTEL, QWidget::WDestructiveClose|QWidget::WMouseNoMask, index, type);
			break;
		case INTEL_80C32:
			pointerToProcessorW=new Intel80C32W(workspaceToAdd, PROCESSORWGROUPNAME_INTEL, QWidget::WDestructiveClose|QWidget::WMouseNoMask, index, type);
			break;
		case INTEL_8051:
			pointerToProcessorW=new Intel8051W(workspaceToAdd, PROCESSORWGROUPNAME_INTEL, QWidget::WDestructiveClose|QWidget::WMouseNoMask, index, type);
			break;
		case INTEL_80C51:
			pointerToProcessorW=new Intel80C51W(workspaceToAdd, PROCESSORWGROUPNAME_INTEL, QWidget::WDestructiveClose|QWidget::WMouseNoMask, index, type);
			break;
		case INTEL_8052:
			pointerToProcessorW=new Intel8052W(workspaceToAdd, PROCESSORWGROUPNAME_INTEL, QWidget::WDestructiveClose|QWidget::WMouseNoMask, index, type);
			break;
		case INTEL_80C52:
			pointerToProcessorW=new Intel80C52W(workspaceToAdd, PROCESSORWGROUPNAME_INTEL, QWidget::WDestructiveClose|QWidget::WMouseNoMask, index, type);
			break;
		default:
			return 0;
	}
	return pointerToProcessorW;
}

