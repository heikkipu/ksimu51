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
#include "Logic74C.h"

#include <qwidget.h>
#include <kpopupmenu.h>
#include "../ExternalComponentBaseW.h"


Logic74Components::Logic74Components() : Components ()
{
}

Logic74Components::~Logic74Components()
{
}

void Logic74Components::addMenu(QWidget * parent, KPopupMenu* popupMenu)
{
	KPopupMenu* logic74KPopupMenu = new KPopupMenu(parent);
	//Add-Logic74Components-xx573
	logic74KPopupMenu=new KPopupMenu(parent);
	logic74KPopupMenu->setCheckable(TRUE);
	int id = logic74KPopupMenu->insertItem("2I NAND", parent,  SLOT(addComponentFromGroup_Logic74(int)));
	logic74KPopupMenu->setItemParameter(id, GATES_2NAND);
	id = logic74KPopupMenu->insertItem("2I AND", parent,  SLOT(addComponentFromGroup_Logic74(int)));
	logic74KPopupMenu->setItemParameter(id, GATES_2AND);
	id = logic74KPopupMenu->insertItem("2I NOR", parent,  SLOT(addComponentFromGroup_Logic74(int)));
	logic74KPopupMenu->setItemParameter(id, GATES_2NOR);
	id = logic74KPopupMenu->insertItem("2I OR", parent,  SLOT(addComponentFromGroup_Logic74(int)));
	logic74KPopupMenu->setItemParameter(id, GATES_2OR);
	id = logic74KPopupMenu->insertItem("Decoder 3 To 8", parent,  SLOT(addComponentFromGroup_Logic74(int)));
	logic74KPopupMenu->setItemParameter(id, DECODER3TO8);
	id = logic74KPopupMenu->insertItem("Bus Transceiver 8", parent,  SLOT(addComponentFromGroup_Logic74(int)));
	logic74KPopupMenu->setItemParameter(id, BUSTRANSCEIVER8);
	id = logic74KPopupMenu->insertItem("Latch 8", parent,  SLOT(addComponentFromGroup_Logic74(int)));
	logic74KPopupMenu->setItemParameter(id, LATCH8);
	id = logic74KPopupMenu->insertItem("Inverter", parent,  SLOT(addComponentFromGroup_Logic74(int)));
	logic74KPopupMenu->setItemParameter(id, INVERTER);

	popupMenu->insertItem(iconKPixmap, i18n("&LOGIC 74xx"), logic74KPopupMenu);
}

ExternalComponentBaseW* Logic74Components::addComponent_VirtualPart(QWidget * parent, int const  type, int const  componentIndexCounter, QString& indexString)
{
	indexString.prepend(COMPONENTGROUPNAME_LOGIC74);
	ExternalComponentBaseW* pointerToEDMCW;
	switch(type)
	{
		case GATES_2NAND:
			pointerToEDMCW = new Gate2NANDW(parent, indexString, QWidget::WDestructiveClose|QWidget::WMouseNoMask, componentIndexCounter, type);
			break;
		case GATES_2AND:
			pointerToEDMCW = new Gate2ANDW(parent, indexString, QWidget::WDestructiveClose|QWidget::WMouseNoMask, componentIndexCounter, type);
			break;
		case GATES_2NOR:
			pointerToEDMCW = new Gate2NORW(parent, indexString, QWidget::WDestructiveClose|QWidget::WMouseNoMask, componentIndexCounter, type);
			break;
		case GATES_2OR:
			pointerToEDMCW = new Gate2ORW(parent, indexString, QWidget::WDestructiveClose|QWidget::WMouseNoMask, componentIndexCounter, type);
			break;
		case DECODER3TO8:
			pointerToEDMCW = new Decoder3To8W(parent, indexString, QWidget::WDestructiveClose|QWidget::WMouseNoMask, componentIndexCounter, type);
			break;
		case BUSTRANSCEIVER8:
			pointerToEDMCW = new BusTransceiver8W(parent, indexString, QWidget::WDestructiveClose|QWidget::WMouseNoMask, componentIndexCounter, type);
			break;
		case LATCH8:
			pointerToEDMCW = new Latch8W(parent, indexString, QWidget::WDestructiveClose|QWidget::WMouseNoMask, componentIndexCounter, type);
			break;
		case INVERTER:
			pointerToEDMCW = new GateInverterW(parent, indexString, QWidget::WDestructiveClose|QWidget::WMouseNoMask, componentIndexCounter, type);
			break;
		default:
			return 0;
	}
	return pointerToEDMCW;
}

