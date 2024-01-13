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
#include "SfrW.h"
#include "I51BaseW.h"

#include <string>

#include <qtooltip.h>
#include <qlabel.h>

SfrW::SfrW( QWidget* parent, char const * name, int wflags )
	: ShareMainW( parent, name, wflags )
{
	int n;
	QString qStr,apuQStr;
	sfrTable_=new QTable(this,"sfrTable_");
	setCentralWidget(sfrTable_);
	QFont sfrTable_font(  sfrTable_->font() );
	sfrTable_font.setFamily( FONTFAMILY );
	sfrTable_font.setPointSize(10);
	sfrTable_->setFont( sfrTable_font );
	sfrTable_->setFrameShape( QTable::GroupBoxPanel );
	for(n=0;n<8;n++)
	{
		sfrTable_->setNumCols(sfrTable_->numCols()+1);
		qStr="0";
		qStr+=apuQStr.setNum(n,16);;
		sfrTable_->horizontalHeader()->setLabel(sfrTable_->numCols()-1, qStr.upper());
		sfrTable_->adjustColumn(sfrTable_->numCols()-1);
	}
	for(n=0;n<16;n++)
	{
		sfrTable_->setNumRows(sfrTable_->numRows()+1);
		qStr="[";
		qStr+=apuQStr.setNum(0xF8-n*8,16);
		qStr+="]";
		sfrTable_->verticalHeader()->setLabel(sfrTable_->numRows()-1, qStr.upper());
	}
	sfrTable_->setRowMovingEnabled(false);
	sfrTable_->setColumnMovingEnabled(false);
	sfrTable_->setReadOnly(true);
	sfrTable_->setSelectionMode(QTable::NoSelection);

}

SfrW::~SfrW()
{
}

void SfrW::setValue(unsigned int const  address, unsigned char const  value)
{
	QLabel *l;
	if((address>=I8031_NameS::MIN_SFR_ADDRESS) && (address<=I8031_NameS::MAX_SFR_ADDRESS))
	{
		int row=(address-I8031_NameS::MIN_SFR_ADDRESS)/8;
		int column=(address-I8031_NameS::MIN_SFR_ADDRESS)%8;
		QString qStrNum;
		qStrNum=qStrNum.setNum(value, 16);
		if(qStrNum.length()<2)
			qStrNum.prepend('0');
		l=(QLabel *)sfrTable_->cellWidget(sfrTable_->numRows()-1-row, column);
		l->setText(qStrNum.upper());
	}
}

//Voidaan laittaa rekisterin nimi ToolTippinä taulukkoon.
void SfrW::setRegisterName(unsigned int const  address, const QString& rName)
{
	if((address>=I8031_NameS::MIN_SFR_ADDRESS) && (address<=I8031_NameS::MAX_SFR_ADDRESS))
	{
		int row=(address-I8031_NameS::MIN_SFR_ADDRESS)/8;
		int column=(address-I8031_NameS::MIN_SFR_ADDRESS)%8;
		QLabel * l=new QLabel("Xx",sfrTable_,rName,0);
		QToolTip::add(l,rName);
		sfrTable_->setCellWidget(sfrTable_->numRows()-1-row, column, l);
	}
}


QSize SfrW::sfrTableGeometrySize(void)
{
	int n;
	int height=sfrTable_->horizontalHeader()-> frameGeometry().height();
	for(n=0;n<sfrTable_->numRows();n++)
		height+=sfrTable_->rowHeight(n);
	int width=sfrTable_->verticalHeader()-> frameGeometry().width();
	for(n=0;n<sfrTable_->numCols();n++)
		width+=sfrTable_->columnWidth(n);
	return QSize(width,height);
}

QSize SfrW::getMaximumSize(void)
{
	return QSize(sfrTableGeometrySize().width()+I51BaseW::WIDTH_TO_ADD_WINDOW_GEOMETRY, sfrTableGeometrySize().height()+I51BaseW::HEIGHT_TO_ADD_WINDOW_GEOMETRY);
}


void SfrW::saveWorkDataToFile(QDataStream& saveFileStream)
{
	//saveFileStream<<(Q_UINT16)X<<(Q_UINT8)KSimu51::QUINTxx<<(Q_UINTxx)Data_xxx_;
	saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;
}

void SfrW::readWorkDataFromFile(QDataStream& readFileStream,
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
