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
#ifndef ANALOGINPUTPINLIST_H
#define ANALOGINPUTPINLIST_H

#include <string>
#include <functional>
#include <qdatastream.h>

#include "AnalogInputPin.h"

class AnalogInputPinList
{
	public:
		AnalogInputPinList(void);
		virtual ~AnalogInputPinList(void);
		void writeValue(string name, double const  fVal); //Asetataan pinnin arvo nimellä
		void writeValue(int const index, double const  fVal); //Asetataan pinnin arvo indexillä
		bool isChangedValues(void){return changedValues_;};//Voidaan päivitystä varten katsoa onko arvoa/ja muutettu
		double readValue(string name); //Luetaan pinnin arvo nimellä
		double readValue(int const index);

		int readIndex(string name);

		AnalogInputPin* readPins(void); //
		//Luokka joka on listarakenne.

		void appendPin(string const name, double const  fVal, double const  minVal, double const  maxVal);//oletus arvot

		//Pinnitilojen tallennus work fileen. Ei ANSI mukainen vaan QT
		void saveWorkDataToFile(QDataStream& saveFileStream);
		void readWorkDataFromFile(QDataStream& readFileStream, std::function<bool( QDataStream& readFileStream)>funcHandleUnknownIndexRead);

	protected:
	private:
		AnalogInputPin *pinList_; //Osoitin listan alkuun
		AnalogInputPin *pinListLastMember_;   //Osoitin viimeismpään jäseneneen
		bool firstPinToAdd_; //Pinnilistan aloitus merkki
		bool changedValues_;
};

#endif
