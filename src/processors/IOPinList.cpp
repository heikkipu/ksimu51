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
#include "IOPinList.h"
#include "../ksimu51.h"


IOPinList::IOPinList(void)
{
	NO_INDEX_.index_=-1; //Laitetaan vakiolle arvo
	firstPinToAdd_=true; //Pinnilistan aloitus merkki
	numPins_=0;
	someInPinChanged_=false; //Merkki jonkun inputpinnin tilan muuttumisesta. Voidaan käyttää updateissa.
}

IOPinList::~IOPinList(void)
{
	//delete pinListFirstMember_;
}

IOPinList::IOPinList(IOPin* sourceFirstPin)
{
	bool firstPin=true;
	IOPin* sourPin=sourceFirstPin;
	IOPin* pinToAdd= new IOPin(sourPin);
	numPins_=0;
	while(sourPin!=0)
	{
		if(firstPin)
		{
			pinListFirstMember_=new IOPin();
			pinListLastMember_=new IOPin();
			pinListLastMember_=pinToAdd;
			pinListFirstMember_=pinListLastMember_;
			firstPin=false;
		}
		else
		{
			pinToAdd = new IOPin(sourPin);
			pinListLastMember_->setNextPin(pinToAdd);
			pinListLastMember_=pinToAdd;
		}
		sourPin=sourPin->next();
		numPins_++;
	}
}

void IOPinList::operator=(const IOPinList* l)
{
	IOPin* sourcePin=l->pinListFirstMember_;
	IOPin* destinationPin=this->pinListFirstMember_;
	while((sourcePin!=0)&&(destinationPin!=0))
	{
		destinationPin->changeID(sourcePin->readID());
		destinationPin->writeValue(sourcePin->readValue(false), true);
		sourcePin=sourcePin->next();
		destinationPin=destinationPin->next();
	}
}

void IOPinList::copyListValues(IOPinList* sourcePinList)
{
	IOPin* sourPin=sourcePinList->pToFirstPin();
	IOPin* destPin=this->pToFirstPin();
	while(destPin!=0)
	{
		destPin->writeValue(sourPin->readValue(true), true);
		if(sourPin->next())
			sourPin=sourPin->next();
		else
			break;
		destPin=destPin->next();
	}
}


//Pointteri eka pinniin
IOPin* IOPinList::pToFirstPin(void)
{
	if(numPins_)
		return pinListFirstMember_;
	return 0;
}

//Pointteri pinnin nimellä. Palauttaa 0 jos ei onnaa
IOPin* IOPinList::pToPin(string const name)
{
	IOPin* apuPin=pinListFirstMember_;
	while(apuPin!=0)
	{
		if(apuPin->readName()==name)
			return apuPin;
		apuPin=apuPin->next();
	}
	return 0;
}

//Pointteri ID. Palauttaa 0 jos ei onnaa.
IOPin* IOPinList::pToPin(IOPin::PinID const id)
{
	IOPin::PinID pid;
	IOPin* pinP=pinListFirstMember_;
	while(pinP!=0)
	{
		pid=pinP->readID();
		if(pid.id_==id.id_)
			return pinP;
		pinP=pinP->next();
	}
	return 0;
}

//Pointteri indexillä. Palauttaa 0 jos ei onnaa.
IOPin* IOPinList::pToPin(IOPinList::PinIndex const index)
{
	if(index.index_<numPins_)
	{
		IOPin *pinP=pinListFirstMember_;
		int indexCount;
		for(indexCount=0; indexCount<index.index_; indexCount++)
			pinP=pinP->next();
		return pinP;
	}
	return 0;
} 

IOPinList::PinIndex IOPinList::readIndex(string const name)
{
	IOPin* pin=pinListFirstMember_;
	IOPinList::PinIndex indexCount;
	indexCount.index_=0;
	while(pin!=0)
	{
		string pName=pin->readName();
		if(pName==name)
			return indexCount;
		indexCount.index_++;
		pin=pin->next();
	}
	indexCount.index_=-1;
	return indexCount;
}

IOPin::PinID IOPinList::readID(string const name)
{
	IOPin* idPin=pinListFirstMember_;
	while(idPin!=0)
	{
		if(idPin->readName()==name)
			return idPin->readID();
		idPin=idPin->next();
	}
	return idPin->NO_ID();
}

