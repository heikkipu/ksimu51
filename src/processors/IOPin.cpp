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
#include "IOPin.h"

IOPin::IOPin(void)
{
	setDefaultValues();
}

IOPin::IOPin(string const pinName, IOPin::PinID const  id, PinTYPE const pinType)
{
	setDefaultValues();
	name_=pinName;
	id_=id;
	switch(pinType)
	{
		case PINTYPE_INPUT:
			direction_=false;
			bidir_=false;
			break;
		case PINTYPE_OUTPUT:
			direction_=true;
			bidir_=false;
			break;
		case PINTYPE_BIDIR_LEFT:
			direction_=false; //Jos vasemmalla oletus inputiksi.
			bidir_=true;
			bidirLeftRight_=true;
			break;
		case PINTYPE_BIDIR_RIGHT:
			direction_=true; //Jos oikeella asettaa oletuksen outputiksi.
			bidir_=true;
			bidirLeftRight_=false;
			break;
		default:
			direction_=false;
			bidir_=false;
	}
}


IOPin::IOPin(IOPin* sourcePin)
{
	setDefaultValues();
	name_=sourcePin->readName();
	id_=sourcePin->readID();
	direction_=sourcePin->isOut(); //outputti on true eli suoraan verrannollinen.
	bidir_=sourcePin->isBidir(bidirLeftRight_);
	hizState_=sourcePin->isInHizState();
	value_=sourcePin->readValue(false);
}

void IOPin::setDefaultValues(void)
{
	NO_ID_.id_=-1;
	name_="PIN";
	nextPin_=0;
	id_=NO_ID();
	direction_=false; //Oletus inputti
	bidir_=false;
	bidirLeftRight_=true; //Oletus vasemmalla
	value_=false;
	hizState_=false;
	changed_=false;
}

IOPin::~IOPin(void)
{
}

bool IOPin::readValue(bool const  clearFlag)
{
	if(clearFlag)
		changed_=false;
	return value_;
}

void IOPin::writeValue(bool const  pinValue, bool const  setFlag)
{
	if(value_!=pinValue)
	{
		value_=pinValue;
		if(setFlag)
			changed_=true;
	}
}

void IOPin::changeID(IOPin::PinID const  id)
{
	id_=id;
}

bool IOPin::isValueChanged(void)
{
	return changed_;
}

const IOPin::PinTYPE IOPin::readType(void)
{
	if(!bidir_&&!direction_)
		return PINTYPE_INPUT;
	if(!bidir_&&direction_)
		return PINTYPE_OUTPUT;
	if(bidir_&&bidirLeftRight_)
		return PINTYPE_BIDIR_LEFT;
	if(bidir_&&!bidirLeftRight_)
		return PINTYPE_BIDIR_RIGHT;
	return PINTYPE_ZERO;
}

bool IOPin::isOut(void)
{
	return direction_;
}

bool IOPin::isIn(void)
{
	return !direction_;
}

bool IOPin::isBidir(bool& leftRight)
{
	leftRight=bidirLeftRight_;
	return bidir_;
}

bool IOPin::setDirection(bool const  dir)
{
	if(bidir_)
	{
		direction_=dir;
		return true;
	}
	return false;
}

bool IOPin::isInHizState(void)
{
	return hizState_;
}

void IOPin::setHizState(bool const  state)
{
	hizState_=state;
}

IOPin* IOPin::next(void)
{
	return nextPin_;
}

void IOPin::setNextPin(IOPin* nextP)
{
	nextPin_=nextP;
}


