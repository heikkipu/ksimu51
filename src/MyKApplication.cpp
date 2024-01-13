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
#include "MyKApplication.h"

bool MyKApplication::x11EventFilter(XEvent *e)
{
	//Hiiren vasemman-napin painalluseventti.
	if((e->xbutton.type==ButtonPress)&&(e->xbutton.button==1))
	{
		QPoint p = QPoint(e->xbutton.x_root,e->xbutton.y_root);
		emit x11LeftMouseButtonPressed(&p); //Signaali siitä
	}
	return false;
}


