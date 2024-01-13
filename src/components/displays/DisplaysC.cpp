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
#include "DisplaysC.h"

#include <qwidget.h>
#include <kpopupmenu.h>
#include "../ExternalComponentBaseW.h"


DisplaysComponents::DisplaysComponents() : Components()
{
	iconKPixmap.load("pics/KS51hi32DisplayLinux.png", 0, KPixmap::Auto );
}

DisplaysComponents::~DisplaysComponents()
{
}

void DisplaysComponents::addMenu(QWidget * parent, KPopupMenu* popupMenu)
{
	//Add-Display
	KPopupMenu* displayMenu = new KPopupMenu(parent);
	popupMenu->insertItem(iconKPixmap, i18n("&DISPLAY"), displayMenu);
	//Add-Display-Controller
	KPopupMenu* displaysControllersKPopupMenu = new KPopupMenu(parent);
	displaysControllersKPopupMenu->setCheckable( TRUE );
	int id = displaysControllersKPopupMenu->insertItem("HD44780", parent,  SLOT(addComponentFromGroup_Displays(int)));
	displaysControllersKPopupMenu->setItemParameter( id, DisplaysComponents::HITACHI_HD44780);
	id = displaysControllersKPopupMenu->insertItem("ICM7211M", parent,  SLOT(addComponentFromGroup_Displays(int)));
	displaysControllersKPopupMenu->setItemParameter( id, DisplaysComponents::MAXIM_ICM7211M);
	id = displaysControllersKPopupMenu->insertItem("ICM7211", parent,  SLOT(addComponentFromGroup_Displays(int)));
	displaysControllersKPopupMenu->setItemParameter( id, DisplaysComponents::MAXIM_ICM7211);
	displayMenu->insertItem(i18n("&Controller "),displaysControllersKPopupMenu);
	//Add-Display-Module
	KPopupMenu* displaysModulesKPopupMenu = new KPopupMenu(parent);
	displaysModulesKPopupMenu->setCheckable(TRUE);
	id = displaysModulesKPopupMenu->insertItem("C550001", parent,  SLOT(addComponentFromGroup_Displays(int)));
	displaysModulesKPopupMenu->setItemParameter( id, DisplaysComponents::SEIKO_C550001);
	id = displaysModulesKPopupMenu->insertItem("PC0802A", parent,  SLOT(addComponentFromGroup_Displays(int)));
	displaysModulesKPopupMenu->setItemParameter( id, DisplaysComponents::POWERTRIP_PC0802A);
	displayMenu->insertItem(i18n("&Module"),displaysModulesKPopupMenu);

}

ExternalComponentBaseW* DisplaysComponents::addComponent_VirtualPart(QWidget * parent, int const  type, int const  componentIndexCounter, QString& indexString)
{
	indexString.prepend(COMPONENTGROUPNAME_DISPLAYS);
	ExternalComponentBaseW* pointerToEDMCW;
	switch(type)
	{
		case DisplaysComponents::MAXIM_ICM7211M:
			pointerToEDMCW = new Icm7211mW(parent, indexString, QWidget::WDestructiveClose|QWidget::WMouseNoMask, componentIndexCounter, type);
			break;
		case DisplaysComponents::MAXIM_ICM7211:
			pointerToEDMCW = new Icm7211W(parent, indexString, QWidget::WDestructiveClose|QWidget::WMouseNoMask, componentIndexCounter, type);
			break;
		case DisplaysComponents::HITACHI_HD44780:
			pointerToEDMCW = new Hd44780W(parent, indexString, QWidget::WDestructiveClose|QWidget::WMouseNoMask, componentIndexCounter, type);
			break;
		default:
			return 0;
	}
	return pointerToEDMCW;
}

