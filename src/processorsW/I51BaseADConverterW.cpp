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
#include "I51BaseADConverterW.h"

//---------------------------------Analogia inputit.
AnalogInputExternalW::AnalogInputExternalW(I8031* processor, QWidget* parent, char const * name, int wflag): ExternalW(processor, parent, name, wflag)
{
	isAnalogInput_=false;
	addAnalogInputPins(dynamic_cast<I8031_ADConverter*>(processor)->readAnalogInputPins());
}

AnalogInputExternalW::~AnalogInputExternalW()
{
}

//Voidaan luoda uudelleen.
void AnalogInputExternalW::addAnalogInputPins(AnalogInputPin* aiPinList)
{
	if(isAnalogInput_)
		delete analogInputPinsTools_;
	//Luodaan ensin toolbaari
	analogInputPinsTools_=new KToolBar( this, "analogInputPinsTools_" );
	addToolBar(analogInputPinsTools_, i18n( "A/D-Input Pins" ), DockRight, FALSE );
	analogInputPinsTools_->setMovingEnabled(false);
	analogInputPinsTools_->setMinimumWidth(300);
	analogInputPinsTools_->setMinimumHeight(300);
	//Labeli toolbaariin
	QLabel *analogInputPinsLabel= new QLabel(analogInputPinsTools_, "analogInputPinsLabel", 0);
	QFont analogInputPinsLabel_font(analogInputPinsLabel->font());
	analogInputPinsLabel_font.setFamily(FONTFAMILY);
	analogInputPinsLabel_font.setPointSize(10);
	analogInputPinsLabel->setFont(analogInputPinsLabel_font);
	analogInputPinsLabel->setText(i18n(" A/D-Input Pins "));
	analogInputPinsTools_->insertWidget(-1, 200, analogInputPinsLabel,-1);
	//Luodaan editointilista pinneistä
	QString pname;
	int index,id=0;
	AnalogInputPin* apuAIPin=new AnalogInputPin();
	apuAIPin=aiPinList;
	while(apuAIPin!=0)
	{
		pname=apuAIPin->readName();
		double min=apuAIPin->readMin();
		double max=apuAIPin->readMax();
		double val=apuAIPin->readValue();
		analogInputPinInput_=new IndexedKDoubleNumInput(id++,min, max, val, 0.001, 3, analogInputPinsTools_, pname);
		analogInputPinInput_->setPrefix(pname+": ");
		analogInputPinInput_->setSuffix("V");
		connect(analogInputPinInput_, SIGNAL(valueIsChanged(int, QString&, double)), this, SLOT(analogInputPinListValueChanged(int, QString&, double)));
		apuAIPin=apuAIPin->nextAnalogInputPin_;
	}
	isAnalogInput_=true;
}

void AnalogInputExternalW::analogInputPinListValueChanged(int id, QString& name, double dV)
{
	I8031_ADConverter* p = dynamic_cast<I8031_ADConverter*>(pToProcessor_);
	p->writeAnalogInputPinValue(name, dV);
}

I51BaseADConverterW::I51BaseADConverterW(QWidget* parent, char const * name, int wflags, int const  index, int const  type) : I51BaseW(parent, name, wflags, index, type)
{
}

I51BaseADConverterW::~I51BaseADConverterW()
{
}

void I51BaseADConverterW::createExternalAreaWindow(void)
{
	externalw_= new AnalogInputExternalW(processor_, windowWorkspace_, WINDOWNAME_EXTERNAL_AREA, WDestructiveClose|WMouseNoMask);
	externalw_->show();
}

void I51BaseADConverterW::saveWorkDataToFile_VirtualPart(QDataStream& saveFileStream)
{
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void I51BaseADConverterW::readWorkDataFromFile_VirtualPart(QDataStream& readFileStream,
													   std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	bool done=false;
	do
	{
		readFileStream>>indexReadData;
		switch(indexReadData)
		{
			case KSimu51_NameS::INDEX_SAVEDWORKDATA_END:
				done=true;
				break;
			default:
				done = !funcHandleUnknownIndexRead(readFileStream);
		}
	}
	while(!done);
}
