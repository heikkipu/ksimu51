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
#ifndef ANALOGINPUTPIN_H
#define ANALOGINPUTPIN_H

#include <string>
#include "Global_NameS.h"

using namespace Global_NameS;

class AnalogInputPin
{
	public:
		AnalogInputPin(void);
		AnalogInputPin(string pName, double const  fV, double const  minV, double const  maxV);
		virtual ~AnalogInputPin(void);

		double readValue(void);
		double readMin(void);
		double readMax(void);
		void writeValue(double const  fV);
		string readName(void);

		AnalogInputPin* nextAnalogInputPin_;   //Mahdollistaa listarakenteen.

	private:
		string name_;      //Voidaan lukea, mutta ei vaihtaa. Konstruktorissa asetetaan.
		double value_;
		double minValue_;   //Voidaan lukea, mutta ei vaihtaa. Konstruktorissa asetetaan.
		double maxValue_;   //Voidaan lukea, mutta ei vaihtaa. Konstruktorissa asetetaan.
};

#endif
