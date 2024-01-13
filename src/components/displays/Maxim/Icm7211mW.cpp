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
#include "Icm7211mW.h"
#include "../../../ksimu51.h"

#include <kpopupmenu.h>

#include <qtooltip.h>

#include <string>

Icm7211mW::Icm7211mW( QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType) : ExternalComponentBaseW(parent, name, wflags, componentNumber, componentType, 1)
{
	componentName_+=" ICM7211M";

	displayQGridLayout_ = new QGridLayout(centralWidget(), 2, 1, 1, 1, "displayQGridLayout_");
	//4-Digit LCD näyttö
	displayQLCDNumber_ = new QLCDNumber(4, centralWidget(), "displayQLCDNumber_");
	displayQLCDNumber_->setSegmentStyle(QLCDNumber::Filled);
	displayQLCDNumber_->setSmallDecimalPoint (true);
	displayQLCDNumber_->display("0000");
	displayQGridLayout_->addWidget(displayQLCDNumber_, 0, 0 );
	displayQLCDNumber_->setFixedSize(150,50);
	//Tietotaulu
	addressTable_ = new QTable(centralWidget(),"addressTable_");
	QFont addressTable_font(addressTable_->font());
	addressTable_font.setFamily(FONTFAMILY);
	addressTable_font.setPointSize(10);
	addressTable_->setFont(addressTable_font);
	addressTable_->setFrameShape( QTable::GroupBoxPanel );
	addressTable_->setNumCols(1);
	addressTable_->setColumnStretchable(0, true);
	addressTable_->setNumRows(1);
	addressTable_->verticalHeader()->setLabel(0, "0000");
	addressTable_->horizontalHeader()->hide();
	addressTable_->setTopMargin(0);
	addressTable_->setFixedSize(displayQLCDNumber_->width()-40,addressTable_->rowHeight(0)+4);
	addressTable_->setRowMovingEnabled(false);
	addressTable_->setColumnMovingEnabled(false);
	addressTable_->setReadOnly(true);
	addressTable_->setSelectionMode(QTable::NoSelection);
	displayQGridLayout_->addWidget(addressTable_, 1, 0, Qt::AlignLeft);
	QToolTip::add(addressTable_->verticalHeader(),"Address");
	QToolTip::add(addressTable_,"Input:\nb0-b3=data0-data3\nb4,b5=segment\nb6,b7=desimal point");
	connect(addressTable_->verticalHeader(), SIGNAL(clicked(int)), this, SLOT(addressTableVerticalHeaderClicked(int)));

	//this->setFixedSize(displayQLCDNumber_->width()+2, addressTable_->height()+displayQLCDNumber_->height()+4);
	//Pääikkunaa ei kannata tehdä kiinteäkokiseksi. Minimikoko hieman pienmmäksi maksimia.
	maxWSize_=QSize(displayQLCDNumber_->width()+6,addressTable_->height()+displayQLCDNumber_->height()+26);
	this->setMinimumSize(displayQLCDNumber_->width(),addressTable_->height()+displayQLCDNumber_->height());
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

	addPopupMenu();
	for(int n=0;n<4;n++)
		digitValue_[n]=0;

	inputValueChanged_=false;
}

Icm7211mW::~Icm7211mW()
{
}

void Icm7211mW::addPopupMenu(void)
{
	QString qStr;
	int id,n;
	externalDataSpaceStartAddressMenu_=new KPopupMenu(this);
	//Upper Menu
	externalDataSpaceStartAddressUpperMenu_=new KPopupMenu(this);
	externalDataSpaceStartAddressMenu_->insertItem(i18n("&Upper "), externalDataSpaceStartAddressUpperMenu_);
	for(n=0;n<65536;n+=256)
	{
		qStr=qStr.setNum(n,16);
		if(qStr.length()<2)
			qStr.prepend('0');
		if(qStr.length()<3)
			qStr.prepend('0');
		if(qStr.length()<4)
			qStr.prepend('0');
		qStr[2]='x';
		qStr[3]='x';
		id=externalDataSpaceStartAddressUpperMenu_->insertItem(qStr.upper(), this,SLOT(externalDataSpaceStartAddressUpperActivated(int)));
		externalDataSpaceStartAddressUpperMenu_->setItemParameter(id,n);
	}
	//Lower menu
	externalDataSpaceStartAddressLowerMenu_=new KPopupMenu(this);
	externalDataSpaceStartAddressMenu_->insertItem(i18n("&Lower "), externalDataSpaceStartAddressLowerMenu_);
	for(n=0;n<256;n++)
	{
		qStr=qStr.setNum(n,16);
		if(qStr.length()<2)
			qStr.prepend('0');
		id=externalDataSpaceStartAddressLowerMenu_->insertItem(qStr.upper(), this,SLOT(externalDataSpaceStartAddressLowerActivated(int)));
		externalDataSpaceStartAddressLowerMenu_->setItemParameter(id,n);
	}
}

void Icm7211mW::externalDataSpaceStartAddressUpperActivated(int sA)
{
	changeExternalDataSpaceStartAddress(sA,true);
}

void Icm7211mW::externalDataSpaceStartAddressLowerActivated(int sA)
{
	changeExternalDataSpaceStartAddress(sA,false);
}