//Lukee ID:n indexillä.
IOPin::PinID IOPinList::readID(IOPinList::PinIndex const index)
{
	IOPin* idPin=pinListFirstMember_;
	int indexCount=0;
	while(idPin!=0)
	{
		if(indexCount==index.index_)
			return idPin->readID();
		indexCount++;
		idPin=idPin->next();
	}
	return idPin->NO_ID();
} 

//Lisätään uusi pinni listarakenteeseen. Pinnin indeksi kasvaa aina yhdellä listan määrällä
void IOPinList::addPin(string const name, bool const value, IOPin::PinTYPE const pinType)
{
	IOPin::PinID pid;pid.id_=numPins_;
	addPin(pid, name, value, pinType);
}

void IOPinList::addPin(IOPin::PinID const  id, string const name, bool const  value, IOPin::PinTYPE const pinType)
{
	IOPin* pinToAdd;
	pinToAdd = new IOPin(name, id, pinType);
	if(firstPinToAdd_) //Jos ensimmäinen kerta kutsuvaan ohjelmaan
	{
		pinListFirstMember_=new IOPin();
		pinListLastMember_=new IOPin();
		pinListLastMember_=pinToAdd;
		pinListFirstMember_=pinListLastMember_;
		firstPinToAdd_=false;
	}
	else
	{
		pinListLastMember_->setNextPin(pinToAdd);
		pinListLastMember_=pinToAdd;
	}
	pinListLastMember_->writeValue(value, false);
	numPins_++;
}

//Asetetaan pinnin arvo prosesorin sisältä ei tarkista suuntaa
void IOPinList::setPin(string const name, bool const  pinValue, bool const  markChanged)
{
	IOPin *pinToSet;
	pinToSet=pinListFirstMember_;
	while(pinToSet!=0)
	{
		string pname=pinToSet->readName();
		if(pname==name)
		{
			pinToSet->writeValue(pinValue, markChanged);
			break;
		}
		pinToSet=pinToSet->next();
	}
}

//Asetetaan pinnin arvo prosesorin sisältä id:llä. Ei tarkista suuntaa.
void IOPinList::setPin(IOPin::PinID const id, bool const pinValue, bool const markChanged)
{
	IOPin *pinToSet;
	pinToSet=pinListFirstMember_;
	while(pinToSet!=0)
	{
		IOPin::PinID pId=pinToSet->readID();
		if(pId.id_==id.id_)
		{
			pinToSet->writeValue(pinValue, markChanged);
			break;
		}
		pinToSet=pinToSet->next();
	}
}

//Asetetaan pinnin arvo ulkoa tarkistaa onko pinni inputti
void IOPinList::writeInputPin(string const name, bool const  pinValue, bool const  markChanged)
{
	IOPin *pinToWrite;
	pinToWrite=pinListFirstMember_;
	while(pinToWrite!=0)
	{
		string pname=pinToWrite->readName();
		if(pname==name)
		{
			if(pinToWrite->isIn())
			{
				if(pinToWrite->readValue(false)!=pinValue)
					someInPinChanged_=true;
				pinToWrite->writeValue(pinValue, markChanged);
			}
			break;
		}
		pinToWrite=pinToWrite->next();
	}
}

//Asetetaan pinnin arvo ulkoa tarkistaa onko pinni inputti
void IOPinList::writeInputPin(IOPin* pointerToPin, bool const  pinValue, bool const  markChanged)
{
	if(pointerToPin->isIn())
	{
		if(pointerToPin->readValue(false)!=pinValue)
			someInPinChanged_=true;
		pointerToPin->writeValue(pinValue, markChanged);
	}

}

//Asetetaan yksittäinen pinnin suunnan nimen mukaan jos bidir
bool IOPinList::setDirection(string const name, bool const  dir)
{
	IOPin *pinToSetDir;
	pinToSetDir=pinListFirstMember_;
	while(pinToSetDir!=0)
	{
		string pname=pinToSetDir->readName();
		if(pname==name)
			return pinToSetDir->setDirection(dir);
		pinToSetDir=pinToSetDir->next();
	}
	return false;
}

//Asetetaan yksittäinen pinnin suunnan indeksin mukaan jos bidir
bool IOPinList::setDirection(IOPin::PinID const id, bool const  dir)
{
	IOPin *pinToSetDir;
	pinToSetDir=pinListFirstMember_;
	while(pinToSetDir!=0)
	{
		IOPin::PinID pId=pinToSetDir->readID();
		if(pId.id_==id.id_)
			return pinToSetDir->setDirection(dir);
		pinToSetDir=pinToSetDir->next();
	}
	return false;
}

