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
#include "IspGAL22V10W.h"

IspGAL22V10W::IspGAL22V10W(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType) : Gal22V10W(parent, name, wflags, componentNumber, componentType)
{
	componentName_.setNum(componentNumber,10);
	componentName_+=" ispGAL22V10 [";

	delayTimeTyp_=DELAYTIMEISPGAL22V10;
	delayTimeTypNumInput_->setValue(delayTimeTyp_*1.0e9);
}

IspGAL22V10W::~IspGAL22V10W()
{
}


void IspGAL22V10W::update_VirtualPart(void)
{
	Gal22V10W::update_VirtualPart();
}

double IspGAL22V10W::execute_VirtualPart(double executeTime)
{
	double retVal=executeTime+1.0;
	retVal=Gal22V10W::execute_VirtualPart(executeTime);
	return retVal;
}

void IspGAL22V10W::saveWorkDataToFile(QDataStream& saveFileStream)
{
	Gal22V10W::saveWorkDataToFile(saveFileStream);
}

void IspGAL22V10W::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	Gal22V10W::readWorkDataFromFile(readFileStream, funcHandleUnknownIndexRead);
}