void Icm7211mW::changeExternalDataSpaceStartAddress(int const  sA, bool const  upper)
{
	QString qStr;
	unsigned int n;
	clearExternalDataSpace();
	clearDatapointers();
	unsigned int sAApu=sA;
	if(upper)
	{
		externalDataSpaceStartAddress_&=0x00FF;   //Maskataan ylöosa pois
		externalDataSpaceStartAddress_|=sAApu&0xFF00; //Oorataan yläosa
	}
	else
	{
		externalDataSpaceStartAddress_&=0xFF00;   //Maskataan alaosa pois
		externalDataSpaceStartAddress_|=sAApu&0x00FF; //Oorataan alaosa
	}
	qStr=qStr.setNum(externalDataSpaceStartAddress_,16);
	if(qStr.length()<2)
		qStr.prepend('0');
	if(qStr.length()<3)
		qStr.prepend('0');
	if(qStr.length()<4)
		qStr.prepend('0');
	addressTable_->verticalHeader()->setLabel(0, qStr.upper());
	emit message_MemoryValueChanged(this->name(), sA, 0);//Lähetetään viesti jotta kaikki tiedot päivittyy
	setDatapointers();
	updateDatapointers();
}

void Icm7211mW::writeExternalDataSpaceStartAddress(unsigned int const  address)
{
	//Ylä ja alosa pitää muuttaa erikseen
	changeExternalDataSpaceStartAddress(address,true);
	changeExternalDataSpaceStartAddress(address,false);
}

void Icm7211mW::addressTableVerticalHeaderClicked(int section)
{
	QRect verticalRect=addressTable_->verticalHeader()->sectionRect(section);
	externalDataSpaceStartAddressMenu_->popup(addressTable_->mapToGlobal(verticalRect.center()));
}


void Icm7211mW::codeBQCheckBoxStateChanged(int s)
{
	if(s==QButton::On)
		codeBIsOn_=true;
	else
		codeBIsOn_=false;
	update_VirtualPart();
}

void Icm7211mW::clearExternalDataSpace(void)
{
	ExternalComponentBaseW::clearExternalDataSpace(); //kantaluokan cleari
	addressTable_->clearCell(0,0);
	for(int n=0; n<NUM_DIGITS; n++)
		digitValue_[n]=0;
}

void Icm7211mW::update_VirtualPart(void)
{
	//addressTable_
	QString qStr;
	unsigned char lastInputValue=readExternalDataSpaceValue(externalDataSpaceStartAddress_);
	qStr=qStr.setNum(lastInputValue,16);
	if(qStr.length()<2)
		qStr.prepend('0');
	addressTable_->setText(0, 0, qStr.upper());
	//displayQLCDNumber_
	char codeA[6]={'A','B','C','D','E','F'};
	char codeB[6]={'-','E','H','L','P',' '};
	qStr="";
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
	if(lastInputValue>>6)
		qStr.insert(4-(lastInputValue>>6),'.');
	displayQLCDNumber_->display(qStr);
}

double Icm7211mW::execute_VirtualPart(double executeTime)
{
	double retVal=executeTime+1.0;
	//Käytetään kantaluokan metodeja, niin osoitteet sattuu oikein.
	if(isExternalDataSpaceValueChanged(externalDataSpaceStartAddress_)||inputValueChanged_)
	{
		unsigned char value=readExternalDataSpaceValue(externalDataSpaceStartAddress_);
		digitValue_[(value>>4)&0x03]=value&0x0F;
		changes_=true;
		inputValueChanged_=false;
	}
	return retVal;
}

void Icm7211mW::updateDatapointers(void)
{
	QString s=addressTable_->text(0, 0);
	addressTable_->clearCell(0, 0);
	addressTable_->setText(0, 0, s);
	if(isDPTR_)
		addressTable_->setPixmap(0, 0, dptrMark_);
	if(isR0_)
		addressTable_->setPixmap(0, 0, r0Mark_);
	if(isR1_)
		addressTable_->setPixmap(0, 0, r1Mark_);
	if(isDPTR_&&isR0_&&(*pToDPTRValue_==*pToR0Value_))
		addressTable_->setPixmap(0, 0, dptrR0Mark_);
	if(isDPTR_&&isR1_&&(*pToDPTRValue_==*pToR1Value_))
		addressTable_->setPixmap(0, 0, dptrR1Mark_);
	if(isR0_&&isR1_&&(*pToR0Value_==*pToR1Value_))
		addressTable_->setPixmap(0, 0, r0r1Mark_);
	if(isDPTR_&&isR0_&&isR1_&&(*pToDPTRValue_==*pToR0Value_)&&(*pToDPTRValue_==*pToR1Value_))
		addressTable_->setPixmap(0, 0, dptrR0R1Mark_);
}

void Icm7211mW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	ExternalComponentBaseW::saveWorkDataToFile(saveFileStream);   //Talletetaan kantaluokan jutut

	//Digits
	for(int n=0; n<NUM_DIGITS; n++)
		saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)digitValue_[n];

	//CodeB
	saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)codeBIsOn_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void Icm7211mW::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
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
	inputValueChanged_=true;
}

