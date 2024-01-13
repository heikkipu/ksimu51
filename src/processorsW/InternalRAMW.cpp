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
#include "InternalRAMW.h"
#include "I51BaseW.h"

#include <string>

InternalRAMW::InternalRAMW( QWidget* parent, char const * name, int wflags, unsigned int const startAddress, unsigned int const size, bool const  charColumn)
	: ShareMainW( parent, name, wflags )
{
	//Konnektointi tehty tässä progresbaarin vuoksi
	connect(this,SIGNAL(message_MemoryValueChanged(const QString&, const unsigned long, const int )), this->parent()->parent()->parent(), SLOT(messageFromWorkspaceW_MemoryValueChanged(const QString&, const unsigned long, const int)));

	iRAMStartAddress_=startAddress;
	iRAMSize_=size;
	charColumnOn_=charColumn;

	internalRAMTable_ = new QTable(this,"internalRAMTable_");
	setCentralWidget(internalRAMTable_);
	QFont internalRAMTable_font(internalRAMTable_->font());
	internalRAMTable_font.setFamily(FONTFAMILY);
	internalRAMTable_font.setPointSize(10);
	internalRAMTable_->setFont(internalRAMTable_font);
	internalRAMTable_->setFrameShape( QTable::GroupBoxPanel );

	//Columns
	unsigned int ro,co, m;
	QString qStr;
	if(charColumnOn_)
	{
		internalRAMTable_->setNumCols(9);
		internalRAMTable_->horizontalHeader()->setLabel(8, "  CHAR ");
		internalRAMTable_->adjustColumn(8);
	}
	else
		internalRAMTable_->setNumCols(8);
	for(co=0; co<8; co++)
	{
		qStr=qStr.setNum(co, 16);
		qStr.prepend('0');
		internalRAMTable_->horizontalHeader()->setLabel(co, qStr);
		internalRAMTable_->adjustColumn(co);
	}
	//Rows
	internalRAMTable_->setNumRows(iRAMSize_/8);
	ro=0;
	for(m=iRAMStartAddress_; m<(iRAMStartAddress_+iRAMSize_); m+=8)
	{
		qStr=qStr.setNum(m, 16);
		if(qStr.length()<2)
			qStr.prepend('0');
		if((iRAMStartAddress_+iRAMSize_-1)>0xFF)
		{
			if(qStr.length()<3)
				qStr.prepend('0');
			if(qStr.length()<4)
				qStr.prepend('0');
		}
		qStr.prepend('[');
		qStr.append(']');
		internalRAMTable_->verticalHeader()->setLabel(ro, qStr.upper());
		for(co=0; co<8; co++)
		{
			pToInternalRAMTableItem_=new MyQTableItem(internalRAMTable_, QTableItem::OnTyping, Qt::AlignRight, "");
			internalRAMTable_->setItem(ro, co, pToInternalRAMTableItem_);
		}
		ro++;
	}

	internalRAMTable_->setRowMovingEnabled(false);
	internalRAMTable_->setColumnMovingEnabled(false);
	internalRAMTable_->setReadOnly(false);
	internalRAMTable_->setSelectionMode(QTable::Multi);
	if(charColumnOn_)
		internalRAMTable_->setColumnReadOnly(8, true); //Char sarake readonlyksi

	pointerSPMark_.load( "pics/KS51hi8x16SPMark.png", 0, KPixmap::Auto );
	registerR0Mark_.load( "pics/KS51hi8x16R0Mark.png", 0, KPixmap::Auto );
	registerR1Mark_.load( "pics/KS51hi8x16R1Mark.png", 0, KPixmap::Auto );
	pointerSPRegisterR0Mark_.load("pics/KS51hi8x16SPR0Mark.png", 0, KPixmap::Auto);
	pointerSPRegisterR1Mark_.load("pics/KS51hi8x16SPR1Mark.png", 0, KPixmap::Auto);
	registerR0RegisterR1Mark_.load("pics/KS51hi8x16R0R1Mark.png", 0, KPixmap::Auto);
	pointerSPRegisterR0RegisterR1Mark_.load("pics/KS51hi8x16SPR0R1Mark.png", 0, KPixmap::Auto);

	connect(internalRAMTable_, SIGNAL(valueChanged (int, int)), this, SLOT(internalRAMTableValueChanged( int, int)));
	connect(internalRAMTable_, SIGNAL(currentChanged (int, int)), this, SLOT(internalRAMTableCurrentChanged( int, int)));

	pointerSPRow_=0;
	pointerSPColumn_=0;
	registerR0Row_=0;
	registerR0Column_=0;
	registerR1Row_=0;
	registerR1Column_=0;
	isPointerSP_=false;
	isRegisterR0_=false;
	isRegisterR1_=false;

}


