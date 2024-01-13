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
#include "AnalogInputPin.h"


AnalogInputPin::AnalogInputPin(void)
{
	name_="AIN";
	minValue_=0.0;
	maxValue_=5.0;
	value_=0.0;
	nextAnalogInputPin_=0;
}


AnalogInputPin::AnalogInputPin(string pName, double const  fV, double const  minV, double const  maxV)
{
	name_=pName;
	minValue_=minV;
	maxValue_=maxV;
	value_=fV;
	nextAnalogInputPin_=0;
}

AnalogInputPin::~AnalogInputPin(void)
{
}

double AnalogInputPin::readValue(void)
{
	return value_;
}

double AnalogInputPin::readMin(void)
{
	return minValue_;
}

double AnalogInputPin::readMax(void)
{
	return maxValue_;
}

void AnalogInputPin::writeValue(double const  fV)
{
	if(fV<minValue_)
		value_=minValue_;
	else
		if(fV>maxValue_)
			value_=maxValue_;
		else
			value_=fV;
}

string AnalogInputPin::readName(void)
{
	return name_;
}
