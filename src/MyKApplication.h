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
#ifndef MYKAPPLICATION_H
#define MYKAPPLICATION_H

#include <kapplication.h>
#include </usr/include/X11/Xlib.h>      //X11Events
#include <qpoint.h>

//Tämä luokka on sitä varten, että saadaan siepattua X11 eventti koko applicatiolle.
class MyKApplication : public KApplication
{
	Q_OBJECT
	public:
		MyKApplication():KApplication(){;};
		~MyKApplication(){;};

		bool x11EventFilter(XEvent *e);

	signals:
		void x11LeftMouseButtonPressed(QPoint *);
};

#endif