InternalRAMW::~InternalRAMW()
{
}

void InternalRAMW::clear(void)
{
	for(int row=0;row<internalRAMTable_->numRows();row++)
	{
		for(int column=0;column<internalRAMTable_->numCols();column++)
			setText(row, column, "");
		if(charColumnOn_)
			internalRAMTable_->setText(row,8,"........");
	}
}

void InternalRAMW::setValue(unsigned int address, unsigned char value)
{
	if((address>=iRAMStartAddress_) && (address<(iRAMStartAddress_+iRAMSize_)))
	{
		int row=(address-iRAMStartAddress_)/8;
		int column=(address-iRAMStartAddress_)%8;
		QString qStr;
		qStr=qStr.setNum(value, 16);
		if(qStr.length()<2)
			qStr.prepend('0');
		setText(row, column, qStr.upper());
		if(charColumnOn_)
		{
			qStr=internalRAMTable_->text(row, 8);
			if(value>=0x20)
				qStr[column]=char(value);
			else
				qStr[column]='.';
			internalRAMTable_->setText(row, 8, qStr);
		}
	}
}

void InternalRAMW::setText(int const row, int const column, const QString &text)
{
	pToInternalRAMTableItem_=dynamic_cast<MyQTableItem*>(internalRAMTable_->item(row, column));
	if(pToInternalRAMTableItem_)
	{
		pToInternalRAMTableItem_->setText(text);
		internalRAMTable_->updateCell(row, column);
	}
}

void InternalRAMW::setPixmap(int const row, int const column, const QPixmap &pix)
{
	pToInternalRAMTableItem_=dynamic_cast<MyQTableItem*>(internalRAMTable_->item(row, column));
	if(pToInternalRAMTableItem_)
	{
		pToInternalRAMTableItem_->setPixmap(pix);
		internalRAMTable_->updateCell(row, column);
	}
}

void InternalRAMW::clearPointers(void)
{
	//DPTR
	isPointerSP_=false;
	setPixmap(pointerSPRow_, pointerSPColumn_, nullIcon_);
	//R0
	isRegisterR0_=false;
	setPixmap(registerR0Row_, registerR0Column_, nullIcon_);
	//R1
	isRegisterR1_=false;
	setPixmap(registerR1Row_, registerR1Column_, nullIcon_);
}

void InternalRAMW::writePointerValues(unsigned int const  pointerSPV, unsigned char const  registerR0V, unsigned char const  registerR1V)
{
	//SP
	if((pointerSPV>=iRAMStartAddress_) && (pointerSPV<(iRAMStartAddress_+iRAMSize_)))
	{
		pointerSPValue_=pointerSPV;
		pointerSPRow_=(pointerSPValue_-iRAMStartAddress_)/8;
		pointerSPColumn_=(pointerSPValue_-iRAMStartAddress_)%8;
		isPointerSP_=true;
	}
	else
		isPointerSP_=false;
	//R0
	if((registerR0V>=iRAMStartAddress_) && (registerR0V<(iRAMStartAddress_+iRAMSize_)))
	{
		registerR0Value_=registerR0V;
		registerR0Row_=(registerR0Value_-iRAMStartAddress_)/8;
		registerR0Column_=(registerR0Value_-iRAMStartAddress_)%8;
		isRegisterR0_=true;
	}
	else
		isRegisterR0_=false;
	//R1
	if((registerR1V>=iRAMStartAddress_) && (registerR1V<(iRAMStartAddress_+iRAMSize_)))
	{
		registerR1Value_=registerR1V;
		registerR1Row_=(registerR1Value_-iRAMStartAddress_)/8;
		registerR1Column_=(registerR1Value_-iRAMStartAddress_)%8;
		isRegisterR1_=true;
	}
	else
		isRegisterR1_=false;
}

