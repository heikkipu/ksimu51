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
#ifndef LOGICGALPALCOMPONENTS_H
#define LOGICGALPALCOMPONENTS_H

//Käyttöluokka:
//Tähän luokkaan kastaan kaikki LOGIC74 luokan jäsenet.

#include "../Components.h"

#include "Gal22V10W.h"
#include "IspGAL22V10W.h"


class Components;

class LogicGALPALComponents : public Components
{
	public:
		LogicGALPALComponents();
		~LogicGALPALComponents();

		void addMenu(QWidget* parent, KPopupMenu* popupMenu) override;

		#define COMPONENTGROUPNAME_LOGICGALPAL "logicGALPALComponents"
		enum logicGALPALType
		{
			GAL22V10,
			ISPGAL22V10,
		};

	protected:

		ExternalComponentBaseW* addComponent_VirtualPart(QWidget * parent, int const  type, int const  componentIndexCounter, QString& indexString) override;

};

#endif

