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
#include "BitAddressableAreaW.h"
#include "I51BaseW.h"

#include <string>
#include <qtooltip.h>


BitAddressableAreaW::BitAddressableAreaW( QWidget* parent, char const * name, int wflags )
	: ShareMainW(parent, name, wflags)
{
	connect(this,SIGNAL(message_MemoryValueChanged(const QString&, const unsigned long, const int )), this->parent()->parent()->parent(), SLOT(messageFromWorkspaceW_MemoryValueChanged(const QString&, const unsigned long, const int)));
	bitAddressableTable_ = new NumQTable(this,"bitAddressableTable_", FONTFAMILY, 10);
	QToolTip::add(bitAddressableTable_,"Dpl click, toggle bit.");
	setCentralWidget(bitAddressableTable_);

	unsigned int n;
	QString qStr,apuQStr;
	bitAddressableTable_->addColumn(ColumnBase::BIN, 8);

	for(n=I8031_NameS::MIN_BIT_AREA_ADDRESS;n<=I8031_NameS::MAX_BIT_AREA_ADDRESS;n+=bitAddressableTable_->numCols())
	{
		qStr="[";
		qStr+=apuQStr.setNum(n,16);
		qStr+="]";
		bitAddressableTable_->addRow(qStr.upper());
	}

	setGeometry(0, 0, bitAddressableTableGeometrySize().width(), bitAddressableTableGeometrySize().height());
	
	SPMark_.load ( "pics/KS51hi8x16SPMark.png", 0, KPixmap::Auto );
	R0Mark_.load ( "pics/KS51hi8x16R0Mark.png", 0, KPixmap::Auto );
	R1Mark_.load ( "pics/KS51hi8x16R1Mark.png", 0, KPixmap::Auto );
	SPR0Mark_.load("pics/KS51hi8x16SPR0Mark.png", 0, KPixmap::Auto);
	SPR1Mark_.load("pics/KS51hi8x16SPR1Mark.png", 0, KPixmap::Auto);
	R0R1Mark_.load("pics/KS51hi8x16R0R1Mark.png", 0, KPixmap::Auto);
	SPR0R1Mark_.load("pics/KS51hi8x16SPR0R1Mark.png", 0, KPixmap::Auto);

	connect(bitAddressableTable_, SIGNAL(numTableValueChanged(int, int)), this, SLOT(bitAddressableTableValueChanged( int, int)));

	spRow_=0;
	spColumn_=0;
	r0Row_=0;
	r0Column_=0;
	r1Row_=0;
	r1Column_=0;
	isSP_=false;
	isR0_=false;
	isR1_=false;

}

BitAddressableAreaW::~BitAddressableAreaW()
{
}

void BitAddressableAreaW::clear(void)
{
	for(int row=0;row<bitAddressableTable_->numRows();row++)
	{
		for(int column=0;column<bitAddressableTable_->numCols();column++)
		{
			bitAddressableTable_->clearCell(row, column);
			bitAddressableTable_->updateCell(row, column);
		}
	}
}

void BitAddressableAreaW::setValue(unsigned int address, unsigned char value)
{
	if((address>=I8031_NameS::MIN_BIT_AREA_ADDRESS) && (address<=I8031_NameS::MAX_BIT_AREA_ADDRESS))
	{
		int row=address-I8031_NameS::MIN_BIT_AREA_ADDRESS;
		bitAddressableTable_->setValue(row, value);
	}
}

void BitAddressableAreaW::bitAddressableTableValueChanged(int row, int col)
{
	bool ok;
	QString cellStr(bitAddressableTable_->text(row,col));
	int val=cellStr.toUInt(&ok, 2);
	if(ok)
	{
		int addr=I8031_NameS::MIN_BIT_AREA_ADDRESS+(row*bitAddressableTable_->numCols()+col);
		emit message_MemoryValueChanged(this->name(), addr, val);
	}
}

void BitAddressableAreaW::clearPointers(void)
{
	//DPTR
	isSP_=false;
	QString s=bitAddressableTable_->text(spRow_, spColumn_);
	bitAddressableTable_->clearCell(spRow_, spColumn_);
	bitAddressableTable_->setText(spRow_, spColumn_, s);
	//R0
	isR0_=false;
	s=bitAddressableTable_->text(r0Row_, r0Column_);
	bitAddressableTable_->clearCell(r0Row_, r0Column_);
	bitAddressableTable_->setText(r0Row_, r0Column_, s);
	//R1
	isR1_=false;
	s=bitAddressableTable_->text(r1Row_, r1Column_);
	bitAddressableTable_->clearCell(r1Row_, r1Column_);
	bitAddressableTable_->setText(r1Row_, r1Column_, s);
}

