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
#include "Logic74B.h"
#include "Logic74C.h"
#include "../../ksimu51.h"


Logic74BaseW::Logic74BaseW(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType) : ExternalComponentBaseW(parent, name, wflags, componentNumber, componentType)
{
	delayTimeTyp_=DELAYTIME74HCXXX;
	randomizeDelayTime(DELAYTIME_RANDOMVALUE); //delaylisä

	componentImageQLabel_=new QLabel(centralWidget(),"componentImageQLabel");
}

Logic74BaseW::~Logic74BaseW()
{
}

void Logic74BaseW::initW(void)
{
	componentImageQLabel_->resize(componentImage_.width(),componentImage_.height());

	int wL1=0, wL2=0, wR1=0, wR2=0, hL=0, hR=delayTimeTypNumInput_->height()+delayTimeRandomNumInput_->height()+8;
	if(inputPinCounter_)
	{
		wL1+=inputPinKListBox_->maxItemWidth();
		hL+=inputPinKListBox_->itemHeight(0)*inputPinKListBox_->numRows()+4;
	}
	if(bidirPinCounterInLeftKToolBar_)
	{
		wL2+=bidirPinKListBoxInLeftKToolBar_->maxItemWidth();
		hL+=bidirPinKListBoxInLeftKToolBar_->itemHeight(0)*bidirPinKListBoxInLeftKToolBar_->numRows()+4;
	}
	if(outputPinCounter_)
	{
		wR1+=outputPinKListBox_->maxItemWidth();
		hR+=outputPinKListBox_->itemHeight(0)*outputPinKListBox_->numRows()+4;
	}
	if(bidirPinCounterInRightKToolBar_)
	{
		wR2+=bidirPinKListBoxInRightKToolBar_->maxItemWidth();
		hR+=bidirPinKListBoxInRightKToolBar_->itemHeight(0)*bidirPinKListBoxInRightKToolBar_->numRows()+4;
	}
	int h=std::max(std::max(hL, hR), componentImageQLabel_->height());
	int w=std::max(wL1, wL2)+std::max(std::max(wR1, wR2), delayTimeTypNumInput_->width());
	maxWSize_=QSize(w+componentImageQLabel_->width()+10, h+10);
	setMinimumHeight(h);
	setMinimumWidth(w);
	setGeometry(0,0,maxWSize_.width(), maxWSize_.height());

	delayTimeTypNumInput_->setValue(delayTimeTyp_*1.0e9);
	delayTimeCalculate();
}


double Logic74BaseW::execute_VirtualPart(double executeTime)
{
	return execute_VirtualPart_ExecuteInputChange(executeTime); //Käytetään yleisohjelmaa kantaluokasta
}

void Logic74BaseW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	ExternalComponentBaseW::saveWorkDataToFile(saveFileStream);

	//saveFileStream<<(Q_UINT16)X<<(Q_UINT8)KSimu51::QUINTxx<<(Q_UINTxx)Data_xxx_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void Logic74BaseW::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	ExternalComponentBaseW::readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);

	initW();

	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			//case X:
			//	readFileStream>>typeReadData>>Data_xxx_;
			//	break;
			case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
				done=true;
				break;
			default:
				done = !funcHandleUnknownIndexRead(readFileStream);
		}
	}
	while(!done);
}

