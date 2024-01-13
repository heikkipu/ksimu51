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
#include "MAX7651.h"

//Muodostin
MAX7651::MAX7651(): MAX765x(INTERNAL_PROGRAM_MEMORY_SIZE)
{
	processorName_="MAX7651";
	createAnalogInputPins();
}

//Hajotin
MAX7651::~MAX7651()
{
}

void MAX7651::createAnalogInputPins(void)
{
	//ANALOGISET INPUTPINNIT uusi määritys 3.3V
	analogInputPins_.appendPin(PINNAME_AIN0, 1.0, 0.0, 5.0);
	analogInputPins_.appendPin(PINNAME_AIN1, 1.0, 0.0, 5.0);
	analogInputPins_.appendPin(PINNAME_AIN2, 1.0, 0.0, 5.0);
	analogInputPins_.appendPin(PINNAME_AIN3, 1.0, 0.0, 5.0);
	analogInputPins_.appendPin(PINNAME_AIN4, 1.0, 0.0, 5.0);
	analogInputPins_.appendPin(PINNAME_AIN5, 1.0, 0.0, 5.0);
	analogInputPins_.appendPin(PINNAME_AIN6, 1.0, 0.0, 5.0);
	analogInputPins_.appendPin(PINNAME_AIN7, 1.0, 0.0, 5.0);
	analogInputPins_.appendPin(PINNAME_ACOM, 2.5, 0.0, 5.0);
	analogInputPins_.appendPin(PINNAME_VREFP, 5.0, 0.0, 5.0);
	analogInputPins_.appendPin(PINNAME_VREFN, 0.0, 0.0, 5.0);
	aPinAIN0_Index_=analogInputPins_.readIndex(PINNAME_AIN0);
	aPinAIN1_Index_=analogInputPins_.readIndex(PINNAME_AIN1);
	aPinAIN2_Index_=analogInputPins_.readIndex(PINNAME_AIN2);
	aPinAIN3_Index_=analogInputPins_.readIndex(PINNAME_AIN3);
	aPinAIN4_Index_=analogInputPins_.readIndex(PINNAME_AIN4);
	aPinAIN5_Index_=analogInputPins_.readIndex(PINNAME_AIN5);
	aPinAIN6_Index_=analogInputPins_.readIndex(PINNAME_AIN6);
	aPinAIN7_Index_=analogInputPins_.readIndex(PINNAME_AIN7);
	aPinACOM_Index_=analogInputPins_.readIndex(PINNAME_ACOM);
	aPinVREFP_Index_=analogInputPins_.readIndex(PINNAME_VREFP);
	aPinVREFN_Index_=analogInputPins_.readIndex(PINNAME_VREFN);

}

