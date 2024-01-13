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
#include "Icm7211W.h"
#include "../../../ksimu51.h"

#include <kpopupmenu.h>

#include <qtooltip.h>

Icm7211W::Icm7211W( QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType) : ExternalComponentBaseW(parent, name, wflags, componentNumber, componentType)
{
	componentName_+=" ICM7211";

	//setCentralWidget( new QWidget( this, "qt_central_widget" ) );
	displayQGridLayout_= new QGridLayout(centralWidget(), 2, 1, 1, 1, "displayQGridLayout_");
	//Input
	addInputPin(PINNAME_DS0);
	addInputPin(PINNAME_DS1);
	addInputPin(PINNAME_DS2);
	addInputPin(PINNAME_DS3);
	addInputPin(PINNAME_D0);
	addInputPin(PINNAME_D1);
	addInputPin(PINNAME_D2);
	addInputPin(PINNAME_D3);
	pointterDS0_=ioList_.pToPin(PINNAME_DS0);      //Pointteri DS0:n alkuun
	pointterD0_=ioList_.pToPin(PINNAME_D0);         //Pointteri D0 alkuun

	//4-Digit LCD näyttö
	displayQLCDNumber_ = new QLCDNumber(4, centralWidget(), "displayQLCDNumber_");
	displayQLCDNumber_->setSegmentStyle(QLCDNumber::Filled);
	displayQLCDNumber_->setSmallDecimalPoint (true);
	displayQLCDNumber_->display("0000");
	displayQGridLayout_->addWidget(displayQLCDNumber_, 0, 0);
	//displayQLCDNumber_->setFixedSize(150,50);
	displayQLCDNumber_->setMaximumWidth(150);
	displayQLCDNumber_->setMinimumHeight(50);

	//Code B checkbox
	codeBQCheckBox_ = new QCheckBox(centralWidget(),"codeBQCheckBox_");
	QFont codeBQCheckBox_font(codeBQCheckBox_->font());
	codeBQCheckBox_font.setFamily(FONTFAMILY);
	codeBQCheckBox_font.setPointSize(10);
	codeBQCheckBox_->setFont(codeBQCheckBox_font);
	codeBQCheckBox_->setText(" ");
	displayQGridLayout_->addWidget(codeBQCheckBox_, 1, 0, Qt::AlignRight);
	connect(codeBQCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(codeBQCheckBoxStateChanged(int)));
	QToolTip::add(codeBQCheckBox_,"Code B");
	codeBIsOn_=false;

	//this->setFixedSize(this->frameSize());
	setMinimumSize(displayQLCDNumber_->frameSize());
	maxWSize_=QSize(200, inputPinKListBox_->itemHeight(0)*inputPinKListBox_->numRows()+34);

	for(int n=0;n<4;n++)
	{
		digitValue_[n]=0;
		prevDigitValue_[n]=0;
	}
}

Icm7211W::~Icm7211W()
{
}

void Icm7211W::codeBQCheckBoxStateChanged(int s)
{
	if(s==QButton::On)
		codeBIsOn_=true;
	else
		codeBIsOn_=false;
	inputPinKListBox_->clearSelection();
	update_VirtualPart();
}

void Icm7211W::clear(void)
{
	for(int n=0; n<NUM_DIGITS; n++)
	{
		digitValue_[n]=0;
		prevDigitValue_[n]=0;
	}
}

void Icm7211W::update_VirtualPart(void)
{
	//displayQLCDNumber_
	char codeA[6]={'A','B','C','D','E','F'};
	char codeB[6]={'-','E','H','L','P',' '};
	QString qStr;
	for(int n=3;n>=0;n--)
	{
		if(digitValue_[n]<10)
			qStr+=digitValue_[n]+0x30; //ASCIIKSI
		else
		{
			if(codeBIsOn_)
				qStr+=codeB[digitValue_[n]-10];
			else
				qStr+=codeA[digitValue_[n]-10];
		}
	}
	displayQLCDNumber_->display(qStr);
}

double Icm7211W::execute_VirtualPart(double executeTime)
{
	double retVal=executeTime+1.0;
	IOPin* apuPin=pointterD0_; //Viedään pointteri Data pinnien alkuun.
	char data=0;
	for(int n=0;n<4;n++)
	{
		if(apuPin->readValue(true))
			data|=1<<n;
		apuPin=apuPin->next();
	}
	//Pointteri DS0 pinniin
	apuPin=pointterDS0_;
	for(int n=0;n<4;n++)
	{
		if(apuPin->readValue(true))
		{
			digitValue_[n]=data;
			if(prevDigitValue_[n]!=digitValue_[n])
			{
				prevDigitValue_[n]=digitValue_[n];
				changes_=true;
			}
		}
		apuPin=apuPin->next();
	}
	return retVal;
}

void Icm7211W::saveWorkDataToFile(QDataStream& saveFileStream)
{
	ExternalComponentBaseW::saveWorkDataToFile(saveFileStream);   //Talletetaan kantaluokan jutut

	//Digits
	for(int n=0; n<NUM_DIGITS; n++)
		saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)digitValue_[n];

	//CodeB
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)codeBIsOn_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void Icm7211W::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	ExternalComponentBaseW::readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead); //Luetaan kantaluokan jutut

	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;
	Q_UINT8 value8;

	//Digits
	for(int n=0; n<NUM_DIGITS; n++)
	{
		readFileStream>>indexReadData>>typeReadData>>value8;
		digitValue_[n]=(char)value8;
	}

	//CodeB
	readFileStream>>indexReadData>>typeReadData>>value8;
	if(value8&&(codeBQCheckBox_->state()==QButton::Off))
		codeBQCheckBox_->toggle();

	readFileStream>>indexReadData; //INDEX_SAVEDWORKDATA_END
	update_VirtualPart();
}


