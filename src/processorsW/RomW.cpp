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
#include "RomW.h"
#include "I51BaseW.h"

#include <string>

RomW::RomW( QWidget* parent, char const * name, int wflags, unsigned int const  startAddress, unsigned long const  size)
	: ShareMainW( parent, name, wflags )
{
	romTable_ = new QTable(this,"romTable_");
	setCentralWidget(romTable_);
	QFont romTable_font(romTable_->font());
	romTable_font.setFamily(FONTFAMILY);
	romTable_font.setPointSize(10);
	romTable_->setFont(romTable_font);
	romTable_->setFrameShape( QTable::GroupBoxPanel );
	//Columns
	unsigned int n,m;
	QString qStr,apuQStr;
	romTable_->setNumCols(17);
	for(n=0;n<16;n++)
	{
		qStr=qStr.setNum(n,16);
		if(qStr.length()<2)
			qStr.prepend('0');
		romTable_->horizontalHeader()->setLabel(n,qStr.upper());
		romTable_->adjustColumn (n);
	}
	romTable_->horizontalHeader()->setLabel(16, "      CHAR     ");
	romTable_->adjustColumn (16);

	romTable_->setNumRows(size/16);
	m=0;
	for(n=startAddress;n<(startAddress+size);n+=16)
	{
		qStr=apuQStr.setNum(n,16);
		if(qStr.length()<2)
			qStr.prepend('0');
		if(qStr.length()<3)
			qStr.prepend('0');
		if(qStr.length()<4)
			qStr.prepend('0');
		qStr.prepend('[');
		qStr.append(']');
		romTable_->verticalHeader()->setLabel(m++, qStr.upper());
	}
	romTable_->setRowMovingEnabled(false);
	romTable_->setColumnMovingEnabled(false);
	romTable_->setReadOnly(true);
	romTable_->setSelectionMode(QTable::NoSelection);
	tableRowCounter_=0;

	programCounterIcon_.load("pics/KS51hi8WhiteLedOn.png", 0, KPixmap::Auto );
	prevProgramCounterPointerAddress_=0;
}

RomW::~RomW()
{
}

void RomW::clear(void)
{
	unsigned int n,m;
	for(n=0;n<romTable_->numRows();n++)
	{
		for(m=0;m<16;m++)
			writeValue(n*16+m,0xFF);
	}
}

void RomW::writeValue(unsigned long address, unsigned char value)
{
	int n=address/16; //Row
	int m=address%16; //Column
	QString qStr;
	qStr=qStr.setNum(value, 16);
	if(qStr.length()<2)
		qStr.prepend('0');
	romTable_->setItem(n, m, new MyQTableItem(romTable_, QTableItem::OnTyping, Qt::AlignRight, qStr.upper()));

	qStr=romTable_->text(n, 16);
	if(value>=0x20)
		qStr[m]=char(value);
	else
		qStr[m]='.';
	romTable_->setText(n, 16, qStr);
}

void RomW::focusAddress(unsigned long address)
{
	int r=address/16; //Row
	int c=address%16; //Column
	romTable_->setCurrentCell(r, 16);
	romTable_->clearSelection(true);
	romTable_->selectCells(r, c, r, c);
	romTable_->ensureCellVisible(r, c);
}

void RomW::setProgramCounterPointer(unsigned long const address)
{
	
	int r=prevProgramCounterPointerAddress_/16; //Row
	int c=prevProgramCounterPointerAddress_%16; //Column
	QString cellText=romTable_->text(r, c);
	romTable_->clearCell(r,c);
	romTable_->setText(r, c, cellText);
	
	r=address/16; //Row
	c=address%16; //Column
	romTable_->setPixmap(r, c, programCounterIcon_);
	focusAddress(address);
	prevProgramCounterPointerAddress_=address;
}
QSize RomW::romTableGeometrySize(void)
{
	int n;
	int height=romTable_->horizontalHeader()-> frameGeometry().height();
	for(n=0;n<romTable_->numRows();n++)
		height+=romTable_->rowHeight(n);
	int width=romTable_->verticalHeader()-> frameGeometry().width();
	for(n=0;n<romTable_->numCols();n++)
		width+=romTable_->columnWidth(n);
	return QSize(width,height);
}

QSize RomW::getMaximumSize(void)
{
	return QSize(romTableGeometrySize().width()+I51BaseW::WIDTH_TO_ADD_WINDOW_GEOMETRY, I51BaseW::WINDOW_HEIGHT_MAX);
}


void RomW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	//saveFileStream<<(Q_UINT16)X<<(Q_UINT8)KSimu51::QUINTxx<<(Q_UINTxx)Data_xxx_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void RomW::readWorkDataFromFile(QDataStream& readFileStream,
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
