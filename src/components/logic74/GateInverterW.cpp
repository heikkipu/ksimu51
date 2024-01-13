/**************************************************************************
*   Copyright (C) 2024 - ... Heikki Pulkkinen                             *
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
#include "GateInverterW.h"


GateInverterW::GateInverterW(QWidget* parent, char const * name, int wflags, unsigned int const  componentNumber, unsigned int const  componentType) : Logic74BaseW(parent, name, wflags, componentNumber, componentType)
{
	componentName_+=" Inverter Gate [";
	delayTimeTyp_=DELAYTIME;

	loadComponentXPMImage();
	componentImageQLabel_->setPixmap(componentImage_);

	//Input pins
	addInputPin(PINNAME_1A);
	addOutputPin(PINNAME_1Y);

	pTo1APin_=ioList_.pToPin(PINNAME_1A);
	pTo1YPin_=ioList_.pToPin(PINNAME_1Y);

	initW(); //

}


GateInverterW::~GateInverterW()
{

}


bool GateInverterW::logic1Operation(IOPin* inA )
{
	return  !inA->readValue(true);
}

void GateInverterW::execute_VirtualPart_ExecuteInputChange_ExecutionReady_VirtualPart(void)
{
	pTo1YPin_->writeValue(logic1Operation(pTo1APin_), true);
}

void GateInverterW::loadComponentXPMImage(void)
{
/* XPM */
static char const  *logicXPM[]={
"105 22 7 1",
"# c #993333",
"c c #996666",
"b c #cc6666",
"e c #cc9999",
"d c #cccccc",
"a c #ffcccc",
". c #ffffff",
".........................................................................................................",
"............................#####################################d.......................................",
"............................#eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee#d.......................................",
"............................#d..................................#dd......................................",
"............................#d..............db##d...............#d#d.....................................",
"............................#d.............a##b#d...............#d##d....................................",
"............................#d................a#d...............#d.##d...................................",
"............................#d................a#d...............#d..##d..................................",
"............................#d................a#d...............#d...##d.................................",
"............................#d................a#d...............#d....##d................................",
"#############################d................a#d...............#d#######################################",
"eeeeeeeeeeeeeeeeeeeeeeeeeeee#d................a#d...............#deeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeee",
"............................#d..............ddd#edd.............##.......................................",
"............................#d..............#######c............##.......................................",
"............................#d..................................#d.......................................",
"............................#d..................................#d.......................................",
"............................#d..................................#d.......................................",
"............................#d..................................#d.......................................",
"............................#d..................................#d.......................................",
"............................#####################################d.......................................",
"............................eeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeed.......................................",
"........................................................................................................."};

	componentImage_=QPixmap(logicXPM);

}

