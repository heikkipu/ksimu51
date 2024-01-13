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
#include "MiscW.h"
#include "I51BaseW.h"

#include <string>

MiscW::MiscW( QWidget* parent, char const * name, int wflags )
	: ShareMainW( parent, name, wflags )
{
	connect(this,SIGNAL(message_ValueChanged(const QString&, const unsigned long, const long )), this->parent()->parent()->parent(), SLOT(messageFromWorkspaceW_ValueChanged(const QString&, const unsigned long, const long)));
	
	miscTable_ = new QTable(this,"miscTable_");
	setCentralWidget(miscTable_);
	QFont miscTable_font(miscTable_->font());
	miscTable_font.setFamily(FONTFAMILY);
	miscTable_font.setPointSize(10);
	miscTable_->setFont(miscTable_font);
	miscTable_->setFrameShape( QTable::GroupBoxPanel );

	miscTable_->setNumCols(2);
	miscTable_->horizontalHeader()->setLabel(0,"  HEX|BIN|--- ");
	miscTable_->horizontalHeader()->setLabel(1,"    DEC    ");
	for(int n=0;n<2;n++)
		miscTable_->adjustColumn(n);

	miscTable_->setRowMovingEnabled(false);
	miscTable_->setColumnMovingEnabled(false);
	miscTable_->setSelectionMode(QTable::NoSelection);

	setGeometry (0, 0, miscTableGeometrySize().width()+20, miscTableGeometrySize().height()+4);
	
	connect(miscTable_, SIGNAL(valueChanged(int, int)), this, SLOT(miscTableValueChanged( int, int)));
	connect(miscTable_, SIGNAL(currentChanged(int, int)), this, SLOT(miscTableCurrentChanged(int, int)));
}
 
MiscW::~MiscW()
{
}

void MiscW::remove(void)
{
	while(miscTable_->numRows())
		miscTable_->removeRow(miscTable_->numRows()-1);
}

int MiscW::addRow(const QString& rowName, bool const editable)
{
	//RegName
	miscTable_->setNumRows(miscTable_->numRows()+1);
	int rowToAdd=miscTable_->numRows()-1;
	miscTable_->verticalHeader()->setLabel(rowToAdd, rowName);
	miscTable_->setRowReadOnly(rowToAdd, editable);
	pToMiscTableItem_=new MyQTableItem(miscTable_, QTableItem::OnTyping, Qt::AlignRight, "");
	miscTable_->setItem(rowToAdd, 0, pToMiscTableItem_);
	setGeometry(0, 0, miscTableGeometrySize().width()+20, miscTableGeometrySize().height()+4);
	return rowToAdd;
}


void MiscW::setValue(int const row, unsigned long const value)
{
	QString qStr;
	//HEX
	qStr.setNum(value,16);
	pToMiscTableItem_=dynamic_cast<MyQTableItem*>(miscTable_->item(row,0));
	pToMiscTableItem_->setText(qStr.upper()+"H");
	miscTable_->updateCell(row, 0);
	//DEC
	qStr.setNum(value,10);
	miscTable_->setText(row, 1, qStr);
}

void MiscW::setValue(int const  row, double const  value)
{
	//Liukuluku vain DEC sarakkeeseen.
	QString qStr;
	qStr.setNum(value,'f',9);
	miscTable_->setText(row, 1, qStr);
}

void MiscW::setValue(int const  row, unsigned int const  value, int bitToShow)
{
	QString qStr;
	qStr.setNum(value, 2);
	if(bitToShow>=0)
	{
		if(bitToShow>(qStr.length()-1))
			qStr=qStr.rightJustify(bitToShow+1, '0');
		int place=(qStr.length()-1)-bitToShow;
		qStr.insert(place, '(');
		qStr.insert(place+2, ')');
	}
	pToMiscTableItem_=dynamic_cast<MyQTableItem*>(miscTable_->item(row,0));
	pToMiscTableItem_->setText(qStr+"B");
	miscTable_->updateCell(row, 0);
	
	qStr.setNum((unsigned char)value, 10);
	miscTable_->setText(row, 1, qStr);
}

void MiscW::setValue(int const  row, const QString& value)
{
	pToMiscTableItem_=dynamic_cast<MyQTableItem*>(miscTable_->item(row,0));
	pToMiscTableItem_->setText(value);
	miscTable_->updateCell(row, 0);
}

QSize MiscW::miscTableGeometrySize(void)
{
	int n;
	int height=miscTable_->horizontalHeader()-> frameGeometry().height();
	for(n=0;n<miscTable_->numRows();n++)
		height+=miscTable_->rowHeight(n);
	int width=miscTable_->verticalHeader()-> frameGeometry().width();
	for(n=0;n<miscTable_->numCols();n++)
		width+=miscTable_->columnWidth(n);
	return QSize(width,height);
}

QSize MiscW::getMaximumSize(void)
{
	return QSize(miscTableGeometrySize().width()+I51BaseW::WIDTH_TO_ADD_WINDOW_GEOMETRY, miscTableGeometrySize().height()+I51BaseW::HEIGHT_TO_ADD_WINDOW_GEOMETRY);

}

void MiscW::miscTableValueChanged(int row, int col)
{
	bool ok;
	unsigned long luku;
	//Arvo
	QString messageStr(miscTable_->text(row,col));
	if(col==0)//Heksa
		luku=messageStr.toUInt(&ok,16);
	if(col==1)//Dec
		luku=messageStr.toUInt(&ok,10);
	if(ok)
	{
		messageStr.setNum(luku,16);
		emit message_ValueChanged(this->name(), row, luku);
	}
	else
		miscTable_->setText(row, col, miscTableCurrentCellText_);
}

void MiscW::miscTableCurrentChanged(int row, int col)
{
	miscTableCurrentCellText_=miscTable_->text(row,col);
}

void MiscW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	//saveFileStream<<(Q_UINT16)X<<(Q_UINT8)KSimu51::QUINTxx<<(Q_UINTxx)Data_xxx_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void MiscW::readWorkDataFromFile(QDataStream& readFileStream,
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
