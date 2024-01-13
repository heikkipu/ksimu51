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
#include"NumQTable.h"

NumQTable::NumQTable(QWidget *parent, const char *name, const char *fontFamily, int const fontPointSize) : QTable(parent, name)
{
	QFont table_font(this->font());
	table_font.setFamily(fontFamily);
	table_font.setPointSize(fontPointSize);
	this->setFont(table_font);
	this->setFrameShape(QTable::GroupBoxPanel);

	this->setSelectionMode(QTable::NoSelection);
	
	fontPointSize_=fontPointSize;
	connect(this, SIGNAL(doubleClicked(int, int, int, const QPoint&)), this, SLOT(thisTableMouseDoubleClicked(int, int, int, const QPoint&)));
	connect(this, SIGNAL(currentChanged(int, int)), this, SLOT(thisTableCurrentChanged(int, int)));
	connect(this, SIGNAL(valueChanged(int, int)), this, SLOT(thisTableValueChanged( int, int)));
	
	maxTableValue_=std::numeric_limits<unsigned long >::max();
}

NumQTable::~NumQTable()
{
	columnBaseVec_.clear();
}

void NumQTable::addColumn(int const base, int const numbers)
{
	pToColumnBase_=new ColumnBase(base, numbers);
	columnBaseVec_.push_back(pToColumnBase_);
	setNumCols(numCols()+1);
	QString qStr="DEC";
	int convBase=10;
	switch(base)
	{
		case ColumnBase::HEX:
			qStr="HEX";
			convBase=16;
			break;
		case ColumnBase::OCT:
			qStr="OCT";
			convBase=8;
			break;
		case ColumnBase::BIN:
			qStr="BIN";
			convBase=2;
			this->setColumnReadOnly(numCols()-1, true); //BIN Readonlyksi.
			break;
		default:		//DEC
			qStr="DEC";
			convBase=10;
	};
	unsigned long maxNum=pow(convBase,numbers)-1;
	if(maxNum<maxTableValue_)
		maxTableValue_=maxNum;
	int tl=qStr.length();
	int ns=numbers;
	qStr=qStr.rightJustify((ns-tl)/2+tl, ' ', false);
	qStr=qStr.leftJustify(ns, ' ', false);
	horizontalHeader()->setLabel(numCols()-1, qStr);
	adjustColumn(numCols()-1);
	this->setColumnMovingEnabled(false);
}

void NumQTable::addRow(const QString& name)
{
	int cols=this->numCols();
	if(cols)
	{
		this->setNumRows(this->numRows()+1);
		int rowToAdd=this->numRows()-1;
		this->verticalHeader()->setLabel(rowToAdd, name);
		for(int n=0; n<cols; n++)
		{
			pToNumTableItem_=new MyQTableItem(this, QTableItem::OnTyping, Qt::AlignRight);
			this->setItem(rowToAdd, n, pToNumTableItem_);
		}
		this->setRowMovingEnabled(false);
	}
}

void NumQTable::setValue(int const row, long const value)
{
	int cols=this->numCols();
	if(cols)
		for(int n=0; n<cols;n++)
		{
			switch(columnBaseVec_[n]->base())
			{
				case ColumnBase::HEX:
					setValueHex(row, n, value);
					break;
				case ColumnBase::OCT:
					setValueOct(row, n, value);
					break;
				case ColumnBase::BIN:
					setValueBin(row, n, value);
					break;
				default:		//DEC
					setValueDec(row, n, value);
			}
		}
}
		
void NumQTable::setValueDec(int const row, int const col, long const value)
{
	//DEC
	QString qStr;
	qStr=qStr.setNum(value, 10);
	this->setText(row, col, qStr);
}

void NumQTable::setValueHex(int const row, int const col, long const value)
{
	//HEX
	QString qStr;
	qStr=qStr.setNum(value, 16);
	int ns=columnBaseVec_[col]->numbers();
	if(qStr.length()<ns)
		qStr.rightJustify(ns, '0');
	pToNumTableItem_=dynamic_cast<MyQTableItem*>(this->item(row,col));
	pToNumTableItem_->setText(qStr.upper());
	this->updateCell(row, col);
}

void NumQTable::setValueOct(int const row, int const col, long const value)
{
	QString qStr;
	qStr=qStr.setNum(value, 8);
	this->setText(row, col, qStr);
}

void NumQTable::setValueBin(int const row, int const col, long const value)
{
	//BIN
	QString qStr;
	qStr="";
	int ns=columnBaseVec_[col]->numbers()-1;
	for(int n=ns;n>=0;n--)
	{
		if((value)&(1<<n))
			qStr+="1";
		else
			qStr+="0";
	}
	this->setText(row, col, qStr);
}

void NumQTable::thisTableMouseDoubleClicked(int row, int col, int button, const QPoint& mousePos)
{
	int cBase=columnBaseVec_[col]->base();
	int cNumbers=columnBaseVec_[col]->numbers();
	if(cBase==ColumnBase::BIN)
	{
		int cPos=this->columnPos(col);
		int cWidth=this->columnWidth(col);
		int mousePx=mousePos.x();
		int mousePosXInCell=mousePx-cPos-(cWidth-(cNumbers*(fontPointSize_-2)+2));
		int toggleCharInString=cNumbers;
		if(mousePosXInCell>=0)
			toggleCharInString=mousePosXInCell/cNumbers;
		if(toggleCharInString<cNumbers)
		{
			QString cellStr(this->text(row,col));
			if(cellStr[toggleCharInString]=='0')
				cellStr[toggleCharInString]='1';
			else
				cellStr[toggleCharInString]='0';
			this->setText(row, col, cellStr);
			emit valueChanged(row, col);
		}
	}
}

void NumQTable::thisTableCurrentChanged(int row, int col)
{
	thisTableCurrentCellText_=this->text(row,col);
}

void NumQTable::thisTableValueChanged(int row, int col)
{
	QString qStr=this->text(row,col);
	int nu=columnBaseVec_[col]->numbers();
	long convNum=0;
	bool ok;
	switch(columnBaseVec_[col]->base())
	{
		case ColumnBase::HEX:
			convNum=qStr.toUInt(&ok,16);
			break;
		case ColumnBase::OCT:
			convNum=qStr.toUInt(&ok,8);
			break;
		case ColumnBase::BIN:
			convNum=qStr.toUInt(&ok,2);
			break;
		default:		//DEC
			convNum=qStr.toUInt(&ok,10);
	}
	if((qStr.length()<=nu)&&(convNum<=maxTableValue_)&&ok)
		emit numTableValueChanged(row, col);
	else
		this->setText(row, col, thisTableCurrentCellText_);
}

