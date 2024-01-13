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
#include "LogicGALPAL_C.h"

#include <qwidget.h>
#include <kpopupmenu.h>
#include "../ExternalComponentBaseW.h"


LogicGALPALComponents::LogicGALPALComponents() : Components ()
{
}

LogicGALPALComponents::~LogicGALPALComponents()
{
}

void LogicGALPALComponents::addMenu(QWidget * parent, KPopupMenu* popupMenu)
{
	KPopupMenu* logicGALPALKPopupMenu = new KPopupMenu(parent);
	//Add-LogicGALPALComponents-xx573
	logicGALPALKPopupMenu=new KPopupMenu(parent);
	logicGALPALKPopupMenu->setCheckable(TRUE);
	int id = logicGALPALKPopupMenu->insertItem("GAL22V10", parent,  SLOT(addComponentFromGroup_LogicGALPAL(int)));
	logicGALPALKPopupMenu->setItemParameter(id, GAL22V10);
	id = logicGALPALKPopupMenu->insertItem("ispGAL22V10", parent,  SLOT(addComponentFromGroup_LogicGALPAL(int)));
	logicGALPALKPopupMenu->setItemParameter(id, ISPGAL22V10);

	popupMenu->insertItem(iconKPixmap, i18n("&GAL/PAL"), logicGALPALKPopupMenu);
}

ExternalComponentBaseW* LogicGALPALComponents::addComponent_VirtualPart(QWidget * parent, int const  type, int const  componentIndexCounter, QString& indexString)
{
	indexString.prepend(COMPONENTGROUPNAME_LOGICGALPAL);
	ExternalComponentBaseW* pointerToEDMCW;
	switch(type)
	{
		case GAL22V10:
			pointerToEDMCW = new Gal22V10W(parent, indexString, QWidget::WDestructiveClose|QWidget::WMouseNoMask, componentIndexCounter, type);
			break;
		case ISPGAL22V10:
			pointerToEDMCW = new IspGAL22V10W(parent, indexString, QWidget::WDestructiveClose|QWidget::WMouseNoMask, componentIndexCounter, type);
			break;
		default:
			return 0;
	}
	return pointerToEDMCW;
}

