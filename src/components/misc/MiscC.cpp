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
#include "MiscC.h"

#include <qwidget.h>
#include <kpopupmenu.h>
#include "../ExternalComponentBaseW.h"


MiscComponents::MiscComponents() : Components ()
{
}

MiscComponents::~MiscComponents()
{
}

void MiscComponents::addMenu(QWidget * parent, KPopupMenu* popupMenu)
{
}

ExternalComponentBaseW* MiscComponents::addComponent_VirtualPart(QWidget * parent, int const  type, int const  componentIndexCounter, QString& indexString)
{
	ExternalComponentBaseW* pointerToEDMCW;
}

