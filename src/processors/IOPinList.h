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
#ifndef IOPINLIST_H
#define IOPINLIST_H

#include <string>
#include <functional>

#include <qdatastream.h>

#include "IOPin.h"

class IOPinList
{
	public:
		//Tyyppimuunnos ylikuormitusta varten?!?!?!?
		class PinIndex{public:int index_;};
		PinIndex NO_INDEX(void){return NO_INDEX_;}
		
		IOPinList(void);
		IOPinList(IOPin* sourceFirstPin);
		virtual ~IOPinList(void);
		void copyListValues(IOPinList* sourcePinList);
		//ID on pinnin identifionumero = annettu luku
		//Index on paikka listassa = nopein tapa käsitellä pinniä
		void writeInputPin(string const name, bool const  pinValue, bool const  markChanged); //Asetetaan yksittäinen pinnin nimen mukaan ulkoisesti huomioi pinnin suunnan.
		void writeInputPin(IOPin* pointerToPin, bool const  pinValue, bool const  markChanged); //Asetetaan yksittäinen pinnin indeksin mukaan ulkoisesti huomioi pinnin suunnan.
		//void writeInputPinWithIndex(int const index, bool const  pinValue, bool const  markChanged); //Asetetaan yksittäinen pinnin indeksin mukaan ulkoisesti huomioi pinnin suunnan.
		bool readValue(string const name, bool const  markRead); //Luetaan pinnin arvo nimellä
		bool readValue(IOPin::PinID const  id, bool const  markRead); //Luetaan pinnin arvo ID:llä
		bool readValue(IOPinList::PinIndex const index, bool const  markRead); //Luetaan pinnin arvo indexillä
		void toggleInputPin(string const name, bool const  markChanged); //Vaihdetaan pinnin tila nimellä
		void toggleInputPin(IOPin::PinID const id, bool const  markChanged); //Vaihdetaan pinnin tila indeksillä
		bool setDirection(string const name, bool const  dir); //Asetetaan yksittäinen pinnin suunnan nimen mukaan jos bidir
		bool setDirection(IOPin::PinID const id, bool const  dir); //Asetetaan yksittäinen pinnin suunnan indeksin mukaan jos bidir
		IOPin* pToFirstPin(void); //Pointteri pinnilistan eka pinniin.
		IOPin* pToPin(string const name);   //Pointteri pinnin nimellä. Palauttaa 0 jos ei onnaa
		IOPin* pToPin(IOPin::PinID const id);   //Pointteri id:llä. Palauttaa 0 jos ei onnaa.
		IOPin* pToPin(IOPinList::PinIndex const index);   //Pointteri indexillä. Palauttaa 0 jos ei onnaa.
		IOPinList::PinIndex readIndex(string const name); //Lukee paikan listassa. -1= ei listassa.
		IOPin::PinID readID(string const name); //Lukee pinnin ID:n;
		IOPin::PinID readID(IOPinList::PinIndex const index); //Lukee ID:n indexillä.
		bool ifSomeInPinChanged(bool const  markChecked); //Merkki jonkun inputpinnin tilan muuttumisesta. Voidaan käyttää updateissa. Outputista ei tartte, koska tiedetään milloin tila muuttuu. Voidaan päivittää heti.

		//Yksittäise I/O pinnit joilla on toiminnan kannalta merkitystä.
		//Luokka joka on listarakenne.
		unsigned int numberOfPins(void){return numPins_;};
		void addPin(string const name, bool const value, IOPin::PinTYPE const pinType);//oletus arvot
		void addPin(IOPin::PinID const id, string const name, bool const value, IOPin::PinTYPE const pinType);//oletus arvot
		void setPin(string const name, bool const pinValue, bool const markChanged); //Asetetaan yksittäinen pinnin arvo nimen mukaan sisäisesti
		void setPin(IOPin::PinID const id, bool const pinValue, bool const markChanged); //Asetetaan yksittäinen pinnin arvo indeksin mukaan sisäisesti

		void operator=(const IOPinList* l);

		//Pinnitilojen tallennus work fileen. Ei ANSI mukainen vaan QT
		void saveWorkDataToFile(QDataStream& saveFileStream);
		void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead);

	protected:
	private:
		IOPin *pinListFirstMember_; //Osoitin listan alkuun
		IOPin *pinListLastMember_;   //Osoitin viimeismpään jäseneneen
		bool firstPinToAdd_; //Pinnilistan aloitus merkki
		unsigned int numPins_;
		PinIndex NO_INDEX_;
		bool someInPinChanged_; //Merkki jonkun inputpinnin tilan muuttumisesta. Voidaan käyttää updateissa.
};

#endif
