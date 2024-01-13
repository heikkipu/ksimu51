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
#include "InstrumentsC.h"

#include <qwidget.h>
#include <kpopupmenu.h>
#include "../ExternalComponentBaseW.h"


InstrumentsComponents::InstrumentsComponents() : Components ()
{
}

InstrumentsComponents::~InstrumentsComponents()
{
}

void InstrumentsComponents::addMenu(QWidget * parent, KPopupMenu* popupMenu)
{
	KPopupMenu* instrumentsComponentsKPopupMenu=new KPopupMenu(parent);
	instrumentsComponentsKPopupMenu->setCheckable(TRUE);
	int id = instrumentsComponentsKPopupMenu->insertItem("LOGIC-ANALYZER 8", parent,  SLOT(addComponentFromGroup_Instruments(int)));
	instrumentsComponentsKPopupMenu->setItemParameter(id, LOGICANALYZER8);
	id = instrumentsComponentsKPopupMenu->insertItem("LOGIC-ANALYZER 16", parent,  SLOT(addComponentFromGroup_Instruments(int)));
	instrumentsComponentsKPopupMenu->setItemParameter(id, LOGICANALYZER16);
	id = instrumentsComponentsKPopupMenu->insertItem("LOGIC-ANALYZER 24", parent,  SLOT(addComponentFromGroup_Instruments(int)));
	instrumentsComponentsKPopupMenu->setItemParameter(id, LOGICANALYZER24);
	id = instrumentsComponentsKPopupMenu->insertItem("LOGIC-ANALYZER 32", parent,  SLOT(addComponentFromGroup_Instruments(int)));
	instrumentsComponentsKPopupMenu->setItemParameter(id, LOGICANALYZER32);
	id = instrumentsComponentsKPopupMenu->insertItem("LOGIC-ANALYZER 40", parent,  SLOT(addComponentFromGroup_Instruments(int)));
	instrumentsComponentsKPopupMenu->setItemParameter(id, LOGICANALYZER40);
	
	id = instrumentsComponentsKPopupMenu->insertItem("LOGIC-GENERATOR", parent,  SLOT(addComponentFromGroup_Instruments(int)));
	instrumentsComponentsKPopupMenu->setItemParameter(id, LOGICGENERATOR);

	popupMenu->insertItem(i18n("&INSTRUMENTS"), instrumentsComponentsKPopupMenu);
}

ExternalComponentBaseW* InstrumentsComponents::addComponent_VirtualPart(QWidget * parent, int const  type, int const  componentIndexCounter, QString& indexString)
{
	indexString.prepend(COMPONENTGROUPNAME_INSTRUMENTS);
	ExternalComponentBaseW* pointerToEDMCW;
	int aType=type;
	switch(aType)
	{
		case 0:
			aType=1;
		case LOGICANALYZER8: //1
		case LOGICANALYZER16: //2
		case LOGICANALYZER24: //3
		case LOGICANALYZER32: //4
		case LOGICANALYZER40: //5
			pointerToEDMCW = new LogicAnalyzerW(parent, indexString, QWidget::WDestructiveClose|QWidget::WMouseNoMask, componentIndexCounter, aType);
			break;
		case LOGICGENERATOR:
			pointerToEDMCW = new LogicGeneratorW(parent, indexString, QWidget::WDestructiveClose|QWidget::WMouseNoMask, componentIndexCounter, aType);
			break;
		default:
			return 0;
	}
	return pointerToEDMCW;
}

