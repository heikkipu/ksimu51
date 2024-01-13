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
#include "MemoryC.h"

#include <qwidget.h>
#include <kpopupmenu.h>
#include "../ExternalComponentBaseW.h"


MemoryComponents::MemoryComponents() : Components ()
{
}

MemoryComponents::~MemoryComponents()
{
}

void MemoryComponents::addMenu(QWidget * parent, KPopupMenu* popupMenu)
{
	KPopupMenu* memoryKPopupMenu = new KPopupMenu( parent );
	memoryKPopupMenu->setCheckable(TRUE);
	int id = memoryKPopupMenu->insertItem("256x8", parent,  SLOT(addComponentFromGroup_Memory(int)));
	memoryKPopupMenu->setItemParameter( id, MemoryComponents::RAM256X8);
	id = memoryKPopupMenu->insertItem("512x8", parent,  SLOT(addComponentFromGroup_Memory(int)));
	memoryKPopupMenu->setItemParameter( id, MemoryComponents::RAM512X8);
	id = memoryKPopupMenu->insertItem("1024x8",parent, SLOT(addComponentFromGroup_Memory(int)));
	memoryKPopupMenu->setItemParameter( id, MemoryComponents::RAM1KX8);
	id = memoryKPopupMenu->insertItem("2048x8",parent, SLOT(addComponentFromGroup_Memory(int)));
	memoryKPopupMenu->setItemParameter( id, MemoryComponents::RAM2KX8);
	id = memoryKPopupMenu->insertItem("4096x8",parent, SLOT(addComponentFromGroup_Memory(int)));
	memoryKPopupMenu->setItemParameter( id, MemoryComponents::RAM4KX8);
	id = memoryKPopupMenu->insertItem("8192x8",parent, SLOT(addComponentFromGroup_Memory(int)));
	memoryKPopupMenu->setItemParameter( id, MemoryComponents::RAM8KX8);
	id = memoryKPopupMenu->insertItem("16384x8",parent, SLOT(addComponentFromGroup_Memory(int)));
	memoryKPopupMenu->setItemParameter( id, MemoryComponents::RAM16KX8);
	id = memoryKPopupMenu->insertItem("32768x8",parent, SLOT(addComponentFromGroup_Memory(int)));
	memoryKPopupMenu->setItemParameter( id, MemoryComponents::RAM32KX8);
	id = memoryKPopupMenu->insertItem("65536x8",parent, SLOT(addComponentFromGroup_Memory(int)));
	memoryKPopupMenu->setItemParameter( id, MemoryComponents::RAM64KX8);
	popupMenu->insertItem(iconKPixmap, i18n("&RAM"), memoryKPopupMenu);
}

ExternalComponentBaseW* MemoryComponents::addComponent_VirtualPart(QWidget * parent, int const  type, int const  componentIndexCounter, QString& indexString)
{
	ExternalComponentBaseW* pointerToEDMCW;
	indexString.setNum(componentIndexCounter,16);
	indexString.prepend(COMPONENTGROUPNAME_MEMORY);

	pointerToEDMCW = new ExternalRAMW(parent, indexString, QWidget::WDestructiveClose|QWidget::WMouseNoMask, componentIndexCounter, type, type);

	return pointerToEDMCW;
}

