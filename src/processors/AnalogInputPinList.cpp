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
#include "AnalogInputPinList.h"

AnalogInputPinList::AnalogInputPinList(void)
{
	firstPinToAdd_=true; //Pinnilistan aloitus merkki
	changedValues_=false;
}

AnalogInputPinList::~AnalogInputPinList(void)
{
	//delete pinList_;
}

//Lisätään uusi pinni listarakenteeseen.
void AnalogInputPinList::appendPin(string const name, double const  fVal, double const  minVal, double const  maxVal)
{
	AnalogInputPin* pinToAdd;
	pinToAdd = new AnalogInputPin(name, fVal, minVal, maxVal);
	if(firstPinToAdd_) //Jos ensimmäinen kerta kutsuvaan ohjelmaan
	{
		pinList_=new AnalogInputPin();
		pinListLastMember_=new AnalogInputPin();
		pinListLastMember_=pinToAdd;
		pinList_=pinListLastMember_;
		firstPinToAdd_=false;
	}
	else
	{
		pinListLastMember_->nextAnalogInputPin_=pinToAdd;
		pinListLastMember_=pinToAdd;
	}
}


//Asetetaan pinnin arvo ulkoa tarkistaa onko pinni inputti
void AnalogInputPinList::writeValue(string name, double const  fVal)
{
	AnalogInputPin *pinToWrite;
	pinToWrite=pinList_;
	while(pinToWrite!=0)
	{
		string pname=pinToWrite->readName();
		if(pname==name)
		{
			pinToWrite->writeValue(fVal);
			break;
		}
		pinToWrite=pinToWrite->nextAnalogInputPin_;
	}
	changedValues_=true;
}

void AnalogInputPinList::writeValue(int const index, double const  fVal)
{
	AnalogInputPin *pinToWrite;
	pinToWrite=pinList_;
	int indexCount=0;
	while(pinToWrite!=0)
	{
		if(indexCount==index)
		{
			pinToWrite->writeValue(fVal);
			break;
		}
		indexCount++;
		pinToWrite=pinToWrite->nextAnalogInputPin_;
	}
	changedValues_=true;
}

//Sisäinen luku
double AnalogInputPinList::readValue(string name)
{
	AnalogInputPin *pinToRead;
	pinToRead=pinList_;
	while(pinToRead!=0)
	{
		string pname=pinToRead->readName();
		if(pname==name)
			return pinToRead->readValue();
		pinToRead=pinToRead->nextAnalogInputPin_;
	}
	return 0.0;
}

double AnalogInputPinList::readValue(int const index)
{
	AnalogInputPin *pinToRead;
	pinToRead=pinList_;
	int indexCount=0;
	while(pinToRead!=0)
	{
		if(indexCount==index)
			return pinToRead->readValue();
		indexCount++;
		pinToRead=pinToRead->nextAnalogInputPin_;
	}
	return 0.0;
}

int AnalogInputPinList::readIndex(string name)
{
	AnalogInputPin *pinToRead;
	pinToRead=pinList_;
	int indexCount=0;
	while(pinToRead!=0)
	{
		string pname=pinToRead->readName();
		if(pname==name)
			return indexCount;
		indexCount++;
		pinToRead=pinToRead->nextAnalogInputPin_;
	}
	return indexCount;
}

//Ulkoinen luku tietojen päivitystä varten
AnalogInputPin* AnalogInputPinList::readPins(void)
{
	changedValues_=false;
	return pinList_;
}

void AnalogInputPinList::saveWorkDataToFile(QDataStream& saveFileStream)
{
	AnalogInputPin *pinToSave;
	pinToSave=pinList_;
	while(pinToSave!=0)
	{
		saveFileStream<<(double)pinToSave->readValue();
		pinToSave=pinToSave->nextAnalogInputPin_;
	}
}

void AnalogInputPinList::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	AnalogInputPin *pinToRead;
	pinToRead=pinList_;
	while(pinToRead!=0)
	{
		double valueDouble;
		readFileStream>>valueDouble;
		pinToRead->writeValue(valueDouble);
		pinToRead=pinToRead->nextAnalogInputPin_;
	}
}

