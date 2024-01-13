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
#ifndef LOGIC74COMPONENTS_H
#define LOGIC74COMPONENTS_H

//Käyttöluokka:
//Tähän luokkaan kastaan kaikki LOGIC74 luokan jäsenet.

#include "../Components.h"

#include "Logic74B.h"
#include "Gate2NANDW.h" //00,01,03..
#include "Gate2ANDW.h"
#include "Gate2NORW.h"
#include "Gate2ORW.h"
#include "Decoder3To8W.h"
#include "BusTransceiver8W.h"
#include "Latch8W.h"
#include "GateInverterW.h"


class Components;

class Logic74BaseW;

class Logic74Components : public Components
{
	public:
		Logic74Components();
		~Logic74Components();

		void addMenu(QWidget* parent, KPopupMenu* popupMenu) override;

		#define COMPONENTGROUPNAME_LOGIC74 "logic74Components"
		enum logic74Type
		{
			GATES_2NAND,
			GATES_2AND,
			GATES_2NOR,
			GATES_2OR,
			DECODER3TO8,
			BUSTRANSCEIVER8,
			LATCH8,
			INVERTER
		};

	protected:


		ExternalComponentBaseW* addComponent_VirtualPart(QWidget * parent, int const  type, int const  componentIndexCounter, QString& indexString) override;

};

#endif

