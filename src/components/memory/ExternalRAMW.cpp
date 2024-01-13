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
#include "ExternalRAMW.h"

#include <kpopupmenu.h>
#include "../../ksimu51.h"

#include <string>

ExternalRAMW::ExternalRAMW( QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType, unsigned int const  sz) : ExternalComponentBaseW(parent, name, wflags, componentNumber, componentType, sz)
{
	QString apuQStr;
	componentName_+=" RAM";
	componentName_+=apuQStr.setNum(sz,10);

	externalRAMTable_ = new QTable(this,"externalRAMTable_");
	setCentralWidget(externalRAMTable_);
	QFont externalRAMTable_font(externalRAMTable_->font());
	externalRAMTable_font.setFamily(FONTFAMILY);
	externalRAMTable_font.setPointSize(10);
	externalRAMTable_->setFont(externalRAMTable_font);
	externalRAMTable_->setFrameShape( QTable::GroupBoxPanel );

	externalDataSpaceStartAddress_=0;
	dptrRow_=0;
	dptrColumn_=0;
	r0Row_=0;
	r0Column_=0;
	r1Row_=0;
	r1Column_=0;

	unsigned int un;
	unsigned long size=externalDataSpaceValueT_->readSize();
	int n;
	QString qStr;

	//Columns
	externalRAMTable_->setNumCols(9);
	for(n=0;n<8;n++)
	{
		qStr=qStr.setNum(n,16);
		qStr.prepend('0');
		externalRAMTable_->horizontalHeader()->setLabel(n,qStr);
		externalRAMTable_->adjustColumn(n);
	}
	externalRAMTable_->horizontalHeader()->setLabel(8, "  CHAR ");
	externalRAMTable_->adjustColumn(8);
	//Rows
	int rowCount=0;
	externalRAMTable_->setNumRows(size/8);
	for(un=externalDataSpaceStartAddress_;un<externalDataSpaceStartAddress_+size;un+=8)
	{
		qStr=qStr.setNum(un,16);
		if(qStr.length()<2)
			qStr.prepend('0');
		if(qStr.length()<3)
			qStr.prepend('0');
		if(qStr.length()<4)
			qStr.prepend('0');
		qStr.prepend('[');
		qStr.append(']');
		externalRAMTable_->verticalHeader()->setLabel(rowCount, qStr.upper());
		//externalRAMTable_->setText(rowCount,8,"........");
		rowCount++;
	}

	externalRAMTable_->setRowMovingEnabled(false);
	externalRAMTable_->setColumnMovingEnabled(false);
	externalRAMTable_->setReadOnly(false);
	externalRAMTable_->setSelectionMode(QTable::Multi);
	externalRAMTable_->setColumnReadOnly (8, true); //Char sarake readonlyksi

	maxWSize_=QSize(externalRAMTableGeometrySize().width()+24, 400);


	connect(externalRAMTable_->verticalHeader(), SIGNAL(clicked(int)), this, SLOT(externalRAMTableVerticalHeaderClicked(int)));

	connect(externalRAMTable_, SIGNAL(valueChanged (int, int)), this, SLOT(externalRAMTableValueChanged( int, int)));
	connect(externalRAMTable_, SIGNAL(currentChanged (int, int)), this, SLOT(externalRAMTableCurrentChanged( int, int)));

	addPopupMenu();
}

ExternalRAMW::~ExternalRAMW()
{
}

void ExternalRAMW::changeAddressHeader(unsigned int sA)
{
	QString qStr;
	unsigned int n;
	externalDataSpaceStartAddress_=sA;
	unsigned int row=0;
	for(n=externalDataSpaceStartAddress_;n<externalDataSpaceStartAddress_+externalDataSpaceValueT_->readSize();n+=8)
	{
		qStr=qStr.setNum(n,16);
		if(qStr.length()<2)
			qStr.prepend('0');
		if(qStr.length()<3)
			qStr.prepend('0');
		if(qStr.length()<4)
			qStr.prepend('0');
		qStr.prepend('[');
		qStr.append(']');
		externalRAMTable_->verticalHeader()->setLabel(row, qStr.upper());
		row++;
	}
}

void ExternalRAMW::externalDataSpaceStartAddressActivated(int sA)
{
	clearExternalDataSpace();
	clearDatapointers();
	changeAddressHeader(sA);
	emit message_MemoryValueChanged(this->name(), sA, 0);//Lähetetään viesti jotta kaikki tiedot päivittyy
}

//Ulkoisesti muutetaan aloitusositetta
void ExternalRAMW::writeExternalDataSpaceStartAddress(unsigned int const  address)
{
	unsigned long size=externalDataSpaceValueT_->readSize();
	for(unsigned int startA=0;startA<65536;startA+=size) //Suhteutetaan kokoon
	{
		if((address>=startA)&&(address<startA+size))
		{
			changeAddressHeader(startA);
			break;
		}
	}
}

