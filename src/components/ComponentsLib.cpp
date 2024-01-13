/**************************************************************************
*   Copyright (C) 2005 - ... Heikki Pulkkinen                             *
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
#include "ComponentsLib.h"


ComponentsLib::ComponentsLib(QWidget* popupMenuParent, KPopupMenu* popupMenu, QWidget* workspace) : QWidget(popupMenuParent, "componentsLib")
{
	hide();
	componentIndexCounter_=1;
	workspaceToAddComponent_=workspace;
	//Add->Memory
	memoryC_ = new MemoryComponents();
	memoryC_->addMenu(this, popupMenu);

	//Add-Displays
	displaysC_ = new DisplaysComponents();
	displaysC_->addMenu(this, popupMenu);

	//Add-Logic74
	logic74C_ = new Logic74Components();
	logic74C_->addMenu(this, popupMenu);

	//Add-LogicGALPAL
	logicGALPALC_ = new LogicGALPALComponents();
	logicGALPALC_->addMenu(this, popupMenu);

	//Add-Keys
	keysButtonsC_ = new KeysButtonsComponents();
	keysButtonsC_->addMenu(this, popupMenu);

	//Add-MiscComponents
	miscC_ = new MiscComponents();
	miscC_->addMenu(this, popupMenu);

	//Add-InstrumentsComponents
	instrumentsC_ = new InstrumentsComponents();
	instrumentsC_->addMenu(this, popupMenu);
}

ComponentsLib::~ComponentsLib()
{
}

void ComponentsLib::addComponentFromGroup_Memory(int type)
{
	edmcw_=memoryC_->addComponent(workspaceToAddComponent_, type, componentIndexCounter_);
	if(edmcw_)
		sendCreationSignal();
}

void ComponentsLib::addComponentFromGroup_Displays(int type)
{
	edmcw_=displaysC_->addComponent(workspaceToAddComponent_, type, componentIndexCounter_);
	if(edmcw_)
		sendCreationSignal();
}

void ComponentsLib::addComponentFromGroup_KeysButtons(int type)
{
	edmcw_=keysButtonsC_->addComponent(workspaceToAddComponent_, type, componentIndexCounter_);
	if(edmcw_)
		sendCreationSignal();
}

void ComponentsLib::addComponentFromGroup_Logic74(int type)
{
	edmcw_=logic74C_->addComponent(workspaceToAddComponent_, type, componentIndexCounter_);
	if(edmcw_)
		sendCreationSignal();
}

void ComponentsLib::addComponentFromGroup_LogicGALPAL(int type)
{
	edmcw_=logicGALPALC_->addComponent(workspaceToAddComponent_, type, componentIndexCounter_);
	if(edmcw_)
		sendCreationSignal();
}

void ComponentsLib::addComponentFromGroup_Misc(int type)
{
	edmcw_=miscC_->addComponent(workspaceToAddComponent_, type, componentIndexCounter_);
	if(edmcw_)
		sendCreationSignal();

}

void ComponentsLib::addComponentFromGroup_Instruments(int type)
{
	edmcw_=instrumentsC_->addComponent(workspaceToAddComponent_, type, componentIndexCounter_);
	if(edmcw_)
		sendCreationSignal();
}

//-ModifyToCreateComponentLib-
void ComponentsLib::addComponent(unsigned long const  type, const QString& name)
{
	//Tähän vaihtoehto jokaisesta komponenttiryhmästä
	//RAM
	if(name.contains(COMPONENTGROUPNAME_MEMORY))
		addComponentFromGroup_Memory(type);
	//Displays
	if(name.contains(COMPONENTGROUPNAME_DISPLAYS))
		addComponentFromGroup_Displays(type);
	//Keyboards
	if(name.contains(COMPONENTGROUPNAME_KEYSBUTTONS))
		addComponentFromGroup_KeysButtons(type);
	//Misc components
	if(name.contains(COMPONENTGROUPNAME_MISC))
		addComponentFromGroup_Misc(type);
	//logic74 components
	if(name.contains(COMPONENTGROUPNAME_LOGIC74))
		addComponentFromGroup_Logic74(type);
	//logicGALPAL components
	if(name.contains(COMPONENTGROUPNAME_LOGICGALPAL))
		addComponentFromGroup_LogicGALPAL(type);
	//instruments components
	if(name.contains(COMPONENTGROUPNAME_INSTRUMENTS))
		addComponentFromGroup_Instruments(type);

}

void ComponentsLib::sendCreationSignal(void)
{
	componentIndexCounter_++;
	emit componentCreated(edmcw_);
}