void BitAddressableAreaW::writePointerValues(unsigned int const  spV, unsigned char const  r0V, unsigned char const  r1V)
{
	//SP
	if((spV>=I8031_NameS::MIN_BIT_AREA_ADDRESS) && (spV<=I8031_NameS::MAX_BIT_AREA_ADDRESS))
	{
		spRow_=(spV-I8031_NameS::MIN_BIT_AREA_ADDRESS)/bitAddressableTable_->numCols();
		spColumn_=(spV-I8031_NameS::MIN_BIT_AREA_ADDRESS)%bitAddressableTable_->numCols();
		isSP_=true;
		spValue_=spV;
	}
	else
		isSP_=false;
	//R0
	if((r0V>=I8031_NameS::MIN_BIT_AREA_ADDRESS) && (r0V<=I8031_NameS::MAX_BIT_AREA_ADDRESS))
	{
		r0Row_=(r0V-I8031_NameS::MIN_BIT_AREA_ADDRESS)/bitAddressableTable_->numCols();
		r0Column_=(r0V-I8031_NameS::MIN_BIT_AREA_ADDRESS)%bitAddressableTable_->numCols();
		isR0_=true;
		r0Value_=r0V;
	}
	else
		isR0_=false;
	//R1
	if((r1V>=I8031_NameS::MIN_BIT_AREA_ADDRESS) && (r1V<=I8031_NameS::MAX_BIT_AREA_ADDRESS))
	{
		r1Row_=(r1V-I8031_NameS::MIN_BIT_AREA_ADDRESS)/bitAddressableTable_->numCols();
		r1Column_=(r1V-I8031_NameS::MIN_BIT_AREA_ADDRESS)%bitAddressableTable_->numCols();
		isR1_=true;
		r1Value_=r1V;
	}
	else
		isR1_=false;
}

void BitAddressableAreaW::showPointers()
{
	if(isSP_)
		bitAddressableTable_->setPixmap(spRow_, spColumn_, SPMark_);
	if(isR0_)
		bitAddressableTable_->setPixmap(r0Row_, r0Column_, R0Mark_);
	if(isR1_)
		bitAddressableTable_->setPixmap(r1Row_, r1Column_, R1Mark_);
	if(isSP_&&isR0_&&(spValue_==r0Value_))
		bitAddressableTable_->setPixmap(spRow_, spColumn_, SPR0Mark_);
	if(isSP_&&isR1_&&(spValue_==r1Value_))
		bitAddressableTable_->setPixmap(spRow_, spColumn_, SPR1Mark_);
	if(isR0_&&isR1_&&(r0Value_==r1Value_))
		bitAddressableTable_->setPixmap(r0Row_, r0Column_, R0R1Mark_);
	if(isSP_&&isR0_&&isR1_&&(spValue_==r0Value_)&&(spValue_==r1Value_))
		bitAddressableTable_->setPixmap(spRow_, spColumn_, SPR0R1Mark_);
}

void BitAddressableAreaW::setPointers(unsigned int const  spVal, unsigned char const  r0Val, unsigned char const  r1Val)
{
	clearPointers();
	writePointerValues(spVal, r0Val, r1Val);
	showPointers();
}


void BitAddressableAreaW::showPointerSP(void)
{
	if(isSP_)
	{
		setFocus();
		bitAddressableTable_->ensureCellVisible(spRow_, spColumn_);
	}
}
void BitAddressableAreaW::showPointerR0(void)
{
	if(isR0_)
	{
		setFocus();
		bitAddressableTable_->ensureCellVisible(r0Row_, r0Column_);
	}
}

void BitAddressableAreaW::showPointerR1(void)
{
	if(isR1_)
	{
		setFocus();
		bitAddressableTable_->ensureCellVisible(r1Row_, r1Column_);
	}
}

QSize BitAddressableAreaW::bitAddressableTableGeometrySize(void)
{
	int n;
	int height=bitAddressableTable_->horizontalHeader()-> frameGeometry().height();
	for(n=0;n<bitAddressableTable_->numRows();n++)
		height+=bitAddressableTable_->rowHeight(n);
	int width=bitAddressableTable_->verticalHeader()-> frameGeometry().width();
	for(n=0;n<bitAddressableTable_->numCols();n++)
		width+=bitAddressableTable_->columnWidth(n);
	return QSize(width,height);
}

QSize BitAddressableAreaW::getMaximumSize(void)
{
	return QSize(bitAddressableTableGeometrySize().width()+I51BaseW::WIDTH_TO_ADD_WINDOW_GEOMETRY, bitAddressableTableGeometrySize().height()+I51BaseW::HEIGHT_TO_ADD_WINDOW_GEOMETRY);

}

void BitAddressableAreaW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	//saveFileStream<<(Q_UINT16)X<<(Q_UINT8)KSimu51::QUINTxx<<(Q_UINTxx)Data_xxx_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void BitAddressableAreaW::readWorkDataFromFile(QDataStream& readFileStream,
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
