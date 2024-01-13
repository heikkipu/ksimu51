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
#include "SFRBitW.h"
#include "I51BaseW.h"

#include <string>
#include <qtooltip.h>


SFRBitW::SFRBitW( QWidget* parent, char const * name, int wflags )
	: ShareMainW( parent, name, wflags )
{
	connect(this,SIGNAL(message_MemoryValueChanged(const QString&, const unsigned long, const int )), this->parent()->parent()->parent(), SLOT(messageFromWorkspaceW_MemoryValueChanged(const QString&, const unsigned long, const int)));
	connect(this, SIGNAL(message_ToggleBreakpointBit(const QString&, const int )), this->parent()->parent()->parent(), SLOT(messageFromWorkspaceW_ToggleBreakpointBit(const QString&, const int )));

	sfrBitTable_ = new QTable(this,"sfrBitTable_");
	QToolTip::add(sfrBitTable_,"Dpl click, toggle bit.\nRight click, toggle breakpoint.");
	setCentralWidget(sfrBitTable_);
	QFont sfrBitTable_font(sfrBitTable_->font());
	sfrBitTable_font.setFamily(FONTFAMILY);
	sfrBitTable_font.setPointSize(10);
	sfrBitTable_->setFont(sfrBitTable_font);
	sfrBitTable_->setFrameShape( QTable::GroupBoxPanel );
	//Columns
	sfrBitTable_->setNumCols(sfrBitTable_->numCols()+1);
	sfrBitTable_->horizontalHeader()->setLabel(sfrBitTable_->numCols()-1, "H");
	sfrBitTable_->adjustColumn(sfrBitTable_->numCols()-1);
	int n;
	QString qStr,apuQStr;
	for(n=0;n<8;n++)
	{
		sfrBitTable_->setNumCols(sfrBitTable_->numCols()+1);
		qStr="   b";
		qStr+=apuQStr.setNum(7-n,16);
		qStr+="  ";
		sfrBitTable_->horizontalHeader()->setLabel(sfrBitTable_->numCols()-1,qStr);
		sfrBitTable_->adjustColumn(sfrBitTable_->numCols()-1);
	}

	sfrBitTable_->setRowMovingEnabled(false);
	sfrBitTable_->setColumnMovingEnabled(false);
	sfrBitTable_->setReadOnly(true);
	sfrBitTable_->setSelectionMode(QTable::NoSelection);

	setGeometry(0, 0,sfrBitTableGeometrySize().width()+20, sfrBitTableGeometrySize().height()+4);

	ledOnIcon_.load ( "pics/KS51hi16BlueLedOn.png", 0, KPixmap::Auto );
	ledOffIcon_.load ( "pics/KS51hi16BlueLedOff.png", 0, KPixmap::Auto );
	breakpointOnIcon_.load ( "pics/KS51hi16RedBreakpointLedOn.png", 0, KPixmap::Auto );
	breakpointOffIcon_.load ( "pics/KS51hi16RedBreakpointLedOff.png", 0, KPixmap::Auto );


	connect(sfrBitTable_, SIGNAL(doubleClicked ( int, int, int, const QPoint&)), this, SLOT(sfrBitTableMouseDoubleClicked(int, int, int, const QPoint&)));
	connect(sfrBitTable_, SIGNAL(clicked ( int, int, int, const QPoint&)), this, SLOT(sfrBitTableMouseClicked(int, int, int, const QPoint&)));

	//Bitin muutos breakpointtiluokka
	bitBreakpoint_=new BitChangeBreakpoint();
}

SFRBitW::~SFRBitW()
{
}

void SFRBitW::removeRegsRows(void)
{
	while(sfrBitTable_->numRows())
	{
		sfrBitTable_->removeRow(sfrBitTable_->numRows() - 1);
	}
}

void SFRBitW::insertRegsRow(unsigned char const  registerAddress, const QString& registerName, const QString& registerBitNames)
{
	QString qStr,apuQStr;
	if(registerAddress)
	{
		qStr="[";
		qStr+=apuQStr.setNum(registerAddress,16);
		qStr+="]";
	}
	qStr+=registerName;
	sfrBitTable_->setNumRows(sfrBitTable_->numRows()+1);
	int rowToAdd=sfrBitTable_->numRows()-1;
	sfrBitTable_->verticalHeader()->setLabel(rowToAdd, qStr.upper());
	int m=0;
	char mk;
	for(int n=0;n<8;n++)
	{
		qStr="";
		mk=registerBitNames[m++];
		while(mk!=';')
		{
			qStr+=mk;
			mk=registerBitNames[m++];
		}
		sfrBitTable_->setText(rowToAdd, 8-n, qStr);
	}
	pToSfrBitTableItem_=new MyQTableItem(sfrBitTable_, QTableItem::OnTyping, Qt::AlignRight, "");
	sfrBitTable_->setItem(rowToAdd, 0, pToSfrBitTableItem_);
}


