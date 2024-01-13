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
#ifndef COMPONENTS_H
#define COMPONENTS_H

//Kantaluokka component menulle ja luontiluokalle.

#include "ExternalComponentBaseW.h"

#include <kpixmap.h>
#include <kpopupmenu.h>

class Components
{
	public:
		Components();
		~Components();

		virtual void addMenu(QWidget* parent, KPopupMenu* popupMenu){;};
		ExternalComponentBaseW* addComponent(QWidget* parent, int const  type, int const  componentIndexCounter);

	protected:

		virtual ExternalComponentBaseW* addComponent_VirtualPart(QWidget* parent, int const  type, int const  componentIndexCounter, QString& indexString){return nullptr;};
		KPixmap iconKPixmap;

};

#endif

