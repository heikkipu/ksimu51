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
#include "I80C32.h"

//Muodostin
I80C32::I80C32():I8032(),I80C31()
{
	processorName_="80C32";
}

//Hajotin
I80C32::~I80C32()
{
}

void I80C32::reset_VirtualPart()
{
	I8032::reset_VirtualPart();      //Kutsutaan kantaluokan resettiä
	I80C31::reset_VirtualPart();      //Kutsutaan toisen kantaluokan resettiä
}

//Ohjaa sarjaportin lähetystä eri moodeissa
//Muutos edelliseen kantaluokkaan: kello ja timerijakajat.
void I80C32::serialPortRxTxHandling(void)
{
	using namespace I80C32_NameS;
	//Määritellään jakajat uusiksi
	//Alku on sama kuin 80C31 luokassa.
	if(internalDataMemory_[PCON].readValue()&PCON_SMOD)
	{
		uart_->setMode1Mode3TimerDivider(16);
		uart_->setMode2ClockDivider(32);
	}
	else
	{
		uart_->setMode1Mode3TimerDivider(32);
		uart_->setMode2ClockDivider(64);
	}
	I8032::serialPortRxTxHandling(); //Kutsutaan kantaluokan ohjelma, tässä 8032
}

void I80C32::saveWorkDataToFile(QDataStream& saveFileStream)
{
	//Käydään kantaluokissa ettei tule overridea
	I8032::saveWorkDataToFile(saveFileStream);
	I80C31::saveWorkDataToFile(saveFileStream);
}

void I80C32::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	//Kantaluokat ettei overridea
	I8032::readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);
	I80C31::readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);
}