void SFRBitW::setRegsBitsValue(int const  row, unsigned char const  value)
{
	unsigned int address=convertRowToAddress(row);
	address<<=8;
	for(int n=1;n<=8;n++)
	{
		bool bb=bitBreakpoint_->isBitBreakpointBit(address|(8-n));
		if(value&(0x80>>(n-1)))
		{
			if(bb)
				sfrBitTable_->setPixmap(row, n, breakpointOnIcon_);
			else
				sfrBitTable_->setPixmap(row, n, ledOnIcon_);
		}
		else
		{
			if(bb)
				sfrBitTable_->setPixmap(row, n, breakpointOffIcon_);
			else
				sfrBitTable_->setPixmap(row, n, ledOffIcon_);
		}
	}

	//HEX arvo
	QString qStr;
	qStr.setNum(value,16);
	if(qStr.length()<2)
		qStr.prepend('0');
	pToSfrBitTableItem_=dynamic_cast<MyQTableItem*>(sfrBitTable_->item(row,0));
	pToSfrBitTableItem_->setText(qStr.upper());
	sfrBitTable_->updateCell(row, 0);
}

QSize SFRBitW::sfrBitTableGeometrySize(void)
{
	int n;
	int height=sfrBitTable_->horizontalHeader()-> frameGeometry().height();
	for(n=0;n<sfrBitTable_->numRows();n++)
		height+=sfrBitTable_->rowHeight(n);
	int width=sfrBitTable_->verticalHeader()-> frameGeometry().width();
	for(n=0;n<sfrBitTable_->numCols();n++)
		width+=sfrBitTable_->columnWidth(n);
	return QSize(width,height);
}

bool SFRBitW::toggleBitChangeBreakpoint(unsigned int const bitAddress, bool const currentValue)
{
	return bitBreakpoint_->toggleBreakpoint(bitAddress, currentValue);
}

unsigned int SFRBitW::isAtBitChangeBreakpoint(I8031* const processor)
{
	return bitBreakpoint_->isBreakpointsAtBitChange(processor);
}

bool SFRBitW::showBitChangeBreakpointBit(void)
{
	unsigned int bitAddress=bitBreakpoint_->lastBreakpoint();
	if(bitAddress!=BitChangeBreakpoint::NO_BITCHANGES)
	{
		int row=0, col=8-(bitAddress&0x000f);
		for(int n=0; n<sfrBitTable_->numRows(); n++)
			if(convertRowToAddress(n)==(bitAddress>>8))
			{
				row=n;
				break;
			}
		sfrBitTable_->setCurrentCell(row, 0);
		sfrBitTable_->clearSelection(true);
		sfrBitTable_->selectCells(row, col, row, col);
		sfrBitTable_->ensureCellVisible(row, col);
		return true;
	}
	return false;
}

int SFRBitW::convertRowToAddress(int const row)
{
	//Osoite riviheaderista
	QString qStr("00");
	QString apuQStr(sfrBitTable_->verticalHeader()->label(row));
	qStr=QChar(apuQStr[1]);
	qStr+=QChar(apuQStr[2]);
	bool ok;
	return qStr.toUInt(&ok,16);
}

int SFRBitW::convertColumnToBit(int column)
{
	int rv=0x80;
	rv>>=(column-1);
	return rv;
}

void SFRBitW::sfrBitTableMouseDoubleClicked(int row, int col, int button, const QPoint& mousePos)
{
	int address=convertRowToAddress(row);
	int bitNum=convertColumnToBit(col);
	QString qStr;
	qStr.setNum(bitNum, 16);
	if(qStr.length()<2)
		qStr.prepend('0');
	emit message_MemoryValueChanged(this->name(), address, bitNum);
}

void SFRBitW::sfrBitTableMouseClicked(int row, int col, int button, const QPoint & mousePos)
{
	if(button==Qt::RightButton)
	{
		int address=convertRowToAddress(row);
		int bitNum=convertColumnToBit(col);
		address<<=8;
		address|=(8-col);
		emit message_ToggleBreakpointBit(this->name(), address);
	}
}

QSize SFRBitW::getMaximumSize(void)
{
	return QSize(sfrBitTableGeometrySize().width()+I51BaseW::WIDTH_TO_ADD_WINDOW_GEOMETRY, sfrBitTableGeometrySize().height()+I51BaseW::HEIGHT_TO_ADD_WINDOW_GEOMETRY);

}


void SFRBitW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	//saveFileStream<<(Q_UINT16)X<<(Q_UINT8)KSimu51::QUINTxx<<(Q_UINTxx)Data_xxx_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void SFRBitW::readWorkDataFromFile(QDataStream& readFileStream,
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
