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
#include "ProcessorsWLib.h"


ProcessorsWLib::ProcessorsWLib(QWidget* popupMenuParent, KPopupMenu* popupMenu, QWidget* workspace) : QWidget(popupMenuParent, "processorsWLib")
{
	hide();
	workspaceToAddProcessor_=workspace;
	processorToAddIndex_=0;

	intelP_ = new IntelProcessors();
	intelP_->addMenu(this, popupMenu);

	atmelP_= new AtmelProcessors();
	atmelP_->addMenu(this, popupMenu);

	maximP_= new MaximProcessors();
	maximP_->addMenu(this, popupMenu);

	philipsP_= new PhilipsProcessors();
	philipsP_->addMenu(this, popupMenu);

	texasInstrumentsP_= new TexasInstrumentsProcessors();
	texasInstrumentsP_->addMenu(this, popupMenu);
}

ProcessorsWLib::~ProcessorsWLib()
{
}

void ProcessorsWLib::addProcessorFromGroup_Intel(int type)
{
	processorWindow_=intelP_->addProcessorW(workspaceToAddProcessor_, processorToAddIndex_, type);
	if(processorWindow_)
		sendCreationSignal();
}

void ProcessorsWLib::addProcessorFromGroup_Atmel(int type)
{
	processorWindow_=atmelP_->addProcessorW(workspaceToAddProcessor_, processorToAddIndex_, type);
	if(processorWindow_)
		sendCreationSignal();
}

void ProcessorsWLib::addProcessorFromGroup_Maxim(int type)
{
	processorWindow_=maximP_->addProcessorW(workspaceToAddProcessor_, processorToAddIndex_, type);
	if(processorWindow_)
		sendCreationSignal();
}

void ProcessorsWLib::addProcessorFromGroup_Philips(int type)
{
	processorWindow_=philipsP_->addProcessorW(workspaceToAddProcessor_, processorToAddIndex_, type);
	if(processorWindow_)
		sendCreationSignal();
}

void ProcessorsWLib::addProcessorFromGroup_TexasInstruments(int type)
{
	processorWindow_=texasInstrumentsP_->addProcessorW(workspaceToAddProcessor_, processorToAddIndex_, type);
	if(processorWindow_)
		sendCreationSignal();
}

//-ModifyToCreateComponentLib-
void ProcessorsWLib::addProcessor(int const  index, int const  type, const QString& name)
{
	processorToAddIndex_=index;

	if(name.contains(PROCESSORWGROUPNAME_INTEL))
		addProcessorFromGroup_Intel(type);
	if(name.contains(PROCESSORWGROUPNAME_ATMEL))
		addProcessorFromGroup_Atmel(type);
	if(name.contains(PROCESSORWGROUPNAME_MAXIM))
		addProcessorFromGroup_Maxim(type);
	if(name.contains(PROCESSORWGROUPNAME_PHILIPS))
		addProcessorFromGroup_Philips(type);
	if(name.contains(PROCESSORWGROUPNAME_TEXASINSTRUMENTS))
		addProcessorFromGroup_TexasInstruments(type);

	processorToAddIndex_=0;
}

void ProcessorsWLib::sendCreationSignal(void)
{
	emit processorCreated(processorWindow_);
}