void ExternalRAMW::externalRAMTableVerticalHeaderClicked(int section)
{
	QRect verticalRect=externalRAMTable_->verticalHeader()->sectionRect(section);
	externalDataSpaceStartAddressMenu_->popup(externalRAMTable_->mapToGlobal(verticalRect.center()));
}

void ExternalRAMW::addPopupMenu(void)
{
	QString qStr;
	int id;
	externalDataSpaceStartAddressMenu_=new KPopupMenu(this);
	unsigned long size=externalDataSpaceValueT_->readSize();
	if(size<65536)
	{
		for(unsigned long n=0;n<65536;n+=size) //Rammin kokoon suhteutettu valikko
		//for(unsigned int n=0;(n+size)<=65536;n+=256)//256 pykälissä koko suhteutettu
		{
			qStr=qStr.setNum(n,16);
			if(qStr.length()<2)
				qStr.prepend('0');
			if(qStr.length()<3)
				qStr.prepend('0');
			if(qStr.length()<4)
				qStr.prepend('0');
			id=externalDataSpaceStartAddressMenu_->insertItem(qStr.upper(), this,SLOT(externalDataSpaceStartAddressActivated(int)));
			externalDataSpaceStartAddressMenu_->setItemParameter(id,n);
		}
	}
}

void ExternalRAMW::clearExternalDataSpace(void)
{
	ExternalComponentBaseW::clearExternalDataSpace();   //Käydään ekaksi kantaluokassa
	for(int n=0;n<externalRAMTable_->numRows();n++)
	{
		for(int m=0;m<externalRAMTable_->numCols();m++)
			externalRAMTable_->clearCell(n,m);
		externalRAMTable_->setText(n,8,"........");
	}
}

//UM
//Tehdään tiedonsiirto vain päivitysbitit muuttaen
void ExternalRAMW::setExternalDataSpaceValue(unsigned int const  address, unsigned char const  value)
{
	ExternalComponentBaseW::writeExternalDataSpaceValue(address, value, true); //Muutta changeAfterClearin
}

void ExternalRAMW::writeValueToTable(unsigned int address, unsigned char value)
{
	int row, column;
	QString qStr;
	row=address/8;
	column=address%8;
	qStr=qStr.setNum(value,16);
	if(qStr.length()<2)
		qStr.prepend('0');
	externalRAMTable_->setText(row, column, qStr.upper());
	qStr=externalRAMTable_->text(row, 8);
	if(value>=0x20)
		qStr[column]=char(value);
	else
		qStr[column]='.';
	externalRAMTable_->setText(row, 8, qStr);
}

void ExternalRAMW::update_VirtualPart(void)
{
	unsigned char value;
	unsigned int minA,maxA;
	if(externalDataSpaceValueT_->toUpdate(minA,maxA))
	{
		for(unsigned int address=minA;address<=maxA;address++)
		{
			if(externalDataSpaceValueT_->toUpdate(address))
			{
				value=externalDataSpaceValueT_->readValue(address);
				writeValueToTable(address, value);
			}
		}
	}
}

void ExternalRAMW::updateDatapointers(void)
{
	//DPTR Clear
	QString s=externalRAMTable_->text(dptrRow_, dptrColumn_);
	externalRAMTable_->clearCell(dptrRow_, dptrColumn_);
	externalRAMTable_->setText(dptrRow_, dptrColumn_, s);
	//R0 Clear
	s=externalRAMTable_->text(r0Row_, r0Column_);
	externalRAMTable_->clearCell(r0Row_, r0Column_);
	externalRAMTable_->setText(r0Row_, r0Column_, s);
	//R1 Clear
	s=externalRAMTable_->text(r1Row_, r1Column_);
	externalRAMTable_->clearCell(r1Row_, r1Column_);
	externalRAMTable_->setText(r1Row_, r1Column_, s);
	if(isDPTR_)
	{
		dptrRow_=(*pToDPTRValue_-externalDataSpaceStartAddress_)/8;
		dptrColumn_=(*pToDPTRValue_-externalDataSpaceStartAddress_)%8;
		externalRAMTable_->setPixmap(dptrRow_, dptrColumn_, dptrMark_);
	}
	if(isR0_)
	{
		r0Row_=(*pToR0Value_-externalDataSpaceStartAddress_)/8;
		r0Column_=(*pToR0Value_-externalDataSpaceStartAddress_)%8;
		externalRAMTable_->setPixmap(r0Row_, r0Column_, r0Mark_);
	}
	if(isR1_)
	{
		r1Row_=(*pToR1Value_-externalDataSpaceStartAddress_)/8;
		r1Column_=(*pToR1Value_-externalDataSpaceStartAddress_)%8;
		externalRAMTable_->setPixmap(r1Row_, r1Column_, r1Mark_);
	}
	if(isDPTR_&&isR0_&&(*pToDPTRValue_==*pToR0Value_))
		externalRAMTable_->setPixmap(dptrRow_, dptrColumn_, dptrR0Mark_);
	if(isDPTR_&&isR1_&&(*pToDPTRValue_==*pToR1Value_))
		externalRAMTable_->setPixmap(dptrRow_, dptrColumn_, dptrR1Mark_);
	if(isR0_&&isR1_&&(*pToR0Value_==*pToR1Value_))
		externalRAMTable_->setPixmap(r0Row_, r0Column_, r0r1Mark_);
	if(isDPTR_&&isR0_&&isR1_&&(*pToDPTRValue_==*pToR0Value_)&&(*pToDPTRValue_==*pToR1Value_))
		externalRAMTable_->setPixmap(dptrRow_, dptrColumn_, dptrR0R1Mark_);
}

