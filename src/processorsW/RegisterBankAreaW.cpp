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
#include "RegisterBankAreaW.h"
#include "I51BaseW.h"

#include <string>

RegisterBankAreaW::RegisterBankAreaW( QWidget* parent, char const * name, int wflags )
	: InternalRAMW(parent, name, wflags, I8031_NameS::MIN_REGISTER_BANK_ADDRESS, I8031_NameS::REGISTER_BANK_SIZE, false)
{
	Null8x16Icon_.load ( "pics/KS51hi8x16Null.png", 0, KPixmap::Auto );
	unsigned int ro, co;
	QString qStr,apuQStr;
	for(co=0; co<8; co++)
	{
		qStr="R";
		qStr+=apuQStr.setNum(co, 16);
		internalRAMTable_->horizontalHeader()->setLabel(co, qStr);
		internalRAMTable_->adjustColumn(co);
	}
	int m=0;
	for(ro=I8031_NameS::MIN_REGISTER_BANK_ADDRESS; ro<I8031_NameS::MAX_REGISTER_BANK_ADDRESS; ro+=8)
	{
		qStr="[";
		apuQStr=apuQStr.setNum(ro,16);
		if(apuQStr.length()<2)
			apuQStr.prepend('0');
		qStr+=apuQStr.upper();
		qStr+="]";
		qStr+="RB";
		qStr+=apuQStr.setNum(ro/8, 16);
		internalRAMTable_->verticalHeader()->setLabel(m++, Null8x16Icon_, qStr);
	}

	RBMark_.load ( "pics/KS51hi8x16RBMark.png", 0, KPixmap::Auto );
	previousActiveRagisterRow_=0;
}

RegisterBankAreaW::~RegisterBankAreaW()
{
}

void RegisterBankAreaW::selectActiveRegisterBankRow(int const  row)
{
	if(internalRAMTable_->numRows()>=row)
	{
		internalRAMTable_->verticalHeader()->setLabel(previousActiveRagisterRow_, Null8x16Icon_, internalRAMTable_->verticalHeader()->label(previousActiveRagisterRow_));
		internalRAMTable_->verticalHeader()->setLabel(row, RBMark_, internalRAMTable_->verticalHeader()->label(row));
		previousActiveRagisterRow_=row;
	}
}