bool IOPinList::ifSomeInPinChanged(bool const  markChecked)
{
	bool retVal=someInPinChanged_;
	if(markChecked)
		someInPinChanged_=false;
	return retVal;
}

bool IOPinList::readValue(string const name, bool const  markRead)
{
	IOPin *pinToRead;
	pinToRead=pinListFirstMember_;
	while(pinToRead!=0)
	{
		string pname=pinToRead->readName();
		if(pname==name)
			return pinToRead->readValue(markRead);
		pinToRead=pinToRead->next();
	}
	return false;
}

bool IOPinList::readValue(IOPin::PinID const id, bool const  markRead)
{
	IOPin *pinToRead;
	pinToRead=pinListFirstMember_;
	while(pinToRead!=0)
	{
		IOPin::PinID pId=pinToRead->readID();
		if(pId.id_==id.id_)
			return pinToRead->readValue(markRead);
		pinToRead=pinToRead->next();
	}
	return false;
}

//Luetaan pinnin arvo indexillä
bool IOPinList::readValue(IOPinList::PinIndex const index, bool const  markRead)
{
	if(index.index_<numPins_)
	{
		IOPin *pinToRead;
		pinToRead=pinListFirstMember_;
		int indexCount;
		for(indexCount=0; indexCount<index.index_; indexCount++)
			pinToRead=pinToRead->next();
		return pinToRead->readValue(markRead);
	}
	return false;
}

//Vaihdetaan nastan tilaa jos on inputti
void IOPinList::toggleInputPin(string const name, bool const  markChanged)
{
	IOPin *pinToToggle;
	pinToToggle=pinListFirstMember_;
	while(pinToToggle!=0)
	{
		string pname=pinToToggle->readName();
		if(pname==name)
		{
			if(pinToToggle->isIn())
			{
				if(pinToToggle->readValue(true))
					pinToToggle->writeValue(false, markChanged);
				else
					pinToToggle->writeValue(true, markChanged);
				someInPinChanged_=true;
			}
			break;
		}
		pinToToggle=pinToToggle->next();
	}
}

//Vaihdetaan nastan tilaa jos on inputti
void IOPinList::toggleInputPin(IOPin::PinID const id, bool const markChanged)
{
	IOPin::PinID pid;
	IOPin *pinToToggle;
	pinToToggle=pinListFirstMember_;
	while(pinToToggle!=0)
	{
		pid=pinToToggle->readID();
		if(pid.id_==id.id_)
		{
			if(pinToToggle->isIn())
			{
				if(pinToToggle->readValue(true))
					pinToToggle->writeValue(false, markChanged);
				else
					pinToToggle->writeValue(true, markChanged);
				someInPinChanged_=true;
			}
			break;
		}
		pinToToggle=pinToToggle->next();
	}
}

void IOPinList::saveWorkDataToFile(QDataStream& saveFileStream)
{
	IOPin::PinID pid;
	IOPin *pinToSave;
	pinToSave=pinListFirstMember_;
	while(pinToSave!=0)
	{
		pid=pinToSave->readID();
		Q_INT32 pidVal=pid.id_;
		saveFileStream<<(Q_UINT16)0<<(Q_UINT8)KSimu51::QUINT32<<pidVal;
		saveFileStream<<(Q_UINT16)1<<(Q_UINT8)KSimu51::QUINT8<<(Q_UINT8)pinToSave->readValue(false);
		saveFileStream<<(Q_UINT16)KSimu51_NameS::INDEX_SAVEDWORKDATA_END;

		pinToSave=pinToSave->next();
	}
}

void IOPinList::readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead)
{
	Q_UINT16 indexReadData;
	Q_UINT8 typeReadData;

	Q_UINT8 value8bool;
	Q_INT32 valueI32;

	IOPin::PinID pid;
	IOPin *pinToRead;
	pinToRead=pinListFirstMember_;
	while(pinToRead!=0)
	{
		readFileStream>>indexReadData>>typeReadData>>valueI32;
		pid.id_=valueI32;
		pinToRead->changeID(pid);

		readFileStream>>indexReadData>>typeReadData>>value8bool;
		pinToRead->writeValue((bool)value8bool,false);

		readFileStream>>indexReadData; //INDEX_SAVEDWORKDATA_END

		pinToRead=pinToRead->next();
	}
}