void ExternalRAMW::showPointerPosition(unsigned char const  pointer)
{
	switch(pointer)
	{
		case ACTIVATE_DPTR:
			if(isDPTR_)
			{
				setFocus();
				externalRAMTable_->ensureCellVisible(dptrRow_, dptrColumn_);
			}
			break;
		case ACTIVATE_R0:
			if(isR0_)
			{
				setFocus();
				externalRAMTable_->ensureCellVisible(r0Row_, r0Column_);
			}
			break;
		case ACTIVATE_R1:
			if(isR1_)
			{
				setFocus();
				externalRAMTable_->ensureCellVisible(r1Row_, r1Column_);
			}
			break;
	}
}

QSize ExternalRAMW::externalRAMTableGeometrySize(void)
{
	int n;
	int height=externalRAMTable_->horizontalHeader()-> frameGeometry().height();
	for(n=0;n<externalRAMTable_->numRows();n++)
		height+=externalRAMTable_->rowHeight(n);
	int width=externalRAMTable_->verticalHeader()-> frameGeometry().width();
	for(n=0;n<externalRAMTable_->numCols();n++)
		width+=externalRAMTable_->columnWidth(n);
	return QSize(width,height);
}

void ExternalRAMW::externalRAMTableValueChanged(int row, int col)
{
	unsigned int address=externalDataSpaceStartAddress_+(row*8+col);
	QString messageStr(externalRAMTable_->text(row,col));
	QString valQStr("xx");
	if(messageStr.length()==1)
	{
		valQStr="0";
		valQStr+=QChar(messageStr[0]);
	}
	else
	{
		if((messageStr.length()>1)&&(messageStr.length()<3))
		{
			valQStr=QChar(messageStr[0]);
			valQStr+=QChar(messageStr[1]);
		}
		else
			valQStr="xx";
	}
	bool change_value,only_clear=false;
	unsigned int ui=valQStr.toUInt(&change_value,16);
	if(valQStr=="0 ")   //Jos oli syötetty välilyönti tyhjätään alkio/alkiot
		only_clear=true;
	if(change_value||only_clear)
	{
		//Katotaan valitut ja täytetään merkeillä.
		for(int n=0;n<externalRAMTable_->numRows();n++)
		{
			for(int m=0;m<8;m++)   //Char saraketta ei oteta huomioon
			{
				if(externalRAMTable_->isSelected(n, m)||((n==row)&&(m==col)))
				{
					if(change_value&&!only_clear)
					{
						address=externalDataSpaceStartAddress_+(n*8+m);
						externalDataSpaceValueT_->setValue(address-externalDataSpaceStartAddress_, ui);
					}
					if(only_clear)
					{
						externalRAMTable_->setText(n, m, "");
						QString qStr=externalRAMTable_->text(n, 8);
						qStr[m]='.';
						externalRAMTable_->setText(n, 8, qStr);
					}
				}
			}
		}

		//update_VirtualPart(); //Päivitetään taulukko.
		emit message_MemoryValueChanged(this->name(), address, ui);//Lähetetään viesti jotta kaikki tiedot päivittyy
	}
	else
	{
		externalRAMTable_->setText(row, col, externalRAMTableCurrentCellText_);
	}
}

void ExternalRAMW::externalRAMTableCurrentChanged(int row, int col)
{
	externalRAMTableCurrentCellText_=externalRAMTable_->text(row,col);
}

void ExternalRAMW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	ExternalComponentBaseW::saveWorkDataToFile(saveFileStream);   //Talletetaan kantaluokan jutut

	//saveFileStream<<(Q_UINT16)X<<(Q_UINT8)KSimu51::QUINTxx<<(Q_UINTxx)Data_xxx_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void ExternalRAMW::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	ExternalComponentBaseW::readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead); //Luetaan kantaluokan jutut

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

	//Päivitetään taulukko
	for(unsigned int n=0; n<externalDataSpaceValueT_->readSize(); n++)
		if(externalDataSpaceValueT_->isValueChangedAfterClear(n))
			writeValueToTable(n, externalDataSpaceValueT_->readValue(n));

}