void InternalRAMW::showPointers()
{
	if(isPointerSP_)
		setPixmap(pointerSPRow_, pointerSPColumn_, pointerSPMark_);
	if(isRegisterR0_)
		setPixmap(registerR0Row_, registerR0Column_, registerR0Mark_);
	if(isRegisterR1_)
		setPixmap(registerR1Row_, registerR1Column_, registerR1Mark_);
	if(isPointerSP_&&isRegisterR0_&&(pointerSPValue_==registerR0Value_))
		setPixmap(pointerSPRow_, pointerSPColumn_, pointerSPRegisterR0Mark_);
	if(isPointerSP_&&isRegisterR1_&&(pointerSPValue_==registerR1Value_))
		setPixmap(pointerSPRow_, pointerSPColumn_, pointerSPRegisterR1Mark_);
	if(isRegisterR0_&&isRegisterR1_&&(registerR0Value_==registerR1Value_))
		setPixmap(registerR0Row_, registerR0Column_, registerR0RegisterR1Mark_);
	if(isPointerSP_&&isRegisterR0_&&isRegisterR1_&&(pointerSPValue_==registerR0Value_)&&(pointerSPValue_==registerR1Value_))
		setPixmap(pointerSPRow_, pointerSPColumn_, pointerSPRegisterR0RegisterR1Mark_);
}

void InternalRAMW::setPointers(unsigned int const  pointerSPVal, unsigned char const  registerR0Val, unsigned char const  registerR1Val)
{
	clearPointers();
	writePointerValues(pointerSPVal, registerR0Val, registerR1Val);
	showPointers();
}

void InternalRAMW::showPointerSP(void)
{
	if(isPointerSP_)
	{
		setFocus();
		internalRAMTable_->ensureCellVisible(pointerSPRow_, pointerSPColumn_);
	}
}

void InternalRAMW::showPointerR0(void)
{
	if(isRegisterR0_)
	{
		setFocus();
		internalRAMTable_->ensureCellVisible(registerR0Row_, registerR0Column_);
	}
}

void InternalRAMW::showPointerR1(void)
{
	if(isRegisterR1_)
	{
		setFocus();
		internalRAMTable_->ensureCellVisible(registerR1Row_, registerR1Column_);
	}
}

QSize InternalRAMW::internalRAMTableGeometrySize(void)
{
	int n;
	int height=internalRAMTable_->horizontalHeader()-> frameGeometry().height();
	for(n=0;n<internalRAMTable_->numRows();n++)
		height+=internalRAMTable_->rowHeight(n);
	int width=internalRAMTable_->verticalHeader()-> frameGeometry().width();
	for(n=0;n<internalRAMTable_->numCols();n++)
		width+=internalRAMTable_->columnWidth(n);
	return QSize(width,height);
}

void InternalRAMW::internalRAMTableValueChanged(int row, int col)
{
	unsigned int address=iRAMStartAddress_+(row*8+col);
	QString messageStr(internalRAMTable_->text(row,col));
	QString valQStr("xx");
	if((messageStr.length()==1))
	{
		valQStr="0";
		valQStr+=QChar(messageStr[0]);
	}
	else
	{
		if(messageStr.length()==2)
		{
			valQStr=QChar(messageStr[0]);
			valQStr+=QChar(messageStr[1]);
		}
	}
	bool change_value,only_clear=false;
	unsigned int ui=valQStr.toUInt(&change_value,16); //Onko heksaluku
	if(valQStr=="0 ")   //Jos oli syötetty välilyönti tyhjätään alkio/alkiot
		only_clear=true;
	if(change_value||only_clear)
	{
		//Katotaan valitut ja täytetään merkeillä.
		unsigned int rows=internalRAMTable_->numRows();
		for(int n=0;n<rows;n++)
		{
			if(internalRAMTable_->isRowSelected(n, false)||(n==row))
			{
				for(int m=0;m<8;m++)//Char columnia ei oteta huomioon
				{
					if(internalRAMTable_->isSelected(n, m)||(m==col))
					{
						if(change_value&&!only_clear)
						{
							address=iRAMStartAddress_+(n*8+m);
							emit message_MemoryValueChanged(this->name(), address, ui);
						}
						if(only_clear)
							internalRAMTable_->setText(n, m, "");
					}
				}
			}
		}
	}
	else
	{
		internalRAMTable_->setText(row, col, internalRAMTableCurrentCellText_);
	}
}

void InternalRAMW::internalRAMTableCurrentChanged(int row, int col)
{
	internalRAMTableCurrentCellText_=internalRAMTable_->text(row,col);
}

QSize InternalRAMW::getMaximumSize(void)
{
	return QSize(internalRAMTableGeometrySize().width()+I51BaseW::WIDTH_TO_ADD_WINDOW_GEOMETRY, internalRAMTableGeometrySize().height()+I51BaseW::HEIGHT_TO_ADD_WINDOW_GEOMETRY);
}


void InternalRAMW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	//saveFileStream<<(Q_UINT16)X<<(Q_UINT8)KSimu51::QUINTxx<<(Q_UINTxx)Data_xxx_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void InternalRAMW::readWorkDataFromFile(QDataStream& readFileStream,
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
